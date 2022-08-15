
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

#include "../Libs/picopng.hpp"

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
		std::ifstream file(szPath, std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			std::vector<unsigned char> img{};
			std::streamsize size = file.tellg();
			std::vector<char> buffer(static_cast<int>(size));
			file.seekg(0, std::ios::beg);

			if (file.read(buffer.data(), size))
			{
				unsigned long w{ m_Properties.w }, h{ m_Properties.h };
				char* p{ buffer.data() };
				unsigned char* pEntData = m_Data.get();

				if (0 == decodePNG(img, w, h, (unsigned char*)(p), m_Properties.size))
				{
					std::copy(begin(img), end(img), pEntData);
					tools::bgraToRgba(pEntData, m_Properties.size);
					return true;
				}
			}
		}

		return false;
	}

} //draw