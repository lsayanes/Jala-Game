
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

#include "Entity.h"

namespace draw
{
	Entity::Entity(draw_t w, draw_t h, unsigned char byBitPixels, components::TYPE_COMPONENT typeID):
		m_Physics{ components::Physics{w, h, typeID} },
		m_Properties{ components::Properties{w, h, byBitPixels, typeID} },
		m_Data{ std::make_unique<unsigned char[]>(m_Properties.size) } 	
	{
		//std::cout << "Entity w:" << w << " h: " << h << " bpp: " << (int)byBitPixels << " Created" << std::endl; 
	}


	Entity::~Entity()
	{
	}

	bool Entity::loadPng(const char* szPath)
	{
		bool bRet = false;
		Png png{};
		
		if (png.load(szPath, m_Properties.bpp()))
		{
			unsigned char* pData = m_Data.get();
			std::memcpy(pData, png.pRawData, m_Properties.size);
			tools::bgraToRgba(pData, m_Properties.size);
			bRet = true;
		}

		//std::cout << "Entity::loadPng " << szPath << " bRet:" << bRet << std::endl; 
		return bRet;
	}

} //draw