
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

		size_t t = m_EnMan.create(BACKGROUND, BCKGRND_W, BCKGRND_H, BCKGRNDTPATH);
		t > 0 && (t = m_EnMan.create(BALL, BALL_W, BALL_H, BALLPATH));
		t > 0 && (t = m_EnMan.create(PLAYER_L_SIDE, PLAYERS_W, PLAYERS_H));
		t > 0 && (t = m_EnMan.create(PLAYER_R_SIDE, PLAYERS_W, PLAYERS_H));
		if (bRet = t > 0)
		{
			//initial positions
			auto& bckphy = m_EnMan[BACKGROUND].physics();
			bckphy.x = static_cast<draw::phy_type>((SCREEN_W / 2) - (BCKGRND_W / 2));
			bckphy.y = 80;

			auto& bckprp = m_EnMan[BACKGROUND].properties();
			m_pGameArea = new draw::RECT{ bckphy.x, bckphy.y, bckprp.w, bckprp.h };
	
			auto &ball = m_EnMan[BALL];
			auto& ballphy = ball.physics();
			ball.renderWithAlpha();

			auto init_player = [&](std::string szPlayer) 
			{
				auto &phy = m_EnMan[szPlayer].physics();
				phy.centery(PLAYERS_H, bckphy.y, bckprp.h + bckphy.y);
				m_EnMan.frameBuffer().fill(m_EnMan[szPlayer], 0, 255, 0);

				//velocity
				phy.vy = 10;
				phy.vx = 0;
			};

			init_player(PLAYER_L_SIDE);
			init_player(PLAYER_R_SIDE);

			auto &plyLPhy = m_EnMan[PLAYER_L_SIDE].physics();
			auto& plyRPhy = m_EnMan[PLAYER_R_SIDE].physics();

			plyLPhy.x = static_cast<draw::phy_type>(bckphy.x + BALL_W);
			plyRPhy.x = static_cast<draw::phy_type>(((bckphy.x + bckprp.w) - PLAYERS_W) - BALL_W);
			
			//ballphy.x = plyLPhy.x + PLAYERS_W;
			//ballphy.centery(BALL_H, plyLPhy.y, plyLPhy.y + PLAYERS_H);
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
		
		switch (m_nStatus)
		{
		case PLAY_STATUS::PLAYER_L_HOLD:
		case PLAY_STATUS::PLAYER_R_HOLD:
			holdBall();
			break;
		case PLAY_STATUS::PLAYING:
			if (i && 0 == i % 3)
				locateBall();
			break;
		case PLAY_STATUS::PAUSE:
			break;
		default:
			break;
		}
		
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
				else if (max < i)
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


void PongGame::moveUp(std::string szPlayer)
{
	auto& plphy = m_EnMan[szPlayer].physics();
	if (plphy.y > m_pGameArea->top)
		plphy.y-=plphy.vy;
	
}

void PongGame::moveDown(std::string szPlayer)
{
	auto& plphy = m_EnMan[szPlayer].physics();
	if ((plphy.y + PLAYERS_H) < m_pGameArea->bottom)
		plphy.y+=plphy.vy;
}

void PongGame::shot()
{
	if (
		PLAY_STATUS::PLAYER_L_HOLD == m_nStatus || 
		PLAY_STATUS::PLAYER_R_HOLD == m_nStatus
		)
	{
		m_nStatus = PLAY_STATUS::PLAYING;
	}
}

void PongGame::holdBall()
{
	auto& ballphy = m_EnMan[BALL].physics();
	if (PLAY_STATUS::PLAYER_L_HOLD == m_nStatus)
	{
		auto& ply = m_EnMan[PLAYER_L_SIDE].physics();
		ballphy.x = ply.x + PLAYERS_W;
		ballphy.centery(BALL_H, ply.y, ply.y + PLAYERS_H);

	}
	else if (PLAY_STATUS::PLAYER_R_HOLD == m_nStatus)
	{
		auto& ply = m_EnMan[PLAYER_R_SIDE].physics();
		ballphy.x = ply.x - PLAYERS_W;
		ballphy.centery(BALL_H, ply.y, ply.y + PLAYERS_H);
	}

}

void PongGame::locateBall()
{
	auto& ballphy = m_EnMan[BALL].physics();
	auto& plylphy = m_EnMan[PLAYER_L_SIDE].physics();
	auto& plyrphy = m_EnMan[PLAYER_R_SIDE].physics();

	ballphy.x += 1 * m_nBallDeltaX;
	ballphy.y += 1 * m_nBallDeltaY;

	if (
		static_cast<draw::phy_type>(ballphy.x + BALL_W) <= static_cast<draw::phy_type>(plylphy.x + PLAYERS_W) &&
		ballphy.y >= plylphy.y && ballphy.y <= static_cast<draw::phy_type>(plylphy.y + PLAYERS_H)
		)
	{
		ballphy.x = static_cast<draw::phy_type>(plylphy.x + PLAYERS_W);
		m_nBallDeltaX = 1;
	}
	else if (static_cast<draw::phy_type>(ballphy.x + BALL_W) > plyrphy.x &&
		ballphy.y >= plyrphy.y && ballphy.y <= static_cast<draw::phy_type>(plyrphy.y + PLAYERS_H)
		)
	{
		ballphy.x = static_cast<draw::phy_type>(plyrphy.x - BALL_W);
		m_nBallDeltaX = -1;
	}
	else if (ballphy.x < m_pGameArea->left)
	{
		holdPlayerBall(PLAYER_L_SIDE);
	}
	else if (ballphy.x + BALL_W > m_pGameArea->right)
	{
		holdPlayerBall(PLAYER_R_SIDE);
	}
	else if (ballphy.y < m_pGameArea->top)
	{
		ballphy.y = m_pGameArea->top;
		m_nBallDeltaY = 1;
	}
	else if (ballphy.y + BALL_H > m_pGameArea->bottom)
	{
		ballphy.y = static_cast<draw::phy_type>(m_pGameArea->bottom - BALL_H);
		m_nBallDeltaY = -1;
	}
}