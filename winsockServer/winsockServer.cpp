// winsockServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
//서버 프로그램  포트번호 7889

//명심명심 또명심
#include "winServerSocket.h"
//1클라이언트에 1스레드 할당하기.~~~~~~~~~~
int main()
{
    winServerSocket* server = new winServerSocket();




    std::cout << "Hello World!\n";
    return 0;
}
