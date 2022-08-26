
#include <vector>
//#include <unordered_map>
#include <memory>
#include <stdint.h>
#include <fstream>
#include <algorithm>

#include <Config.h>

#include <Component.h>
#include <Properties.h>
#include <Physics.h>

#include <Tools.h>

//#include "../Libs/picopng.hpp"
#include "../Libs/png.h"

#include "Entity.h"

namespace draw
{
	Entity::Entity(prop_type w, prop_type h, unsigned char byBitPixels, size_t typeID):
		m_Physics{ components::Physics<phy_type> {0, 0, typeID} },
		m_Properties{ components::Properties<prop_type> {w, h, byBitPixels, typeID} },
		m_Data{ std::make_unique<unsigned char[]>(m_Properties.size) } /*,
		m_typID{ typeID }*/
	{
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

		return bRet;
	}

} //draw