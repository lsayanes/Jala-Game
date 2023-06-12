#pragma once

class PongGame : public JalaGame
{
public:
	const std::string PLAYER_R_SIDE{ "PlayerR" };
	const std::string PLAYER_L_SIDE{ "PlayerL" };
private:

	draw::CharSet			*m_pScoreRPlayer{nullptr};
	draw::CharSet			*m_pScoreLPlayer{nullptr};

	std::mutex				m_mtxRender;
	std::mutex				m_mtxDbg;

	int m_nBallDeltaX;
	int m_nBallDeltaY;

	draw::Rect				*m_pGameArea{nullptr};

	int	m_nStatus{ PLAY_STATUS::PLAYER_R_HOLD };

	short unsigned			m_nScoreLPlayer{0};
	short unsigned			m_nScoreRPlayer{0};

	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH  { "Resources/verdana.ttf" };
	const std::string SCOREFONTPATH{ "Resources/Minecraft.ttf" };
	static constexpr size_t SCOREFONTSIZE{32};
	
	static constexpr size_t BALL_W{ 10 };
	static constexpr size_t BALL_H{ 10 };
	const std::string BALL{ "ball" };
	const std::string BALLPATH{ "Resources/ball.png" };

	static constexpr size_t PLAYERS_W{ 10 };
	static constexpr size_t PLAYERS_H{ 80 };
	
	static constexpr size_t BCKGRND_W{ 640 };
	static constexpr size_t BCKGRND_H{ 480 };
	const std::string BACKGROUND{ "background" };
	const std::string BCKGRNDTPATH{ "Resources/bck.png" };


	enum PLAY_STATUS { PLAYER_L_HOLD, PLAYER_R_HOLD, PLAYING, PAUSE };


	/*
			Error msg
	*/

	const std::string ERR_CREATING{ "Error try to create " };

private:


	void locateBall();
	void holdBall();

	void updateScores();

	inline void holdPlayerBall(std::string szPlayer)
	{
		if (szPlayer == PLAYER_L_SIDE)
			m_nStatus = PLAY_STATUS::PLAYER_L_HOLD;
		if (szPlayer == PLAYER_R_SIDE)
			m_nStatus = PLAY_STATUS::PLAYER_R_HOLD;

	}

public:
	explicit PongGame(draw::draw_t w, draw::draw_t h);
	virtual ~PongGame();

	bool create();

	void moveUp(std::string szPlayer);
	void moveDown(std::string szPlayer);

	void shot();

 	void render();
	void onClose(); 
	void onKeyDown(unsigned long  nKey);

};

