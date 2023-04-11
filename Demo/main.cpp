

#if defined(_WINDOWS)
#include "framework.h"
#endif

#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>

#include <Config.h>

#include <Tools.h>

#include <Types.h>
#include <Component.h>
#include <Physics.h>
#include <Properties.h>

#include <Device.h>
#include <Entity.h>
#include <Sprite.h>
#include <FontLib.h>
#include <CharSet.h>
#include <FrameBuffer.h>
#include <EntityMngr.h>


#include "Demo.h"


#if defined(_WINDOWS)
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
    
    {

        draw::Device dev{hInstance};
#else
int main()
{

    draw::Device dev{};

#endif
 
   //set windows and screen size
    dev.width = Demo::SCREEN_W;
    dev.height = Demo::SCREEN_H;

    //create a farmebuffer from Device
    draw::FrameBuffer fb{dev};

    Demo demo{fb};

    if(demo.create())
    {
        while (dev.isRunning())
        {
            
            demo.render();
            
            //copy all from "render" back buffer to screen
            dev.flip();
        }
  
    }
  
    return 0;

}
