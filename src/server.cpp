#include <iostream>
#include <stdio.h>
#include <windows.h>
//help from user fpiette https://stackoverflow.com/questions/67726142/how-can-i-use-sys-socket-h-on-windows
#include <winsock2.h>

#define MAX_THREADS 2
#define OPERATIONS 100000
//credit for help understanding winssock2 https://www.tenouk.com/Winsock/Winsock2example2.html

DWORD WINAPI accept_clients(void* lpParam);
DWORD WINAPI prep_clients(void* lpParam);

struct socketStruct {
    SOCKET* sArray;
    SOCKET* mSocket;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Error please provide port on exe run.\n";
        return -1;
    }
    int portno = atoi(argv[1]);
    WORD wVersionR;
    WSADATA wsaData;
    //lets uc access version 2.2
    wVersionR = MAKEWORD(2, 2);
    //find a dll
    if (WSAStartup(wVersionR, &wsaData) != 0) {
        std::cout << "Error with dll version\n";
        return -1;
    }
    //check the dll supports 2.2
    //explanation of lobyte and hibyte by user jocke-i https://stackoverflow.com/questions/18033192/what-is-lobyte-in-c
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        std::cout << "Error the dll does not suuport the winsock version.\n";
        WSACleanup();
        return -1;
    }
    SOCKET m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //verify socket creation
    if (m_socket == INVALID_SOCKET) {
        std::cout << "Error at Socket\n";
        WSACleanup();
        return -1;
    }

    //binding action
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(portno);

    if (bind(m_socket, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cout << "Error bind failed\n";
        closesocket(m_socket);
        return -1;
    }
    //listen for connection
    if (listen(m_socket, 1) ==  SOCKET_ERROR) {
        std::cout << "Error listening to socket.\n" + WSAGetLastError();
    } else {
        std::cout << "Listening...\n";
    }
    //create a socket array to handle accepting multiple clients that is read to accept a connection
    SOCKET socketArray[MAX_THREADS];   
    HANDLE hThreadArray[MAX_THREADS];
    DWORD dThreadIdArray[MAX_THREADS];

    socketStruct socketPass;
    socketPass.sArray = socketArray;
    socketPass.mSocket = &m_socket;
    hThreadArray[0] = CreateThread(NULL, 0, accept_clients, &socketPass, 0, &dThreadIdArray[0]);
    //wait for client connections
    std::cout << "Server: Waiting for clients to connect...\n";
    WaitForSingleObject(hThreadArray[0], INFINITE);
    CloseHandle(hThreadArray[0]);

    //prompt clients to prepare them
    for (int i = 0; i < MAX_THREADS; i++) {
        hThreadArray[0] = CreateThread(NULL, 0, prep_clients, &socketArray[i], 0, &dThreadIdArray[i]);
    }
    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    //close sockets and close threads
    for (int i = 0; i < MAX_THREADS; i++) {
        m_socket = socketArray[i];
        closesocket(m_socket);
        //CloseHandle(hThreadArray[i]);
    }
    //closesocket(m_socket);
    WSACleanup();

    system("pause");
    return 0;
}

DWORD WINAPI accept_clients(void* lpParam) {
    socketStruct* sPass = (socketStruct*)lpParam;
    //run a loop to check for connection
    for (int i = 0; i < MAX_THREADS; i++) {
        while(true) {
           sPass->sArray[i] = SOCKET_ERROR;
            while (sPass->sArray[i] == SOCKET_ERROR) {
                sPass->sArray[i] = accept(*sPass->mSocket, NULL, NULL);
            }
            std::cout << "Clients Connected: " << i + 1 << std:: endl;
            //Receive message from incoming client.
            int bytesRecv = SOCKET_ERROR;
            char recvbuf[200] = "";
            while (bytesRecv == SOCKET_ERROR) {
                bytesRecv = recv(sPass->sArray[i], recvbuf, 32, 0);
                if (bytesRecv < 0) break;
                else {
                    std::cout << "Client " << i + 1 << " message: " << recvbuf << std::endl;
                }
            }
             //transfer control from temporary socket to the original socket, m_socket, and stop checking for connection.
            //m_socket = AcceptSocket;
            break;
        }
    }
    return 0;
}

DWORD WINAPI prep_clients(void* lpParam) {
    SOCKET* sClientSocket = (SOCKET*) lpParam;
    int bytesSent;
    char sendbuf[200] = "Server: Client enter any key to continue.\n";
    bytesSent = send(*sClientSocket, sendbuf, strlen(sendbuf), 0);
    std::cout << "Server: send() - Bytes Sent: " << bytesSent << std::endl;
    return 0;
}