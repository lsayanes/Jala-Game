
#include <iostream>
#include <vector>
#include <cstring>

//#include <memory>
#include <malloc.h>
#include <stdint.h>
#include <fstream>
#include <algorithm>

#include "Config.h"
#include "Types.h"

#include <Physics.h>
#include <Properties.h>

#include <Utils.h>
#include <png.h>

#include <debug.h>

#include "Entity.h"

namespace draw
{
	Entity::Entity(draw_t w, draw_t h,  const char *szName):
		m_Physics{ components::Physics{w, h} },
		m_Properties{ components::Properties{w, h, components::ATC_MALLOC_BUFFER, szName} },
		//m_Data{ std::make_unique<uint8_t[]>(m_Properties.size) }
		m_Data { reinterpret_cast<uint8_t*>(malloc(m_Properties.size)) }
	{
		dbg("Entity name:%s w:%d h:%d allocType:%d", szName, w, h, m_Properties.allocType());
	}

	Entity::Entity(draw_t w, draw_t h, uint8_t *pData, const char *szName) :
		m_Physics{ components::Physics{w, h} },
		m_Properties{ components::Properties{w, h, components::ATC_EXTERNAL_MALLOC_BUFFER, szName} },
		m_Data{ pData }
	{
		dbg("Entity from data name:%s w:%d h:%d allocType:%d", szName, w, h, m_Properties.allocType());
	}


	Entity::~Entity()
	{
		if (components::ATC_MALLOC_BUFFER == m_Properties.allocType())
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	bool Entity::loadPng(const char* szPath)
	{
		bool bRet = false;
		Png png{};

		if (png.load(szPath, components::Properties::bpp))
		{
			//unsigned char* pData = m_Data.get();
			std::memcpy(m_Data, png.pRawData, m_Properties.size);
			utils::bgraToRgba(m_Data, m_Properties.size);
			bRet = true;
		}

		dbg("loadPng %s (ret:%d)", szPath, bRet);
		return bRet;
	}

} //draw