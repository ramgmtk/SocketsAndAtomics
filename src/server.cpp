#include <iostream>
#include <stdio.h>
//help from user fpiette https://stackoverflow.com/questions/67726142/how-can-i-use-sys-socket-h-on-windows
#include <winsock2.h>

//credit for help understanding winssock2 https://www.tenouk.com/Winsock/Winsock2example2.html

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
    //create a socket that is read to accept a connection
    SOCKET AcceptSocket;
    std::cout << "Server: Waiting for client to connect...\n";
    //run a loop to check for connection
    while(true) {
        AcceptSocket = SOCKET_ERROR;
        while (AcceptSocket == SOCKET_ERROR) {
            AcceptSocket = accept(m_socket, NULL, NULL);
        }
        //transfer control from temporary socket to the original socket, m_socket, and stop checking for connection.
        std::cout << "Client Connected." << std:: endl;
        m_socket = AcceptSocket;
        break;
    }

    closesocket(m_socket);
    WSACleanup();
    system("pause");
    return 0;
}