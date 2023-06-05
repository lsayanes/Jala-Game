

#if defined(_WINDOWS)
#include "framework.h"
#endif

#include <errno.h>

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

    draw::draw_t w{1024}, h{800}, b;

    if(!Demo::getVideoMode(w, h, b))
    {
        std::cout << "ERROR: Get video mode :" << strerror(errno) << std::endl; 
    }

 
    Demo demo {w, h};

    if(demo.create())
    {
        std::cout << "Demo created" << std::endl;
        while (demo.isRunning())
        {
            demo.render();
            
            //copy all from "render" back buffer to screen
            demo.flip();
        }
  
    }
  
    return 0;

}
