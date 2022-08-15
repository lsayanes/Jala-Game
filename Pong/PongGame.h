#pragma once

class PongGame
{
private:
	void					*m_phDevHandle{nullptr};
	draw::CharSet			*m_pDbgFont{nullptr};
	
	draw::EntityMngr		m_EnMan;

	bool					m_bRun;
	std::mutex				m_mtxRender;


	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH{ "..\\Pong\\Resources\\verdana.ttf" };
	
	const std::string BALL{"ball"};
	static constexpr size_t BALL_W{ 10 };
	static constexpr size_t BALL_H{ 10 };

	
	static constexpr size_t BCKGRND_W{ 640 };
	static constexpr size_t BCKGRND_H{ 480 };
	const std::string BACKGROUND{ "background" };
	const std::string BCKGRNDTPATH{ "..\\Pong\\Resources\\bck.png" };
	



	/*
			Error msg
	*/

	const std::string ERR_CREATING{ "Error try to create " };

private:
	void render();
	void run();

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


	void dbg(std::string sz);

};

