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

	1. ��� �� ����Ʈ ���(�������� ������ ����Ʈ ��ȯ.  �� ����Ʈ �������� ��ȯ)
	2. ���� ����( �������� �������� Ȯ�� �޼��� ��ȯ)
	3. ��ȭ�� ����( �������� ���ӿϷ� �޼��� ��ȯ)
	4. ��ȭ�� ����(�������� �����Ϸ� �޼��� ��ȯ)
	5. ��� �޼��� ����.

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

	int clientfd;//���߿����߿����߿�.
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
	����
	for(clientInfo a : clientList){
		a.Roomnum=��¼��......
	}	
	*/

	winServerSocket();
	~winServerSocket();

private:	
	

	void connectClient(SOCKET sock);	
	int recvn(SOCKET s, char* buf, int len, int flags);//����� ���� ������ �����Լ�
	void waitFunction(SOCKET sock);
	int previousRoomNum;
	struct message structReceive(SOCKET sock);//���� ����ü ����
	void structSend(SOCKET sock, message msg);//���� ����ü �۽�
	void clientStructPrint_(clientInfo);//����ü ���.
	void msgStructPrint_(message a);
	



};

