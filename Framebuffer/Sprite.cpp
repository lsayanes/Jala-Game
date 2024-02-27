

#include <iostream>
#include <memory>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <algorithm>
#include <chrono>

#include "Config.h"
#include "Types.h"

#include <Physics.h>
#include <Properties.h>

#include <Device.h>
#include <Entity.h>

#include "Sprite.h"
//#include "FrameBuffer.h"

namespace draw
{
	Sprite::Sprite(draw_t w, draw_t h) :
		width{ w },
		height{ h },
		curr{ 0 },
		lastTime{ std::chrono::steady_clock::now() },
		m_dElapsed{ 10 }
	{
		m_Entities.reserve(1024);
	}

	Sprite::~Sprite()
	{
		std::for_each(m_Entities.begin(), m_Entities.end(), [&](Entity* it) { delete it; it = nullptr; });
		m_Entities.clear();
	}

	void Sprite::add(Entity* pEntity)
	{
		m_Entities.push_back(pEntity);
	}

	void Sprite::add(uint8_t* prgbaData, int32_t width, int32_t height, bool useAlpha)
	{
		draw::Entity* pNew = new draw::Entity{ static_cast<draw_t>(width),  static_cast<draw_t>(height), prgbaData, draw::components::ATC_NONE, "Sprite" };
		if (pNew)
		{
			if(useAlpha)
				pNew->properties().alpha = 1;

			m_Entities.push_back(pNew);
		}
	}

	void Sprite::pos(int x, int y)
	{

		std::for_each(m_Entities.begin(), m_Entities.end(), [&](Entity* it)
			{
				it->physics().rc.pos(x, y);
			});
	}

	Entity &Sprite::get()
	{

		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime).count() > m_dElapsed)
		{
			curr++;
			curr%=m_Entities.size();
			lastTime = std::chrono::steady_clock::now();
		}

		return *m_Entities.at(curr);
	}
/*
	const Entity& Sprite::operator[](size_t i) const
	{
		return reinterpret_cast<const Entity&>(*m_Entities.at(i));
	}
*/
}//draw
