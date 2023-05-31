

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
 
/*
    Device crea el backbuffer ya que es el buffer del device en cuestion y este debe ser pasado a framebuffer
    quien dibuja en el

    Antiguo contructor FrameBuffer;

   	FrameBuffer::FrameBuffer(draw_t w, draw_t h, uint8_t bits, void* pDevHandle) :
        Device{ pDevHandle },
        m_Properties{ components::Properties{w, h, bits, 0} },
        m_Width{w},
        m_Height{h}
    {

        unsigned char comp = m_Properties.components();

        m_Line = std::make_unique<unsigned char[]>(m_Properties.lineSize);
        m_pbyBuffer = static_cast<unsigned char*>(Device::create(w, h, bits));

    }

    ahora  FrameBuffer no hereda de Device (no se justifica)
    y la plantilla para crear juegos Demo sí hereda de Device para
    poder reescribir todas las funciones que maneja el device en cuestion 
    como onKey, onClose, etc. 
    
    Demo necesita la referencia a un FrameBuffer. 
    El backbuffer devuelto por device y el puntero (m_pbyBuffer) a este backbuffer
    en Device quedan desconectados

*/



    Demo demo { Demo::SCREEN_W, Demo::SCREEN_H, 32 };

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
