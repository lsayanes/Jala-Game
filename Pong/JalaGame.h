#pragma once

#include <Config.h>

class JalaGame : public draw::Device
{
private:

	draw::FrameBuffer					m_frmBuffer;
	const draw::components::Properties	&m_frmbuffProp;


	const std::string TXT_ID_DBG   {"dbg"};
	const std::string DBGFONTPATH  {"Resources/WhiteOnBlack.ttf"};

	/*
			Error msg
	*/

	const std::string ERR_CREATING {"Error try to create "};
private:
	inline void updateFPS();

protected:
	draw::CharSet						*m_pDbgFont{nullptr};
	draw::EntityMngr					EnMan;
	int 								fps;
public:

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

	draw::CharSet *createFont(std::string szPath, size_t size);

	void updateDbg(std::string sz);

	virtual void onClose() = 0; 
	virtual void onKeyDown(unsigned long  nKey) = 0;

};

