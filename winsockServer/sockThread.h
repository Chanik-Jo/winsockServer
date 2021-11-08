#pragma once
#include <iostream>
#include <thread>


using namespace std;

class socketThread {

public:

	int sockfd;
	thread* clientThread; //넣고싶은 함수는 void 타입의 
	//winServerSocket::connectClient(SOCKET sock) 이다아
	// //생각해보니 clientconnect가 아닌 waitfunction을 불러와야 ...하나?
	//그렇다고 함수를 복제하는것도 아니고 포인터로 날리면
	//critical section 문제가 씨게 생기려나? 함수를 shared 해버리면 문제가 200% 생길테니.
	//설령 함수를 간접참조가 아닌 "복사"로 매개변수에 넣는다고 해도,
	//그함수가 단일로 끝나는게 아니고 그 함수도 내부적으로 다른 함수를 불러오는데.
	//어쩌지?????
	//그렇다고 포인터로 불러온 함수에 mutex를 걸면 퍼포먼스는 시궁창으로 떨어진다.
	//function<void()> job 이런식으로 불러오면 함수가 복제되어 들어갈까?
	//이 기법이 안전한지 확인하는데 그리고 무슨 결과를 낼지 예상하는데 타이핑하는시간보다
	//더 많은 시간이 소모되는듯 하다.

	socketThread(void(*clientconnect)()) {
		//스레드 선언형식 sendOpen = thread(&winsocket::sending, this, sock);
		this->sockfd = sockfd;
		clientThread = new thread(clientconnect,this,sockfd); //this가 가르키는건 this다음의
		//parameter가 socketThread 클래스의 그것임을 의미하는건...가?

		//공식문서 왈 '&'없이 그냥 적어주면 클래스는 복제가 된다고는 하는데...
		///함수 (함수(함수)) 이런경우도 오토로 복사가 되....나?
		

	}

	//소멸자는 따로 선언하지 않음 알아서 clientThread도 메모리에서 날아갔으면....




};

