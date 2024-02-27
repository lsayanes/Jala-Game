

#include <iostream>


#if defined(_WINDOWS)
#include <Windows.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <mutex>

#endif

#include <Config.h>
#include <Types.h>
#include <Physics.h>
#include <Properties.h>
#include <Utils.h>
#include <Device.h>
#include <Entity.h>
#include <Sprite.h>
#include <FontLib.h>
#include <CharSet.h>
#include <FrameBuffer.h>
#include <EntityMngr.h>
#include <key.h>
#include <debug.h>
#include <Transform.h>
#include <Effect.h>
#include <Fire.h>

#include <JalaGame.h>


class Fire80s : public JalaGame
{
private:
	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH  { "./Resources/verdana.ttf" };


    draw::effects::Fire     fire;

private:

public:

    explicit Fire80s(draw::draw_t w, draw::draw_t h);

    virtual ~Fire80s()
    {
    }

	bool create();

 	void render();
	void onClose();
	void onKeyDown(unsigned long  nKey);

};

Fire80s::Fire80s(draw::draw_t w, draw::draw_t h) :
    JalaGame(w, h), fire{ }
{

}


bool Fire80s::create()
{
    draw::draw_t w, h;

    w = 800;
    h = 400;

    draw::Entity *pRet = EnMan.create("palette",  w, h);
    uint8_t *pData = pRet->data();
    //const uint8_t *pPalette = fire.palette();

    uint16_t i, y = 0;
    //for(int16_t p = 0; p < fire.totalPaletteElements; p+=4)
    //{
        for(y = 0; y < h; y++)
        {
            for(int16_t x = 0; x < w; x++)
            {
                i = (x + (y * w));
                /*
                pData[i + 0] = pPalette[p + 0]; //r
                pData[i + 1] = pPalette[p + 1]; //g
                pData[i + 2] = pPalette[p + 2]; //b
                pData[i + 3] = pPalette[p + 3]; //a
                */

               pData[i + 0] = 0xFF;
               pData[i + 1] = 0;
               pData[i + 2] = 0;
               pData[i + 3] = 0xFF;
            }
        }
    //}


    fire.printPalette();


    return JalaGame::create();
}

void Fire80s::render()
{
    JalaGame::render();
}

void Fire80s::onClose()
{
    m_bRunning = false;
}

void Fire80s::onKeyDown(unsigned long  ulKey)
{
	//dbg("onKeydown %lu", ulKey);

	switch(ulKey)
	{
	case VK_ESCAPE:
		onClose();
		break;

	default:
		break;
	}
}



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

    Fire80s *pFire80s = new Fire80s {w, h};

    if(pFire80s)
    {
        if(pFire80s->create())
        {
            dbg("pFire80s created w:%d, h:%d", w, h);

            pFire80s->setFps(1000);

            while (pFire80s->isRunning())
            {
                pFire80s->render();
                pFire80s->flip();
            }
        }

        delete pFire80s;
    }

    return 0;
}