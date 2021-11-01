#include "winServerSocket.h"

#define portnum 7889

void  winServerSocket::msgStructPrint_(message a) {
    cout << "fucntion type is " << a.functionType << endl;;
    cout << "nickname is " << a.nickname << endl;
    cout << "roomnum is " << a.Roomnum << endl;
    cout << " strbuffer is \n : " << a.strBuffer  << endl;
}
void  winServerSocket::clientStructPrint_(clientInfo a) {
    cout << "client is active " << a.active << endl;
    cout << "clientfd is " << a.clientfd << endl;
    cout << "nickname is " << a.nickname << endl;
    cout << "Roomnum is " << a.Roomnum << endl;
}
//위의 2개 메소드는 로그찍기용.

int winServerSocket::recvn(SOCKET s, char* buf, int len, int flags)
{

    int received; // recv() 함수의 리턴 값을 저장할 변수
    char* ptr = buf;// 응용 프로그램 버퍼의 시작 주소. 데이터를 읽을 때마다 ptr값을 증가 시킨다.
    int left = len;// 아직 읽지 않은 데이터 크기다. 데이터를 읽을 때마다 left을 감소 한다.

    while (left > 0) // 아직 읽지 않는 데이터가 있으면 계속 루프를 돈다. 
    {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        // recv 함수를 호출해 오류가 발생하면 곧바로 에러 리턴한다.

        else if (received == 0)
            break;
        //함수가 정상 종료하면 루프에서 나온다.

        left -= received;
        ptr += received;
        //데이터를 읽을 때마다 증가 , 감소를 하면서 정확한 바이트 수를 리턴하기 위함.
    }

    return (len - left);//읽은 바이트 수를 리턴
}
winServerSocket::winServerSocket() {

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        puts("WSAStartup 에러.");

    }//출처: https://maincodes.tistory.com/74 [mainCodes:: 메인코즈]

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        puts("socket 에러.");

    }
    memset(&ServAddr, 0, sizeof(ServAddr));
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);//어짜피 서버는 localhost 아녀?
    ServAddr.sin_port = htons(portnum);



    if (::bind(sock, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) < 0)
    {
        puts("bind 에러");

    }

    if (listen(sock, SOMAXCONN) < 0)
    {
        puts("listen 에러");

    }

    while (1) {
        cout << "accept waiting" << endl;
        newClientFd = accept(sock, (struct sockaddr*)&CliAddr, &cliLen);
        if (newClientFd < 0) {
            perror("ERROR accept connection");
        }
        else {
            //이걸 스레드로 분리해야하는데 
            //클래스 배열로 디스크립터 넘버와 함수포인터 넘겨줘야 하나?

            cout << "connect success" << endl;
            connectClient(newClientFd);
        }

    }

}



