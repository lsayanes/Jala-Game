#pragma once

#include <Config.h>


class JalaGame : public draw::Device
{
private:

	draw::FrameBuffer					m_frmBuffer;
	const draw::components::Properties	&m_frmbuffProp;


	const std::string TXT_ID_DBG   {"dbg"};
	const std::string DBGFONTPATH  {"Resources/WhiteOnBlack.ttf"};
public:
	/*
			Error msg
	*/

	const std::string ERR_CREATING {"Error try to create "};


protected:
	void renderFPS();
	bool isRenderTime();

protected:
	draw::CharSet						*pDbgFont{nullptr};
	draw::EntityMngr					EnMan;
	int 								fpsCnt;


	std::chrono::steady_clock::time_point	lastTimeFps;
	std::chrono::steady_clock::time_point	lastTimeFpsCntrl;
	int64_t										frameTime;
public:

	int	Fps;

	const int DGB_X{0};
	const int DGB_Y{height-20};

	static constexpr uint8_t BCK_R{255};
	static constexpr uint8_t BCK_G{255};
	static constexpr uint8_t BCK_B{255};

public:

	explicit JalaGame(draw::draw_t w, draw::draw_t h);
	virtual ~JalaGame();

 	virtual void render();
	virtual bool create();

	bool setFps(int fps);

	draw::CharSet *createFont(std::string szPath, size_t size);

	void updateDbg(std::string sz);

	virtual void onClose() = 0;
	virtual void onKeyDown(unsigned long  nKey) = 0;

};
