#pragma once

class Demo
{
private:

	draw::CharSet			*m_pDbgFont{nullptr};
	draw::EntityMngr		m_EnMan;

	bool					m_bRun;
	bool					m_bRendering;

	draw::Rect				*m_pGameArea{nullptr};

	int						m_fps;


	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH  { "./WhiteOnBlack.ttf" };
	
	static constexpr size_t BCKGRND_W{ 1024 };
	static constexpr size_t BCKGRND_H{ 600 };
	const std::string BACKGROUND{ "background" };
	const std::string BCKGRNDTPATH{ "./bgd_test.png" };


	/*
			Error msg
	*/

	const std::string ERR_CREATING{ "Error try to create " };

//private:
//	void render();


public:
	static constexpr size_t SCREEN_W{ 1024 };
	static constexpr size_t SCREEN_H{ 800 };

	static constexpr int DGB_X{ SCREEN_W / 2};
	static constexpr int DGB_Y{ SCREEN_H / 2};

public:
	explicit Demo(draw::FrameBuffer& fbuffer);
	virtual ~Demo();

	void render();

	bool create();
	void start();
	void stop();

	[[nodiscard]]inline const bool isRunning() const { return m_bRun; } 

	void updateDbg(std::string sz);

};

