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
//���� 2�� �޼ҵ�� �α�����.

int winServerSocket::recvn(SOCKET s, char* buf, int len, int flags)
{

    int received; // recv() �Լ��� ���� ���� ������ ����
    char* ptr = buf;// ���� ���α׷� ������ ���� �ּ�. �����͸� ���� ������ ptr���� ���� ��Ų��.
    int left = len;// ���� ���� ���� ������ ũ���. �����͸� ���� ������ left�� ���� �Ѵ�.

    while (left > 0) // ���� ���� �ʴ� �����Ͱ� ������ ��� ������ ����. 
    {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        // recv �Լ��� ȣ���� ������ �߻��ϸ� ��ٷ� ���� �����Ѵ�.

        else if (received == 0)
            break;
        //�Լ��� ���� �����ϸ� �������� ���´�.

        left -= received;
        ptr += received;
        //�����͸� ���� ������ ���� , ���Ҹ� �ϸ鼭 ��Ȯ�� ����Ʈ ���� �����ϱ� ����.
    }

    return (len - left);//���� ����Ʈ ���� ����
}
winServerSocket::winServerSocket() {

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        puts("WSAStartup ����.");

    }//��ó: https://maincodes.tistory.com/74 [mainCodes:: ��������]

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        puts("socket ����.");

    }
    memset(&ServAddr, 0, sizeof(ServAddr));
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY);//��¥�� ������ localhost �Ƴ�?
    ServAddr.sin_port = htons(portnum);



    if (::bind(sock, (struct sockaddr*)&ServAddr, sizeof(ServAddr)) < 0)
    {
        puts("bind ����");

    }

    if (listen(sock, SOMAXCONN) < 0)
    {
        puts("listen ����");

    }

    while (1) {
        cout << "accept waiting" << endl;
        newClientFd = accept(sock, (struct sockaddr*)&CliAddr, &cliLen);
        if (newClientFd < 0) {
            perror("ERROR accept connection");
        }
        else {
            //�̰� ������� �и��ؾ��ϴµ� 
            //Ŭ���� �迭�� ��ũ���� �ѹ��� �Լ������� �Ѱ���� �ϳ�?

            cout << "connect success" <<newClientFd << endl;
            connectClient(newClientFd);
        }

    }

}



