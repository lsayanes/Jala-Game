//leandro_say

#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>

#include <conio.h>



bool g_bCtrlC{ false };
unsigned long g_ulWait{ 20 };

constexpr const char* szTitle{ "Watching the file!" };


void lastError()
{
    LPVOID	lpMsgBuf = NULL;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);

    std::cout << "error: " << (LPCTSTR)lpMsgBuf << std::endl;
   

    LocalFree(lpMsgBuf);
}

void pressIntro()
{
    std::cout << "Press intro" << std::endl;
    std::cin.get();
}

void help()
{
    std::cout << szTitle << std::endl <<  "use: wtf file" << std::endl;
    pressIntro();
}

void error()
{
    lastError();
    pressIntro();
}

inline void sleep(unsigned long ulMill)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ulMill));
}

BOOL WINAPI consoleHandler(DWORD signal) {

    g_bCtrlC = (CTRL_C_EVENT == signal);
    return TRUE;
}

int main(int argc, char** argv) {

    if (2 == argc)
    {

        const char* szPath = argv[1];

        if (SetConsoleCtrlHandler(consoleHandler, TRUE))
            std::cout << szTitle << " - quit with ctrl+c" << std::endl;

        SetLastError(0);

        FILE* pFile = fopen(szPath, "r");
        if (pFile)
        {
            int nIn;

            fseek(pFile, 0, SEEK_END);

            g_bCtrlC = false;

            while (!g_bCtrlC)
            {
                if (-1 != (nIn = fgetc(pFile)))
                {
                    printf("%c", char(nIn));
                }
                else
                {
                    if (_kbhit())
                        std::cin.get();
                    else
                        sleep(g_ulWait);
                }
            }

            fclose(pFile);
        }
        else
            error();

    }
    else
        help();

    return 0;
}

