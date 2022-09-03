
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

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

	if (m_pScoreLPlayer)
		delete m_pScoreLPlayer;
	
	if (m_pScoreRPlayer)
		delete m_pScoreRPlayer;
}


bool PongGame::create()
{
	bool bRet = false;

	auto score_font = [&](std::string szPath, draw::CharSet** pFnt, size_t size)
	{
		if (nullptr == (*pFnt = new draw::CharSet{ draw::FontLib::instance()->newFont(szPath.c_str(), size), m_EnMan.bpp() }))
			printf(std::string{ ERR_CREATING + szPath }.c_str());
	};

	score_font(DBGFONTPATH, &m_pDbgFont, 16);
	score_font(SCOREFONTPATH, &m_pScoreLPlayer, SCOREFONTSIZE);
	score_font(SCOREFONTPATH, &m_pScoreRPlayer, SCOREFONTSIZE);

	size_t t = m_EnMan.create(BACKGROUND, BCKGRND_W, BCKGRND_H, BCKGRNDTPATH);
	t > 0 && (t = m_EnMan.create(BALL, BALL_W, BALL_H, BALLPATH));
	t > 0 && (t = m_EnMan.create(PLAYER_L_SIDE, PLAYERS_W, PLAYERS_H));
	t > 0 && (t = m_EnMan.create(PLAYER_R_SIDE, PLAYERS_W, PLAYERS_H));
	if (bRet = t > 0)
	{
		//initial positions
		auto& bckphy = m_EnMan[BACKGROUND].physics();
		bckphy.rc.pos(static_cast<draw::draw_t>((SCREEN_W / 2) - (BCKGRND_W / 2)), 80);

		auto& bckprp = m_EnMan[BACKGROUND].properties();
		m_pGameArea = new draw::Rect{ bckphy.rc };
	
		auto &ball = m_EnMan[BALL];
		auto& ballphy = ball.physics();
		ball.renderWithAlpha();

		auto init_player = [&](std::string szPlayer) 
		{
			auto &phy = m_EnMan[szPlayer].physics();
			phy.centery(PLAYERS_H, bckphy.rc.top, bckphy.rc.bottom);
			m_EnMan.frameBuffer().fill(m_EnMan[szPlayer], 0, 255, 0);

			//velocity
			phy.vy = 10;
			phy.vx = 0;
		};

		init_player(PLAYER_L_SIDE);
		init_player(PLAYER_R_SIDE);

		m_EnMan[PLAYER_L_SIDE].physics().rc.posx(bckphy.rc.left + BALL_W);
		m_EnMan[PLAYER_R_SIDE].physics().rc.posx(((bckphy.rc.right) - PLAYERS_W) - BALL_W);
	}
	else
	{
		printf(std::string{ ERR_CREATING + " some entities could not be created" }.c_str());
	}

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

void  PongGame::updateScores()
{

	auto add_score = [&](unsigned short player, draw::CharSet** pFnt, std::string name, int x)
	{
		std::string str{ std::to_string(player) };
		auto vc = (*pFnt)->flatText(str.c_str(), x, m_pGameArea->top + 250);
		m_EnMan.addText(name, vc);
	};
	
	add_score(m_nScoreLPlayer, &m_pScoreLPlayer,  "score L", m_pGameArea->left - 40);
	add_score(m_nScoreRPlayer, &m_pScoreRPlayer, "score R", m_pGameArea->right + 20);

}


void PongGame::render()
{

	int i = 0;
	//char str[1024];
	updateScores();

	auto lasttime = std::chrono::steady_clock::now();

	//int min = 10000, max = 0, seconds = 0;

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
			/*
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
			*/

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
	if (plphy.rc.top > m_pGameArea->top)
		plphy.rc.moveUp(plphy.vy);
	
}

void PongGame::moveDown(std::string szPlayer)
{
	auto& plphy = m_EnMan[szPlayer].physics();
	if (plphy.rc.bottom < m_pGameArea->bottom)
		plphy.rc.moveDown(plphy.vy);
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
		ballphy.rc.posx(ply.rc.right);
		ballphy.centery(BALL_H, ply.rc.top, ply.rc.bottom);

	}
	else if (PLAY_STATUS::PLAYER_R_HOLD == m_nStatus)
	{
		auto& ply = m_EnMan[PLAYER_R_SIDE].physics();
		//ballphy.x = ply.x - PLAYERS_W;
		ballphy.rc.posx(ply.rc.left - PLAYERS_W);
		ballphy.centery(BALL_H, ply.rc.top, ply.rc.bottom);
	}

}

void PongGame::locateBall()
{
	auto& ballphy = m_EnMan[BALL].physics();
	auto& plylphy = m_EnMan[PLAYER_L_SIDE].physics();
	auto& plyrphy = m_EnMan[PLAYER_R_SIDE].physics();

	ballphy.rc.pos(
						ballphy.rc.left + 1 * m_nBallDeltaX,
						ballphy.rc.top + 1 * m_nBallDeltaY
		);

	if (
		ballphy.rc.right <= plylphy.rc.right &&
		ballphy.rc.top >= plylphy.rc.top && ballphy.rc.top <= plylphy.rc.bottom
		)
	{
		m_nBallDeltaX = 1;
	}
	else if (
		ballphy.rc.right > plyrphy.rc.left &&
		ballphy.rc.top >= plyrphy.rc.top && ballphy.rc.top <= plyrphy.rc.bottom
		)
	{
		m_nBallDeltaX = -1;
	}
	else if (ballphy.rc.left < m_pGameArea->left)
	{
		m_nScoreRPlayer++;
		updateScores();
		holdPlayerBall(PLAYER_L_SIDE);
	}
	else if (ballphy.rc.right > m_pGameArea->right)
	{
		m_nScoreLPlayer++;
		updateScores();
		holdPlayerBall(PLAYER_R_SIDE);
	}
	else if (ballphy.rc.top < m_pGameArea->top)
	{
		m_nBallDeltaY = 1;
	}
	else if (ballphy.rc.bottom > m_pGameArea->bottom)
	{
		m_nBallDeltaY = -1;
	}

}