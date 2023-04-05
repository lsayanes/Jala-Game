

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


#include "Demo.h"


Demo::Demo(void* pDevHandle, draw::FrameBuffer& fbuffer):
	m_phDevHandle{pDevHandle},
	m_EnMan{fbuffer},
	m_bRun{false}
{

}

Demo::~Demo()
{
	if (m_pDbgFont)
		delete m_pDbgFont;

	if (m_pGameArea)
		delete m_pGameArea;

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
		auto& bckphy = m_EnMan[BACKGROUND].physics();
		bckphy.rc.pos(static_cast<draw::draw_t>((SCREEN_W / 2) - (BCKGRND_W / 2)), 80);

		m_pGameArea = new draw::Rect{ bckphy.rc };	
	}
	else
	{
		printf("Demo::create %s\n", std::string{ ERR_CREATING + " entities" }.c_str());
	}

	return bRet;
}

void Demo::updateDbg(std::string sz)
{	
	std::cout << "updateDbg:" << sz << std::endl;
	
	std::string str = Demo::TXT_ID_DBG + ": " + sz;
	auto vc = m_pDbgFont->flatText(str.c_str(), DGB_X, DGB_Y);
	m_EnMan.addText(Demo::TXT_ID_DBG, vc);

	std::cout << "updateEnd"  << std::endl;

}


void Demo::render()
{

	int i = 0;
	auto lasttime = std::chrono::steady_clock::now();

	while (m_bRun)
	{

		m_EnMan.fill(255, 255, 255);
		m_EnMan.renderAll();
		m_EnMan.flip();

		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lasttime).count() > 1000)
		{
			/*
			sprintf(str, "fps :%d (min :%d max :%d) seconds :%d ", i, min, max, seconds++);
			updateDbg(str);

			if (seconds > 4)
			{
				if (min > i)
				{
					min = i;
				}
				else if (max < i)
				{
					max = i;
				}
			}
			*/

			i = 0;

			lasttime = std::chrono::steady_clock::now();
		}
		else
			i++;		
	}
}

void Demo::start()
{
	m_bRun = true;

	std::thread tRender(&Demo::render, this);
	tRender.detach();
}

void Demo::stop()
{
	m_bRun = false;
}


