#pragma once

class Demo
{
private:
	void					*m_phDevHandle{nullptr};
	draw::CharSet			*m_pDbgFont{nullptr};
	draw::EntityMngr		m_EnMan;

	bool					m_bRun;

	draw::Rect				*m_pGameArea{nullptr};


	const std::string TXT_ID_DBG{ "dbg" };
	const std::string DBGFONTPATH  { "./White On Black.ttf" };
	
	static constexpr size_t BCKGRND_W{ 1024 };
	static constexpr size_t BCKGRND_H{ 600 };
	const std::string BACKGROUND{ "background" };
	const std::string BCKGRNDTPATH{ "./bgd_test.png" };


	/*
			Error msg
	*/

	const std::string ERR_CREATING{ "Error try to create " };

private:
	void render();


public:
	static constexpr size_t SCREEN_W{ 1024 };
	static constexpr size_t SCREEN_H{ 800 };

	static constexpr int DGB_X{ 0 };
	static constexpr int DGB_Y{ SCREEN_H - 100 };

public:
	explicit Demo(void *pDevHandle, draw::FrameBuffer& fbuffer);
	virtual ~Demo();

	bool create();
	void start();
	void stop();

	void updateDbg(std::string sz);

};

