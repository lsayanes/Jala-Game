#pragma once

class PongGame
{
public:
	const std::string PLAYER_R_SIDE{ "PlayerR" };
	const std::string PLAYER_L_SIDE{ "PlayerL" };
private:
	void					*m_phDevHandle{nullptr};
	draw::CharSet			*m_pDbgFont{nullptr};
	
	draw::EntityMngr		m_EnMan;

	bool					m_bRun;
	std::mutex				m_mtxRender;
	std::mutex				m_mtxDbg;

	int m_nBallDeltaX;
	int m_nBallDeltaY;

	draw::RECT				*m_pGameArea{nullptr};

	int	m_nStatus{ PLAY_STATUS::PLAYER_R_HOLD };

	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH{ "..\\Pong\\Resources\\verdana.ttf" };
	
	static constexpr size_t BALL_W{ 10 };
	static constexpr size_t BALL_H{ 10 };
	const std::string BALL{ "ball" };
	const std::string BALLPATH{ "..\\Pong\\Resources\\ball.png" };

	static constexpr size_t PLAYERS_W{ 10 };
	static constexpr size_t PLAYERS_H{ 80 };
	
	static constexpr size_t BCKGRND_W{ 640 };
	static constexpr size_t BCKGRND_H{ 480 };
	const std::string BACKGROUND{ "background" };
	const std::string BCKGRNDTPATH{ "..\\Pong\\Resources\\bck.png" };


	const enum PLAY_STATUS { PLAYER_L_HOLD, PLAYER_R_HOLD, PLAYING, PAUSE };


	/*
			Error msg
	*/

	const std::string ERR_CREATING{ "Error try to create " };

private:
	void render();

	void locateBall();
	void holdBall();

	inline void holdPlayerBall(std::string szPlayer)
	{
		if (szPlayer == PLAYER_L_SIDE)
			m_nStatus = PLAY_STATUS::PLAYER_L_HOLD;
		if (szPlayer == PLAYER_R_SIDE)
			m_nStatus = PLAY_STATUS::PLAYER_R_HOLD;

	}

public:
	static constexpr size_t SCREEN_W{ 1024 };
	static constexpr size_t SCREEN_H{ 800 };

	static constexpr int DGB_X{ 0 };
	static constexpr int DGB_Y{ SCREEN_H - 100 };

public:
	explicit PongGame(void *pDevHandle, draw::FrameBuffer& fbuffer);
	virtual ~PongGame();

	bool create();
	void start();
	void stop();

	void moveUp(std::string szPlayer);
	void moveDown(std::string szPlayer);

	void shot();

	void updateDbg(std::string sz);

};

