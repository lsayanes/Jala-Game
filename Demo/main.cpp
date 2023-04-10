

#if defined(_WINDOWS)
#include "framework.h"
#endif

#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>

#if defined(LINUX)
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>



#include <X11/X.h>
#include <X11/keysym.h>

extern "C"
{
    #include <tinyptc.h>
}

#endif
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

#if defined(LINUX)
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
 
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
    ch = getchar();
 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
 
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
 
    return 0;
}
#endif

#if defined(LINUX)
int main()
#endif

#if defined(_WINDOWS)
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
#endif

{

    void *pdh = nullptr;

    size_t width, height;
    unsigned char bitpx;
  
  
    draw::Device dev{ hInstance };

    //get video carateristics
    dev.getVideoMode();

    //set windows and screen size
    dev.width = Demo::SCREEN_W;
    dev.height = Demo::SCREEN_H;

    //create a farmebuffer fomr a Device
    draw::FrameBuffer fb{ dev };
    

    Demo demo{fb};

    if(demo.create())
    {
        std::cout << "Demo created" << std::endl;

 
        while (dev.isRunning())
        {
            demo.render();
            dev.flip();
        }
  
    }

    std::cout << "Demo end" << std::endl;
  
    return 0;

}