void winServerSocket::connectClient(SOCKET sock) {
    
    //�����߰�. ��¥�� accept�� �ѹ� ���������� 
    //�ѹ��� ȣ��ȴ�.
    
    bool emptyUsr = false;
    //���� ������ �� ���� �ִٸ�
    for (clientInfo a : clientList) {
        if (a.active == false) {
            a.active = true;
            a.clientfd = sock;
            strcpy_s(a.nickname, "");
            //lvalue �����ߴµ� ���� ���ڿ� �ְ������ strcpy ����! 
            //https://forums.codeguru.com/showthread.php?503752-Expression-must-be-a-modifiable-lvalue
            a.Roomnum = 0;
            emptyUsr = true;//���ڸ��� �����ߴ�.
            break;//���ڸ� ������ ���̻� ã������.
        }        
    }

    if (emptyUsr == false) {//���ڸ��� ��ã�ų� Ȥ�� ������ ����.
        //���� ����Ʈ�� �� ������ ���ٸ�.
        clientInfo newClient;
        newClient.active = true;
        newClient.clientfd = sock;
        strcpy_s(newClient.nickname, "");
        //lvalue �����ߴµ� ���� ���ڿ� �ְ������ strcpy ����! 
        //https://forums.codeguru.com/showthread.php?503752-Expression-must-be-a-modifiable-lvalue
        newClient.Roomnum = 0;
        clientList.push_back(newClient);
        //���ο� �����߰��� ��Ƽ������ ����.
    }
      
    //���� �۾���Ʈ read/write.
    //�̺κ��� ���ѷ������� �ҵ��ϴ�.

    //waitFunction(sock);

    /*using waitFunction = void(*)();
    clientTerminal.push_back(new socketThread(waitFunction));*/
    thread (&winServerSocket::waitFunction, this, sock).detach();
    return;
    
    
}
void winServerSocket::waitFunction(SOCKET sock) {
    message msg;
    string str;
    int searchRoomNum;
    vector<int> roomExistance;
    while (1) {
        msg = structReceive(sock);

        switch (msg.functionType) {
        case 1: {// all room list send to client
            searchRoomNum = 1;
            str = "";

            for (clientInfo & a : clientList) {

                roomExistance.push_back(a.Roomnum);//�ϴ� ��������� �� ��Ȳ�� ������.
                /* str.append(to_string(a.Roomnum));
                    str.append(" room exists. \n");
                    searchRoomNum++;*/


            }
            //�׸��� �ߺ�������
            sort(roomExistance.begin(), roomExistance.end());

            set<int> s(roomExistance.cbegin(), roomExistance.cend());

            roomExistance = vector<int>(s.cbegin(), s.cend());
            //������ �̻翩���� �ٸ��� ���ε�
            for (int a : roomExistance) {
                //str.append(to_string(a));
                str.append(to_string(a));
                str.append(" room exists. \n");
                searchRoomNum++;
            }
            cout << str.c_str() << endl;
            strcpy_s(msg.strBuffer, str.c_str());



            structSend(sock, msg);

            break;

        }
        case 2: //empty client from clientlist using socketnum

            for (clientInfo & a : clientList) {
                if (a.clientfd == sock) {//����� ��ũ���� == ����� ���ϵ�ũ����
                    a.active = false;
                    //a.nickname = msg.nickname;
                    strcpy_s(a.nickname, msg.nickname);
                    //str = a.nickname;
                    cout <<"left nickname recongine print" << a.nickname << endl;
                    str.append(a.nickname);
                    
                }
            }
            cout << "server closed" << endl;

            str = str + " is left the server\n";
            cout << str<<endl;
            strcpy_s(msg.strBuffer, str.c_str());

            for (clientInfo & a : clientList) {
                if (msg.Roomnum == a.Roomnum) {//�޼����߼��ڼҼӷ� = ��ü������ ��������

                    structSend(a.clientfd, msg);
                    //������ : �� �� ������� ���ԵǾ� �����״� ���������.
                }
            }
            return;
            break;
            //Ŀ�� �׽�Ʈ

        case 3: {// set room num using corresponding socket fd.
            for (clientInfo & a : clientList) {
                if (a.clientfd == sock) {


                    str.append(" left ");
                    previousRoomNum = a.Roomnum;//
                    //������ ������.
                    str.append(to_string(previousRoomNum));//�� ���⸸ �ȳ��� 0�̵���?
                    str.append(" room ");

                    /*
                    str = a.nickname + "left"
                        + a.Roomnum.c_str() + " room\n";
                    */
                    strcpy_s(msg.strBuffer, str.c_str());
                    structSend(sock, msg);
                    
                   a.Roomnum = msg.Roomnum;// ���ȣ ���� �ٵ� �� �̰� ���� ������ ����? ���ȣ 0(�⺻��)�϶��� �߿��µ�
                   //���������� �ٽ� 0���� ���ư���.. �ФФ�
                   strcpy_s(a.nickname, msg.nickname);
                  
                                           

                    // ������ roomno ��ġ�ϴ°� ���������°� ������.

                    str.append(" -> ");
                    str.append(to_string(a.Roomnum));
                    str.append(" room\n");
                    cout << "room str result is :" << str << endl;

                    strcpy_s(msg.strBuffer, str.c_str());
                    msgStructPrint_(msg);

                }
            }
            for (clientInfo & a : clientList) {
                if (a.Roomnum == msg.Roomnum)
                    structSend(a.clientfd, msg);//������ ������ı�
                else {
                message* blankMsg = new message();
                structSend(a.clientfd, *blankMsg);
                delete blankMsg;
                }
                //�� ��ġ�ϴ� Ŭ���̾�Ʈ���� �޼��� ����.
            }



            strcpy_s(msg.strBuffer, "");
            previousRoomNum = msg.Roomnum;
            break;
        }
        case 4:{           //����. : file�� 1����Ʈ(char �ѱ��� ������)�� �ҷ��鿩 �迭�� 
           //���̳ʸ� ������ ������ȯ�ؼ� �ۼ����غ���.  �̰� �����ϴٸ� ������...!!!
           //�ùٸ��� �ȵǴ� �ļ������� ������ ����ü(...�� ������ �Ʊ��ϱ�!)�� ��Ȱ���غ���.
            
            ofstream out;
            out.open("main.gif", std::ios::binary);

            if (out.fail()) {
                cout << "error ofstream" << endl;
            }

            out.write(msg.strBuffer, msg.fileSize);

            out.close();

           
            for (clientInfo& a : clientList) {
                if (a.clientfd == sock) {
                    structSend(sock, msg);
                }
            }
            strcpy_s(msg.strBuffer, "server has been received file");
            structSend(sock, msg);
            
            break;       
         }
              

        
        case 5:
            cout << "msg struct print" << endl;
            msgStructPrint_(msg);
            for (clientInfo& a : clientList) {
                if (a.Roomnum == msg.Roomnum)
                    clientStructPrint_(a);
                    structSend(a.clientfd, msg);//������ ������ �ı�.
                //�� ��ġ�ϴ� Ŭ���̾�Ʈ���� �޼��� ����.
            }

            break;

        default:
            break;

        }
        

    }


}






