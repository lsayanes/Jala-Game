

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

        
#else
int main()
{

#endif
 

    Demo demo{};
    
    draw::FrameBuffer fb{ Demo::SCREEN_W, Demo::SCREEN_H, 32 };

    if(demo.create(fb))
    {

        while (demo.isRunning())
        {
            
            demo.render();
            
            //copy all from "render" back buffer to screen
            demo.flip();
        }
  
    }
  
    return 0;

}
