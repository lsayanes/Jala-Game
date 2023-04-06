
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

#include <X11/X.h>
#include <X11/keysym.h>

extern "C"
{
    #include <tinyptc.h>
}

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
 

int main()
{

    void *pdh = nullptr;

    size_t width, height;
    unsigned char bitpx;
  
    if (!draw::Device::getVideoMode(width, height, bitpx))
    {
        std::cout << "draw::Device::getVideoMode ERROR" << std::endl;
    }

    draw::FrameBuffer fb{ Demo::SCREEN_W , Demo::SCREEN_H, bitpx, pdh };
    Demo demo{pdh, fb};

    if(demo.create())
    {
        std::cout << "Demo created" << std::endl;

        demo.start();
  
        while(!fb.processEvent())
            draw::tools::sleep(500);
  
        demo.stop();
    }

    std::cout << "Demo end" << std::endl;
  
    return 0;

}
