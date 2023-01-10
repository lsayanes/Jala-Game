
#include <Windows.h>
#include <iostream>
#include "WinConsole.h"




class __InFunction__
{
public:
    __InFunction__() = default;

    static WinConsole* pInst;

    static BOOL WINAPI consoleHandler(DWORD signal) 
    {
        pInst->consoleEvent(signal);
        return TRUE;
    }
};

WinConsole* __InFunction__::pInst;


WinConsole::WinConsole() :
    m_pInternalFnt{new __InFunction__() }
    
{
    ((__InFunction__*)m_pInternalFnt)->pInst = this;
    if(SetConsoleCtrlHandler(((__InFunction__*)m_pInternalFnt)->consoleHandler, TRUE))
        std::cout << "quit with ctrl+c " << std::endl;
}

WinConsole::~WinConsole()
{
    if (m_pInternalFnt)
        delete m_pInternalFnt;
}

void WinConsole::printLastError() const
{
    LPVOID	lpMsgBuf = nullptr;

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


void WinConsole::consoleEvent(DWORD signal) 
{
    if (CTRL_C_EVENT == signal)
        ctrlC();
}
