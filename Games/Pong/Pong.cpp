// Pong.cpp : Define el punto de entrada de la aplicación.
//

#include <iostream>

#if defined(_WINDOWS)
#include "framework.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <mutex>

#endif


#include <Config.h>
#include <Types.h>
#include <Component.h>
#include <Physics.h>
#include <Properties.h>

#include <Tools.h>
#include <Device.h>
#include <Entity.h>
#include <Sprite.h>
#include <FontLib.h>
#include <CharSet.h>
#include <FrameBuffer.h>

#include <EntityMngr.h>

#include <JalaGame.h>
#include "PongGame.h"

#include <debug.h>

#include "Pong.h"

PongGame* pPong{nullptr};

#if defined(_WINDOWS)

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
#else


int main()
{

#endif

    draw::draw_t w{1024};
    draw::draw_t h{600};

    pPong = new PongGame {w, h};

    if(pPong)
    {
        if(pPong->create())
        {
            dbg("Pong created w:%d, h:%d", w, h);

            pPong->setFps(300);

            while (pPong->isRunning())
            {
                pPong->render();
                pPong->flip();
            }
        }

        delete pPong;
    }

    return 0;
}
