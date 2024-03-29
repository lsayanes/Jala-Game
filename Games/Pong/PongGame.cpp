
#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <iostream>
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
#include <Physics.h>
#include <Properties.h>

#include <Device.h>
#include <Entity.h>
#include <Sprite.h>
#include <FontLib.h>
#include <CharSet.h>
#include <FrameBuffer.h>

#include <EntityMngr.h>

#include <key.h>

#include <JalaGame.h>

#include <debug.h>
#include "PongGame.h"


PongGame::PongGame(draw::draw_t w, draw::draw_t h) :
	JalaGame(w, h),
	m_mtxRender{},
	m_mtxDbg{},
	m_nBallDeltaX{1},
	m_nBallDeltaY{-1},
	m_nBallUpdate{0}
{

}

PongGame::~PongGame()
{
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


	m_pScoreLPlayer = createFont(SCOREFONTPATH, SCOREFONTSIZE);
	m_pScoreRPlayer = createFont(SCOREFONTPATH, SCOREFONTSIZE);

	auto t = EnMan.create(BACKGROUND, BCKGRND_W, BCKGRND_H, BCKGRNDTPATH);
	t && (t = EnMan.create(BALL, BALL_W, BALL_H, BALLPATH));
	t && (t = EnMan.create(PLAYER_L_SIDE, PLAYERS_W, PLAYERS_H));
	t && (t = EnMan.create(PLAYER_R_SIDE, PLAYERS_W, PLAYERS_H));
	if ((bRet = nullptr != t))
	{
		//initial positions
		auto& bckphy = EnMan[BACKGROUND].physics();
		bckphy.rc.pos(static_cast<draw::draw_t>((width / 2) - (BCKGRND_W / 2)), 80);

		//auto& bckprp = EnMan[BACKGROUND].properties();
		m_pGameArea = new draw::types::Rect{ bckphy.rc };

		auto &ball = EnMan[BALL];
		//auto& ballphy = ball.physics();
		ball.renderWithAlpha();

		auto init_player = [&](std::string szPlayer)
		{
			auto &phy = EnMan[szPlayer].physics();
			phy.centery(PLAYERS_H, bckphy.rc.top, bckphy.rc.bottom);
			draw::FrameBuffer::fill(EnMan[szPlayer], 0, 255, 0);

			//velocity
			phy.vy = 12;
			phy.vx = 0;
		};

		init_player(PLAYER_L_SIDE);
		init_player(PLAYER_R_SIDE);

		EnMan[PLAYER_L_SIDE].physics().rc.posx(bckphy.rc.left + BALL_W);
		EnMan[PLAYER_R_SIDE].physics().rc.posx(((bckphy.rc.right) - PLAYERS_W) - BALL_W);


	}
	else
	{
		dbg("%s", std::string{ JalaGame::ERR_CREATING + " some entities could not be created" }.c_str());
	}

	(true == bRet) && (bRet = JalaGame::create());

	return bRet;
}

void PongGame::updateScores()
{

	auto add_score = [&](unsigned short player, draw::CharSet** pFnt, std::string name, int x)
	{
		std::string str{ std::to_string(player) };
		auto vc = (*pFnt)->flatText(str.c_str(), x, m_pGameArea->top + 250);
		EnMan.addText(name, vc);
	};

	add_score(m_nScoreLPlayer, &m_pScoreLPlayer, "score L", m_pGameArea->left - 40);
	add_score(m_nScoreRPlayer, &m_pScoreRPlayer, "score R", m_pGameArea->right + 20);
}


void PongGame::render()
{

	updateScores();

	switch (m_nStatus)
	{
	case PLAY_STATUS::PLAYER_L_HOLD:
	case PLAY_STATUS::PLAYER_R_HOLD:
		holdBall();
		break;
	case PLAY_STATUS::PLAYING:
		locateBall();
		break;
	case PLAY_STATUS::PAUSE:
		break;
	default:
		break;
	}

	JalaGame::render();
}


void PongGame::moveUp(std::string szPlayer)
{
	auto& plphy = EnMan[szPlayer].physics();
	if (plphy.rc.top > m_pGameArea->top)
		plphy.rc.moveUp(plphy.vy);

}

void PongGame::moveDown(std::string szPlayer)
{
	auto& plphy = EnMan[szPlayer].physics();
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
	auto& ballphy = EnMan[BALL].physics();
	if (PLAY_STATUS::PLAYER_L_HOLD == m_nStatus)
	{
		auto& ply = EnMan[PLAYER_L_SIDE].physics();
		ballphy.rc.posx(ply.rc.right);
		ballphy.centery(BALL_H, ply.rc.top, ply.rc.bottom);

	}
	else if (PLAY_STATUS::PLAYER_R_HOLD == m_nStatus)
	{
		auto& ply = EnMan[PLAYER_R_SIDE].physics();
		//ballphy.x = ply.x - PLAYERS_W;
		ballphy.rc.posx(ply.rc.left - PLAYERS_W);
		ballphy.centery(BALL_H, ply.rc.top, ply.rc.bottom);
	}

}

void PongGame::locateBall()
{
	if (m_nBallUpdate && 0 == (m_nBallUpdate % 3))
	{

		auto& ballphy = EnMan[BALL].physics();
		auto& plylphy = EnMan[PLAYER_L_SIDE].physics();
		auto& plyrphy = EnMan[PLAYER_R_SIDE].physics();

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

	m_nBallUpdate++;
}


void PongGame::onKeyDown(unsigned long ulKey)
{

	dbg("onKeydown %lu", ulKey);

	switch(ulKey)
	{
	case VK_a:
		moveDown(PLAYER_L_SIDE);
		break;
	case VK_q:
		moveUp(PLAYER_L_SIDE);
		break;
	case VK_DOWN:
		moveDown(PLAYER_R_SIDE);
		break;
	case VK_UP:
		moveUp(PLAYER_R_SIDE);
		break;
	case VK_ESCAPE:
		onClose();
		break;
	case VK_SPACE:
		shot();
		break;
	default:
		break;
	}

}
void PongGame::onClose()
{
	//TODO: LINUX ahora sale por isRunning
	m_bRunning = false;
	dbg("OnClose %d\n", 1);
}