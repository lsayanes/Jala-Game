

#include <iostream>

#include <Config.h>
#include <Types.h>
#include <Component.h>
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
	draw::Rect				*m_pGameArea{nullptr};


	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH  { "Resources/verdana.ttf" };

    const std::string GHOST  {"ghost"};

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

Rotate::Rotate(draw::draw_t w, draw::draw_t h) : JalaGame(w, h)
{

}


bool Rotate::create()
{
    if(EnMan.create(GHOST, 300, 300, "Resources/ghost.png") > 0)
    {
        //center image
        auto& g = EnMan[GHOST].physics();
        g.centerx(g.rc.right, 0, width);
        g.centery(g.rc.bottom, 0, height);

        //EnMan[GHOST].properties().alpha = 1;

        uint8_t bckc[4] = {255, 255, 255, 1};
        uint8_t *imgRotate = draw::Transform::rotate(EnMan[GHOST].data().get(), 300, 300, 20, bckc);

        if(imgRotate && EnMan.create("gosth_rotate", 300, 300, imgRotate) > 0)
        {
            return JalaGame::create();
        }
    }

    return false;
}

void Rotate::render()
{


    JalaGame::render();
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
	default:
		break;
	}
}



int main()
{

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