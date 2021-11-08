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

    void EnqueueJob(function<void()> job);      // job 을 추가한다.

private:
    size_t num_threads_;                        // 총 Worker 쓰레드의 개수.
    vector<thread> worker_threads_;             // Worker 쓰레드를 보관하는 벡터.
    queue<function<void()>> jobs_;              // 할일들을 보관하는 job 큐.
    condition_variable cv_job_q_;               // 위의 job 큐를 위한 cv 와 m.
    mutex m_job_q_;

    bool stop_all;                              // 위의 job 큐를 위한 cv 와 m.

    void WorkerThread();                        // Worker 쓰레드
};

ThreadPool::ThreadPool(size_t num_threads) : num_threads_(num_threads), stop_all(false)
{
    worker_threads_.reserve(num_threads_);
    for (size_t i = 0; i < num_threads_; ++i)
    {
        worker_threads_.emplace_back([this]() { this->WorkerThread(); });
        //worker_threads_가 vector 자료형이지만 내부에 있는건 Thread변수형식에 함수를 집어넣은거라
        //num_threads의 갯수만큼 스레드가 돌아가고 있는것이다.
    }
} //이거야 뭐 벡터공간 num_threads갯수만큼 마련해주는거니 크게 상관없을거고.

void ThreadPool::WorkerThread()
{
    while (true)
    {
        unique_lock<mutex> lock(m_job_q_);
        cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });
        //wait는 등록이 완료될때까지 기다린다. 스레드가. 등록완료 신호는
        //endqueue함수에서 notifyone호출시 enqueue를 점유하고있는 스레드에서 하나씩 등록해 주겠지
        //예상한대로 enqueue는 mutex가 걸려있다. 스레드가 중복입력되면 안되니.....
        if (stop_all && this->jobs_.empty())
        {
            return;
        }

        // 맨 앞의 job 을 뺀다.
        function<void()> job = move(jobs_.front());//enquque 함수에서 등록한 함수를하나빼온다.
        //worker_threads_에 이는 각 스레드의  실행순서에 따라 랜덤순서로 맨앞에 있는놈 하나 빼올 가능성이 크다.

        jobs_.pop();
        lock.unlock();//빼오는건 한번에 한놈씩 빼와야 중복이 없을테니 여기서 unlock이 될거라고 예상했다.

        // 해당 job 을 수행한다 :)
        job();
    }
}

ThreadPool::~ThreadPool()
{
    stop_all = true;
    cv_job_q_.notify_all();

    for (auto& t : worker_threads_)
    {
        t.join();//파괴하면 당연히 돌아가던 스레드들은 메인메소드에 합쳐야지.
    }
}

void ThreadPool::EnqueueJob(function<void()> job)
{
    if (stop_all)
    {
        throw runtime_error("ThreadPool 사용 중지됨");
    }
    {
        lock_guard<mutex> lock(m_job_q_);
        jobs_.push(move(job));
    }
    cv_job_q_.notify_one();//한번에 하나씩 랜덤으로.... 음.. worker_threads_ 벡터에 있는놈 하나씩 돌리겠지.
    //cv_job_q_.wait 에서 봅시다.
    //암묵적인 unlock 이 여기 있을듯.
}