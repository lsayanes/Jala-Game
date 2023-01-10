//leandro_say

#include <Windows.h>
#include <iostream>


#include <conio.h>

#include "WinConsole.h"
#include "Wtf.h"



int main(int argc, char** argv) {

    if (2 == argc)
    {

        const char* szPath = argv[1];
        Wtf *pApp = new Wtf(szPath);
        if (pApp)
        {
            pApp->read();

            delete pApp;
        }

    }
    else
        Wtf::help();

    return 0;
}

