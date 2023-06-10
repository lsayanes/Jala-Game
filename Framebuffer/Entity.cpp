
#include <iostream>
#include <vector>
#include <cstring>

#include <memory>
#include <stdint.h>
#include <fstream>
#include <algorithm>

#include "Config.h"
#include "Types.h"

#include "../Components/Component.h"
#include "../Components/Physics.h"
#include "../Components/Properties.h"

#include "../Util/Tools.h"
#include "../Libs/png.h"

#include <debug.h>

#include "Entity.h"

namespace draw
{
	Entity::Entity(draw_t w, draw_t h, components::TYPE_COMPONENT typeID):
		m_Physics{ components::Physics{w, h, typeID} },
		m_Properties{ components::Properties{w, h, typeID} },
		m_Data{ std::make_unique<uint8_t[]>(m_Properties.size) } 	
	{
		//dbg("Entity w:%d h:%d typeID:%d", w, h, typeID); 
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
			tools::bgraToRgba(pData, m_Properties.size);
			bRet = true;
		}

		dbg("loadPng %s (ret:%d)", szPath, bRet); 
		return bRet;
	}

} //draw