void winServerSocket::structSend(SOCKET sock, message msg) {
    //int iferr = send(sock, (char*)&msg, sizeof(msg), 0);//ã�ƺôµ� ���� ������ �ϴ�.
    // https://windowshyun.tistory.com/5
    //if (iferr <= 0)
        //cout << "some error happened" << endl;//iferr�� ����Ʈ���� �������̹Ƿ�.

    int retval;
    // ������ ������( ����ü ũ�⸦ ���� ������. )
    int len = sizeof(msg);
    retval = send(sock, (char*)&len, sizeof(int), 0);
    if (retval == SOCKET_ERROR) {
        cout << "send()" << endl;
        exit(1);
    }
    // ������ ������( ����ü �����͸� ������. )
    retval = send(sock, (char*)&msg, sizeof(message), 0);
    if (retval == SOCKET_ERROR) {
        cout << "send()" << endl;
        exit(1);

    }


    //��ó: https://windowshyun.tistory.com/5 [;]



    return;

}
struct message winServerSocket::structReceive(SOCKET sock) {
    int retval;
    int len;

    retval = recvn(sock, (char*)&len, sizeof(int), 0); // ������ �ޱ�(���� ����)
    if (retval == SOCKET_ERROR) {
        cout << " len recvn(SOCKET_ERROR)" << endl;

    }

    int GetSize;
    //char suBuffer[1500];//����ũ��� ����ü�� �� ���� ���������� �����Ӱ�.
    char *suBuffer=new char[len];//������� �ϴ� recvn���� ����� �����Ƿ�.
    //�ƴ� �װͺ��� 1500�� ���̵� retval�� ���� �ϴ°� �Ƴ�?
    message* msg;
    GetSize = recv(sock, suBuffer, len, 0);
    if (GetSize == SOCKET_ERROR) {
        cout << " msg recvn(SOCKET_ERROR)" << endl;
        //������ ������ �������� �̰� ȣ��Ǵ� ���⼭ exit�� ���ְ�, 
        //list ���� active�� �����ص� �ɵ��ϴ�.
        exit(1);
    }

    suBuffer[GetSize] = '\0';
    msg = (message*)suBuffer;

    return *msg;


    //��ó: https://windowshyun.tistory.com/5 [;]

}
