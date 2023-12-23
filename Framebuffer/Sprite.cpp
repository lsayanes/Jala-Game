

#include <memory>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <algorithm>


#include "Config.h"
#include "Types.h"

#include <Component.h>
#include <Physics.h>
#include <Properties.h>

#include <Device.h>
#include <Entity.h>

#include "Sprite.h"
//#include "FrameBuffer.h"

namespace draw
{
	Sprite::Sprite(draw_t w, draw_t h):
		width{w},
		height{h},
		curr{0}
	{
		m_Entities.reserve(1024);
	}

	Sprite::~Sprite()
	{
		std::for_each(m_Entities.begin(), m_Entities.end(), [&](Entity *it) { delete it; it = nullptr; });
		m_Entities.clear();
	}

	void Sprite::add(Entity *pEntity)
	{
		m_Entities.push_back(pEntity);
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
		//is it time to increment ?
		curr++;
		curr%=m_Entities.size();

		return *m_Entities.at(curr);
	}
/*
	const Entity& Sprite::operator[](size_t i) const
	{
		return reinterpret_cast<const Entity&>(*m_Entities.at(i));
	}
*/
}//draw
