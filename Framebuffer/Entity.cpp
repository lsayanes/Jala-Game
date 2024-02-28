
#include <iostream>
#include <vector>
#include <cstring>

#include <memory>
#include <stdint.h>
#include <fstream>
#include <algorithm>

#include "Config.h"
#include "Types.h"

#include "../Components/Physics.h"
#include "../Components/Properties.h"

#include "../Util/Utils.h"
#include "../Libs/png.h"

#include <debug.h>

#include "Entity.h"

namespace draw
{
	Entity::Entity(draw_t w, draw_t h, components::ALLOC_TYPE_COMPONENT allocType, const char *szName):
		m_Physics{ components::Physics{w, h} },
		m_Properties{ components::Properties{w, h, allocType, szName} },
		m_Data{ std::make_unique<uint8_t[]>(m_Properties.size) }
	{
		//dbg("Entity name:%s w:%d h:%d allocType:%d", szName, w, h, allocType);
	}

	Entity::Entity(draw_t w, draw_t h, uint8_t *pData, components::ALLOC_TYPE_COMPONENT allocType, const char *szName) :
		m_Physics{ components::Physics{w, h} },
		m_Properties{ components::Properties{w, h, allocType, szName} },
		m_Data{ std::unique_ptr<uint8_t[]>(pData) }
	{
		//dbg("Entity from data name:%s w:%d h:%d allocType:%d", szName, w, h, allocType);
	}


	Entity::~Entity()
	{
	}

	bool Entity::loadPng(const char* szPath)
	{
		bool bRet = false;
		Png png{};

		if (png.load(szPath, components::Properties::bpp))
		{
			unsigned char* pData = m_Data.get();
			std::memcpy(pData, png.pRawData, m_Properties.size);
			utils::bgraToRgba(pData, m_Properties.size);
			bRet = true;
		}

		dbg("loadPng %s (ret:%d)", szPath, bRet);
		return bRet;
	}

} //draw