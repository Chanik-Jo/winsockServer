#pragma once
#include <iostream>
#include <thread>


using namespace std;

class socketThread {

public:

	int sockfd;
	thread* clientThread; //�ְ���� �Լ��� void Ÿ���� 
	//winServerSocket::connectClient(SOCKET sock) �̴پ�
	// //�����غ��� clientconnect�� �ƴ� waitfunction�� �ҷ��;� ...�ϳ�?
	//�׷��ٰ� �Լ��� �����ϴ°͵� �ƴϰ� �����ͷ� ������
	//critical section ������ ���� �������? �Լ��� shared �ع����� ������ 200% �����״�.
	//���� �Լ��� ���������� �ƴ� "����"�� �Ű������� �ִ´ٰ� �ص�,
	//���Լ��� ���Ϸ� �����°� �ƴϰ� �� �Լ��� ���������� �ٸ� �Լ��� �ҷ����µ�.
	//��¼��?????
	//�׷��ٰ� �����ͷ� �ҷ��� �Լ��� mutex�� �ɸ� �����ս��� �ñ�â���� ��������.
	//function<void()> job �̷������� �ҷ����� �Լ��� �����Ǿ� ����?
	//�� ����� �������� Ȯ���ϴµ� �׸��� ���� ����� ���� �����ϴµ� Ÿ�����ϴ½ð�����
	//�� ���� �ð��� �Ҹ�Ǵµ� �ϴ�.

	socketThread(void(*clientconnect)()) {
		//������ �������� sendOpen = thread(&winsocket::sending, this, sock);
		this->sockfd = sockfd;
		clientThread = new thread(clientconnect,this,sockfd); //this�� ����Ű�°� this������
		//parameter�� socketThread Ŭ������ �װ����� �ǹ��ϴ°�...��?

		//���Ĺ��� �� '&'���� �׳� �����ָ� Ŭ������ ������ �ȴٰ�� �ϴµ�...
		///�Լ� (�Լ�(�Լ�)) �̷���쵵 ����� ���簡 ��....��?
		

	}

	//�Ҹ��ڴ� ���� �������� ���� �˾Ƽ� clientThread�� �޸𸮿��� ���ư�����....




};

