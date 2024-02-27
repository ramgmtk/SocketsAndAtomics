#include <iostream>
#include <WinSock2.h>
#include <windows.h>
//cred to websute https://www.tenouk.com/Winsock/Winsock2example2.html

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Error please provide port on exe run.\n";
        return -1;
    }
    int portno = atoi(argv[1]);
    //initialize winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cout << "Error with dll version." << std::endl;
        return -1;
    }
    //create socket
    SOCKET m_socket;
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        std::cout << "Client: socket error." << std:: endl;
        WSACleanup();
        return -1;
    }
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientService.sin_port = htons(portno);

    //establish connection
    std::cout << "Attempting to connect to server..." << std::endl;
    while (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "Client: connect failure." << std::endl;
        Sleep(3000);
        std::cout << "Attempting Reconnect..." << std::endl;
    }
    std::cout << "Successsfully connected to server." <<std::endl;
    //send a test message
    int bytesSent;
    int bytesRecv = SOCKET_ERROR;
    char sendbuff[200] = "Client: Sending some test string.";
    char recvbuff[200] = "";

    bytesSent = send(m_socket, sendbuff, strlen(sendbuff), 0);
    std::cout << "Client: send() - bytes sent." << std::endl;
    while (bytesRecv == SOCKET_ERROR) {
        bytesRecv = recv(m_socket, recvbuff, 32, 0);
        if (bytesRecv == 0 || bytesRecv == WSAECONNRESET) {
            std::cout << "Client: Connection Closed." << std::endl;
            break;
        } else {
            std::cout << "Client: recv() is ok" << std::endl;
        }

        if (bytesRecv < 0) return 0;
        else std::cout << "Client: Bytes Received." << std::endl;
    }

    WSACleanup();
    system("pause");
    return 0;
}