void winServerSocket::connectClient(SOCKET sock) {
    
    //유저추가. 어짜피 accept가 한번 지나갈때만 
    //한번씩 호출된다.
    
    bool emptyUsr = false;
    //기존 유저중 빈 곳이 있다면
    for (clientInfo a : clientList) {
        if (a.active == false) {
            a.active = true;
            a.clientfd = sock;
            strcpy_s(a.nickname, "");
            //lvalue 오류뜨는데 궃이 문자열 넣고싶으면 strcpy 쓰래! 
            //https://forums.codeguru.com/showthread.php?503752-Expression-must-be-a-modifiable-lvalue
            a.Roomnum = 0;
            emptyUsr = true;//빈자리가 존재했다.
            break;//빈자리 있으니 더이상 찾지마라.
        }    
    
    }

    if (emptyUsr == false) {//빈자리를 못찾거나 혹은 유저가 없다.
        //벡터 리스트에 빈 유저가 없다면.
        clientInfo newClient;
        newClient.active = true;
        newClient.clientfd = sock;
        strcpy_s(newClient.nickname, "");
        //lvalue 오류뜨는데 궃이 문자열 넣고싶으면 strcpy 쓰래! 
        //https://forums.codeguru.com/showthread.php?503752-Expression-must-be-a-modifiable-lvalue
        newClient.Roomnum = 0;
        clientList.push_back(newClient);
        //새로운 유저추가후 멀티스레드 가동.
    }

    //실질 작업파트 read/write.
    //이부분은 무한루프여야 할듯하다.

    waitFunction(sock);

    
}
void winServerSocket::waitFunction(SOCKET sock) {
    message msg;
    string str;
    int searchRoomNum;
    while (1) {
        msg = structReceive(sock);

        switch (msg.functionType) {
        case 1:// all room list send to client
            searchRoomNum = 1;
            for (clientInfo a : clientList) {
                if (a.Roomnum == searchRoomNum) {
                    str.append(to_string(a.Roomnum));
                    str.append(" room exists. \n");
                    searchRoomNum++;
                }

            }
            structSend(sock, msg);
            
            break;


        case 2: //empty client from clientlist using socketnum

            for (clientInfo a : clientList) {
                if (a.clientfd == sock) {//등록한 디스크립터 == 연결된 소켓디스크립터
                    a.active = false;
                    //str = a.nickname;
                    str.append(a.nickname);
                    
                }
            }
            cout << "server closed" << endl;

            str = str + " is left the server\n";
            cout << str<<endl;
            strcpy_s(msg.strBuffer, str.c_str());

            for (clientInfo a : clientList) {
                if (msg.Roomnum == a.Roomnum) {//메세지발송자소속룸 = 전체유저중 같은방사람

                    structSend(a.clientfd, msg);
                    //수신자 : 나 도 룸넘저가 포함되어 있을테니 상관없게지.
                }
            }
            break;

        case 3:// set room num using corresponding socket fd.
            for (clientInfo a : clientList) {
                if (a.clientfd == sock) {

                    
                    str.append(" left ");
                    previousRoomNum = a.Roomnum;
                    //별도에 변수에.
                    str.append(to_string(previousRoomNum));//왜 여기만 안남고 0이되지?
                    str.append(" room ");

                    /*
                    str = a.nickname + "left"
                        + a.Roomnum.c_str() + " room\n";
                    */
                    strcpy_s(msg.strBuffer, str.c_str());
                    structSend(sock, msg);

                    a.Roomnum = msg.Roomnum;// 방번호 갱신 

                    
                    str.append(" -> ");
                    str.append(to_string(a.Roomnum));
                    str.append(" room\n");
                    cout << "room str result is :" << str << endl;

                    strcpy_s(msg.strBuffer, str.c_str());
                    msgStructPrint_(msg);

                }
            }
            for (clientInfo a : clientList) {
                if (a.Roomnum == msg.Roomnum)
                    structSend(a.clientfd, msg);//수신자 같은방식구
                //각 일치하는 클라이언트에게 메세지 전송.
            }



            strcpy_s(msg.strBuffer, "");
            previousRoomNum = msg.Roomnum;
            break;

        case 4:
           

           break;
        
        
        case 5:
            cout << "msg struct print" << endl;
            msgStructPrint_(msg);
            for (clientInfo a : clientList) {
                if (a.Roomnum == msg.Roomnum)
                    clientStructPrint_(a);
                    structSend(a.clientfd, msg);//수신자 같은방 식구.
                //각 일치하는 클라이언트에게 메세지 전송.
            }

            break;

        default:
            break;

        }
        

    }


}






void winServerSocket::structSend(SOCKET sock, message msg) {
    //int iferr = send(sock, (char*)&msg, sizeof(msg), 0);//찾아봤는데 문제 없을듯 하다.
    // https://windowshyun.tistory.com/5
    //if (iferr <= 0)
        //cout << "some error happened" << endl;//iferr는 바이트수로 찍힐것이므로.

    int retval;
    // 데이터 보내기( 구조체 크기를 먼저 보낸다. )
    int len = sizeof(msg);
    retval = send(sock, (char*)&len, sizeof(int), 0);
    if (retval == SOCKET_ERROR) {
        cout << "send()" << endl;
        exit(1);
    }
    // 데이터 보내기( 구조체 데이터를 보낸다. )
    retval = send(sock, (char*)&msg, sizeof(message), 0);
    if (retval == SOCKET_ERROR) {
        cout << "send()" << endl;
        exit(1);
    }


    //출처: https://windowshyun.tistory.com/5 [;]



    return;

}
struct message winServerSocket::structReceive(SOCKET sock) {
    int retval;
    int len;

    retval = recvn(sock, (char*)&len, sizeof(int), 0); // 데이터 받기(고정 길이)
    if (retval == SOCKET_ERROR) {
        cout << " len recvn(SOCKET_ERROR)" << endl;
    }

    int GetSize;
    char suBuffer[1500];//버퍼크기는 구조체가 다 들어갈수 있을정도로 여유롭게.
    message* msg;
    GetSize = recv(sock, suBuffer, len, 0);
    if (GetSize == SOCKET_ERROR) {
        cout << " msg recvn(SOCKET_ERROR)" << endl;
        //소켓이 강제로 끊어져도 이게 호출되니 여기서 exit를 없애고, 
        //list 에서 active를 해제해도 될듯하다.
        exit(1);
    }

    suBuffer[GetSize] = '\0';
    msg = (message*)suBuffer;

    return *msg;


    //출처: https://windowshyun.tistory.com/5 [;]

}
