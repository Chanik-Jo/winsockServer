#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "ThreadPool.h"

//#include <windows.h>
#include <winsock2.h>
//#include <ws2tcpip.h>

#include <string>
#include <cstring>
#include <thread>
#include <vector>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include <WS2tcpip.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PACKET_SIZE 1024

using namespace std;

struct message {

	int functionType;
	/*

	1. 모든 방 리스트 출력(서버에서 접속자 리스트 반환.  방 리스트 서버에서 반환)
	2. 접속 종료( 서버에서 접속종료 확인 메세지 반환)
	3. 대화방 선택( 서버에서 접속완료 메세지 반환)
	4. 대화방 생성(서버에서 생성완료 메세지 반환)
	5. 통상 메세지 전송.

	*/
	char nickname[32];
	int Roomnum;
	char strBuffer[1000];
	

	message() {
		functionType = 0;
		memset(nickname, 0, 32);
		memset(strBuffer, 0, 1000);
		Roomnum = -1;
	}


};
struct clientInfo {

	int clientfd;//최중요최중요최중요.
	char nickname[32];
	bool active;
	int Roomnum;
	clientInfo() {
		clientfd = 0;
		memset(nickname, 0, 32);
		active = false;
		Roomnum = -1;
	}

};





class winServerSocket
{
public:
	WSADATA wsadata;
	SOCKET sock ;
	SOCKADDR_IN ServAddr,CliAddr;
	int newClientFd, cliLen = sizeof(CliAddr);
	vector<clientInfo> clientList;
	/*
	사용법
	for(clientInfo a : clientList){
		a.Roomnum=어쩌고......
	}	
	*/

	winServerSocket();
	~winServerSocket();

private:	
	

	void connectClient(SOCKET sock);	
	int recvn(SOCKET s, char* buf, int len, int flags);//사용자 정의 데이터 수신함수
	void waitFunction(SOCKET sock);
	int previousRoomNum;
	struct message structReceive(SOCKET sock);//실제 구조체 수신
	void structSend(SOCKET sock, message msg);//실제 구조체 송신
	void clientStructPrint_(clientInfo);//구조체 출력.
	void msgStructPrint_(message a);
	



};

