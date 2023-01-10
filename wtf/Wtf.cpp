


#include <string.h>
#include <conio.h>

#include "WinConsole.h"

#include "Wtf.h"

Wtf::Wtf(const char* szPathFile) :
    m_bCtrlC{ false },
    m_szPathFile{ szPathFile }

{
    std::cout << m_szTitle << std::endl;
};


Wtf::~Wtf()
    
{
}

void Wtf::error() const
{
    printLastError();
    pressIntro();
}


void Wtf::ctrlC()
{
    m_bCtrlC = true;
}


void Wtf::read()
{
    SetLastError(0);

    FILE* pFile = fopen(m_szPathFile, "r");
    if (pFile)
    {
        int nIn;

        fseek(pFile, 0, SEEK_END);

        while (!m_bCtrlC)
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
                    sleep(10);
            }
        }

        fclose(pFile);
    }
    else
        error();

}