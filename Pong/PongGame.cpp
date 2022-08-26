
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>

#include <Config.h>

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


#include "PongGame.h"

#include <iostream>
PongGame::PongGame(void* pDevHandle, draw::FrameBuffer& fbuffer):
	m_phDevHandle{pDevHandle},
	m_EnMan{fbuffer},
	m_bRun{false},
	m_mtxRender{},
	m_mtxDbg{},
	m_nBallDeltaY{-1},
	m_nBallDeltaX{1}
{

	auto x = 61;
	auto y = 43;
	auto a = x + y;
	std::cout << ++y + x + x;

}

PongGame::~PongGame()
{
	if (m_pDbgFont)
		delete m_pDbgFont;

	if (m_pGameArea)
		delete m_pGameArea;
}


bool PongGame::create()
{
	bool bRet = false;

	if (nullptr != (m_pDbgFont = new draw::CharSet{ draw::FontLib::instance()->newFont(DBGFONTPATH.c_str(), 16), m_EnMan.bpp() }))
	{

		//size_t t = m_EnMan.create(BALL, BALL_W, BALL_H, BALLPATH);
		size_t t = m_EnMan.create(BACKGROUND, BCKGRND_W, BCKGRND_H, BCKGRNDTPATH);
		t > 0 && (t = m_EnMan.create(BALL, BALL_W, BALL_H, BALLPATH));
		if (bRet = t > 0)
		{
			//initial positions
			auto& bckphy = m_EnMan(BACKGROUND).physics();
			bckphy.x = static_cast<draw::phy_type>((SCREEN_W / 2) - (BCKGRND_W / 2));
			bckphy.y = 80;

			auto& bckprp = m_EnMan(BACKGROUND).properties();
			m_pGameArea = new draw::RECT{ bckphy.x, bckphy.y, bckprp.w, bckprp.h };
	
			auto& ballphy = m_EnMan(BALL).physics();
			auto& ballprp = m_EnMan(BALL).properties();
			ballphy.x = bckphy.x;
			ballphy.y = static_cast<draw::phy_type>( bckphy.y + (BCKGRND_H - ballprp.h) );

			ballprp.alpha = 1;
		
			//m_EnMan.frameBuffer().fill(m_EnMan(BALL), 0, 255, 0);
		}
	}
	else
		printf(std::string{ERR_CREATING + DBGFONTPATH }.c_str());

	return bRet;
}

void PongGame::updateDbg(std::string sz)
{
	m_mtxDbg.lock();
	
	std::string str = PongGame::TXT_ID_DBG + ": " + sz;
	auto vc = m_pDbgFont->flatText(str.c_str(), DGB_X, DGB_Y);
	m_EnMan.addText(PongGame::TXT_ID_DBG, vc);
	
	m_mtxDbg.unlock();
}

void PongGame::render()
{

	int i = 0;
	char str[1024];

	auto lasttime = std::chrono::steady_clock::now();

	int min = 10000, max = 0, seconds = 0;

	while (m_bRun)
	{
		m_mtxRender.lock();

		m_EnMan.fill(255, 255, 255);

		locateBall();

		m_EnMan.renderAll();

		m_EnMan.flip();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lasttime).count() > 1000)
		{
			sprintf(str, "fps :%d (min :%d max :%d) seconds :%d ", i, min, max, seconds++);
			updateDbg(str);

			if (seconds > 4)
			{
				if (min > i)
				{
					min = i;
				}

				if (max < i)
				{
					max = i;
				}
			}


			i = 0;

			lasttime = std::chrono::steady_clock::now();
		}
		else
			i++;

		
		m_mtxRender.unlock();

	}
}

void PongGame::start()
{
	m_bRun = true;

	std::thread tRender(&PongGame::render, this);
	tRender.detach();
}

void PongGame::stop()
{
	m_bRun = false;
}


void PongGame::locateBall()
{
	auto& ballphy = m_EnMan(BALL).physics();
	auto& ballprp = m_EnMan(BALL).properties();

	ballphy.x += 1 * m_nBallDeltaX;
	ballphy.y += 1 * m_nBallDeltaY;

	if (ballphy.x < m_pGameArea->left)
	{
		ballphy.x = m_pGameArea->left;
		m_nBallDeltaX = 1;
	}
	else if (ballphy.x + ballprp.w > m_pGameArea->right)
	{
		ballphy.x = static_cast<draw::phy_type>(m_pGameArea->right - ballprp.w);
		m_nBallDeltaX = -1;
	}

	if (ballphy.y < m_pGameArea->top)
	{
		ballphy.y = m_pGameArea->top;
		m_nBallDeltaY = 1;
	}
	else if (ballphy.y + ballprp.h > m_pGameArea->bottom)
	{
		ballphy.y = static_cast<draw::phy_type>(m_pGameArea->bottom - ballprp.h);
		m_nBallDeltaY = -1;
	}
}