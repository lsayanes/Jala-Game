

#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <thread>
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
#include <Tools.h>





#include "Demo.h"


Demo::Demo(draw::FrameBuffer& fbuffer):
	m_EnMan{fbuffer},
	m_fps{0}
{

}

Demo::~Demo()
{
	if (m_pDbgFont)
		delete m_pDbgFont;
}


bool Demo::create()
{
	bool bRet = false;

	auto create_font = [&](std::string szPath, draw::CharSet** pFnt, size_t size)
	{
		if (nullptr == (*pFnt = new draw::CharSet{ draw::FontLib::instance()->newFont(szPath.c_str(), size), m_EnMan.bpp() }))
			printf("lambda::create_font::create %s", std::string{ ERR_CREATING + szPath }.c_str());
	};

	create_font(DBGFONTPATH, &m_pDbgFont, 16);

	size_t t = m_EnMan.create(BACKGROUND, BCKGRND_W, BCKGRND_H, BCKGRNDTPATH);
	if ((bRet = t > 0))
	{
		//initial positions

		//center the background
		auto& bckphy = m_EnMan[BACKGROUND].physics();
		bckphy.rc.pos(static_cast<draw::draw_t>((SCREEN_W / 2) - (BCKGRND_W / 2)), 80);
		/*
		auto &fbp{ m_EnMan.frameBuffer().properties() };
		bRet = Device::create(SCREEN_W, BCKGRND_W, fbp.bpp());
		*/
	}
	else
	{
		printf("Demo::create %s\n", std::string{ ERR_CREATING + " entities" }.c_str());
	}

	return bRet;
}

void Demo::updateDbg(std::string sz)
{		
	std::string str = Demo::TXT_ID_DBG + ": " + sz + " - ESC Quit";
	auto vc = m_pDbgFont->flatText(str.c_str(), DGB_X, DGB_Y);
	m_EnMan.addText(Demo::TXT_ID_DBG, vc);
}


void Demo::render()
{
	
	static auto lasttime {std::chrono::steady_clock::now()};

	m_EnMan.fill(255, 255, 255);

	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lasttime).count() > 1000)
	{
		char str[1024];
		sprintf(str,"fps:%4d", m_fps);
		updateDbg(str);
		m_fps = 0;
		lasttime = std::chrono::steady_clock::now();
	}
	else
		m_fps++;

	m_EnMan.renderAll();

}


void Demo::onKeyDown(int nKey) 
{

	if (0x1B == nKey)
		onClose();

}
void Demo::onClose()
{
	m_bRunning = 0;
}