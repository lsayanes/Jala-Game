

#include <iostream>
#include <iomanip>

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


    draw::effects::Fire     *pFire;

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
    JalaGame(w, h)
{

}


bool Fire80s::create()
{

    draw::Entity *pRet = EnMan.create("palette",  width, height);
    pFire = new draw::effects::Fire(*pRet);
    //pFire->drawPalette();

    return JalaGame::create();
}

void Fire80s::render()
{
    pFire->doFrame();
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