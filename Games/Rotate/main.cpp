

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

#include <JalaGame.h>


class Rotate : public JalaGame
{
private:
    draw::types::Rect* m_pGameArea{ nullptr };


    static constexpr  const char * const TXT_ID_DBG{ "dbg" };
    static constexpr  const char* const  DBGFONTPATH  { "./Resources/verdana.ttf" };

    static constexpr  const char* const  GHOST  {"ghost"};

    draw::Sprite            *m_pGhost;

    int         m_gx;
    int         m_gy;

private:

public:

    explicit Rotate(draw::draw_t w, draw::draw_t h);

    virtual ~Rotate()
    {
    }

	bool create();

 	void render();
	void onClose();
	void onKeyDown(unsigned long  nKey);

};

Rotate::Rotate(draw::draw_t w, draw::draw_t h) :
    JalaGame(w, h),
    m_pGhost{ new draw::Sprite{300, 300}},
    m_gx{600},
    m_gy{200}
{

}


bool Rotate::create()
{


    if(
        EnMan.create("bck", 1024, 600, "Resources/bck.png") &&
        EnMan.create(GHOST, 300, 300, "Resources/ghost_transparent.png")
        )
    {
        //center image
        auto& g = EnMan[GHOST].physics();
        g.centerx(g.rc.right, 0, width);
        g.centery(g.rc.bottom, 0, height);

        auto gw = m_pGhost->w();
        auto gh = m_pGhost->h();

        for(float f = 0; f < 360.0; f+=1.0)
        {
            m_pGhost->add(
                            draw::Transform::rotate(EnMan[GHOST].data(), gw, gh, f),
                            gw, gh, true
                        );
        }


        dbg("pGhost total frames %u", m_pGhost->total());

        if(m_pGhost->total() && EnMan.create("gosth1", m_pGhost) > 0)
        {
            m_pGhost->pos(m_gx, m_gy);
            if(!EnMan.remove(GHOST))
            {
                dbg("Removing %s error", GHOST);
            }

            return JalaGame::create();
        }
    }

    return false;
}

void Rotate::render()
{
    if (isRenderTime())
    {
        renderFPS();
        EnMan.renderOrdered();
    }

    //JalaGame::render();
}

void Rotate::onClose()
{
    m_bRunning = false;
}

void Rotate::onKeyDown(unsigned long  ulKey)
{
	//dbg("onKeydown %lu", ulKey);

	switch(ulKey)
	{
	case VK_ESCAPE:
		onClose();
		break;

    case VK_LEFT:
        m_gx-=5;
        m_pGhost->pos(m_gx, m_gy);
        break;
    case VK_UP:
        m_gy -= 5;
        m_pGhost->pos(m_gx, m_gy);

        break;
    case VK_RIGHT:
        m_gx+=5;
        m_pGhost->pos(m_gx, m_gy);
        break;
    case VK_DOWN:
        m_gy += 5;
        m_pGhost->pos(m_gx, m_gy);

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

    Rotate *pRotate = new Rotate {w, h};

    if(pRotate)
    {
        if(pRotate->create())
        {
            dbg("pRotate created w:%d, h:%d", w, h);

            pRotate->setFps(1000);

            while (pRotate->isRunning())
            {
                pRotate->render();
                pRotate->flip();
            }
        }

        delete pRotate;
    }

    return 0;
}