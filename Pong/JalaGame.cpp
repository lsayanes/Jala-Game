

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

#include <debug.h>

#include "JalaGame.h"



JalaGame::JalaGame(draw::draw_t w, draw::draw_t h):
	m_frmBuffer { static_cast<uint8_t*>(Device::create(w, h)), w, h},
	m_frmbuffProp{m_frmBuffer.properties()},
	EnMan{m_frmBuffer},
	fpsCnt{0},
	lastTimeFps{std::chrono::steady_clock::now()},
	lastTimeFpsCntrl{lastTimeFps},
	frameTime{1}
{

}

JalaGame::~JalaGame()
{
	if (pDbgFont)
	{
		delete pDbgFont;
		pDbgFont = nullptr;
	}

	draw::FontLib::instance()->free();
}


bool JalaGame::create()
{
	return (pDbgFont = createFont(DBGFONTPATH, 16));
}

draw::CharSet *JalaGame::createFont(std::string szPath, size_t size)
{
	return new draw::CharSet{ draw::FontLib::instance()->newFont(szPath.c_str(), size) };
}

void JalaGame::updateDbg(std::string sz)
{		
	std::string str = JalaGame::TXT_ID_DBG + ": " + sz + " - ESC Quit";
	auto vc = pDbgFont->flatText(str.c_str(), DGB_X, DGB_Y);
	EnMan.addText(JalaGame::TXT_ID_DBG, vc);

	dbg("%s x:%d y:%d", str.c_str(), DGB_X, DGB_Y);
}

void JalaGame::renderFPS()
{
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTimeFpsCntrl).count() > 1000)
	{
		char str[1024];
		sprintf(str,"fps:%4d", fpsCnt);
		updateDbg(str);
		dbg("Total redered %llu", EnMan.size());		
		
		fpsCnt = 0;
		lastTimeFpsCntrl = std::chrono::steady_clock::now();
	}
	else
		fpsCnt++;
}

bool JalaGame::isRenderTime()
{

	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTimeFps).count() > frameTime)
	{	
		lastTimeFps = std::chrono::steady_clock::now();
		return true;
	}

	return false;
}

bool JalaGame::setFps(int fps)
{
	if(fps > 0 && fps < 1001)
	{
		frameTime = 1000 / fps;
		return true;
	}

	return false;
}


void JalaGame::render()
{

	if(isRenderTime())
	{
		m_frmBuffer.fill(JalaGame::BCK_R, JalaGame::BCK_G, JalaGame::BCK_B);
		
		renderFPS();

		EnMan.renderOrdered();
	}
}
