#include <chrono>
#include <condition_variable>
#include <iostream>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

class ThreadPool
{

public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();

    void EnqueueJob(function<void()> job);      // job �� �߰��Ѵ�.

private:
    size_t num_threads_;                        // �� Worker �������� ����.
    vector<thread> worker_threads_;             // Worker �����带 �����ϴ� ����.
    queue<function<void()>> jobs_;              // ���ϵ��� �����ϴ� job ť.
    condition_variable cv_job_q_;               // ���� job ť�� ���� cv �� m.
    mutex m_job_q_;

    bool stop_all;                              // ���� job ť�� ���� cv �� m.

    void WorkerThread();                        // Worker ������
};

ThreadPool::ThreadPool(size_t num_threads) : num_threads_(num_threads), stop_all(false)
{
    worker_threads_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i)
    {
        worker_threads_.emplace_back([this]() { this->WorkerThread(); });
        //worker_threads_�� vector �ڷ��������� ���ο� �ִ°� Thread�������Ŀ� �Լ��� ��������Ŷ�
        //num_threads�� ������ŭ �����尡 ���ư��� �ִ°��̴�.
    }
} //�̰ž� �� ���Ͱ��� num_threads������ŭ �������ִ°Ŵ� ũ�� ��������Ű�.

void ThreadPool::WorkerThread()
{
    while (true)
    {
        unique_lock<mutex> lock(m_job_q_);
        cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });
        //wait�� ����� �Ϸ�ɶ����� ��ٸ���. �����尡. ��ϿϷ� ��ȣ��
        //endqueue�Լ����� notifyoneȣ��� enqueue�� �����ϰ��ִ� �����忡�� �ϳ��� ����� �ְ���
        //�����Ѵ�� enqueue�� mutex�� �ɷ��ִ�. �����尡 �ߺ��ԷµǸ� �ȵǴ�.....
        if (stop_all && this->jobs_.empty())
        {
            return;
        }

        // �� ���� job �� ����.
        function<void()> job = move(jobs_.front());//enquque �Լ����� ����� �Լ����ϳ����´�.
        //worker_threads_�� �̴� �� ��������  ��������� ���� ���������� �Ǿտ� �ִ³� �ϳ� ���� ���ɼ��� ũ��.

        jobs_.pop();
        lock.unlock();//�����°� �ѹ��� �ѳ� ���;� �ߺ��� �����״� ���⼭ unlock�� �ɰŶ�� �����ߴ�.

        // �ش� job �� �����Ѵ� :)
        job();
    }
}

ThreadPool::~ThreadPool()
{
    stop_all = true;
    cv_job_q_.notify_all();

    for (auto& t : worker_threads_)
    {
        t.join();//�ı��ϸ� �翬�� ���ư��� ��������� ���θ޼ҵ忡 ���ľ���.
    }
}

void ThreadPool::EnqueueJob(function<void()> job)
{
    if (stop_all)
    {
        throw runtime_error("ThreadPool ��� ������");
    }
    {
        lock_guard<mutex> lock(m_job_q_);
        jobs_.push(move(job));
    }
    cv_job_q_.notify_one();//�ѹ��� �ϳ��� ��������.... ��.. worker_threads_ ���Ϳ� �ִ³� �ϳ��� ��������.
    //cv_job_q_.wait ���� ���ô�.
    //�Ϲ����� unlock �� ���� ������.
}