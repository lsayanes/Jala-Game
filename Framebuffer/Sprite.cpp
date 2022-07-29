
#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <algorithm>

#include "../Libs/picopng.hpp"


#include <Properties.h>
#include <Physics.h>
#include "Device.h"
#include "Entity.h"

#include "Sprite.h"


namespace draw
{
	Sprite::Sprite(size_t w, size_t h, uint8_t bits, size_t stTotal):
		m_Entities {stTotal}
	{
		for (size_t i = 0; i < stTotal; i++)
			m_Entities[i] = new Entity{ w, h, bits };
	}

	Sprite::~Sprite()
	{
		std::for_each(m_Entities.begin(), m_Entities.end(), [&](Entity *it) { delete it; it = nullptr; });
		m_Entities.clear();
	}

	bool Sprite::load(size_t stIndex, const char* szPath)
	{
		if (stIndex < m_Entities.size())
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
					Entity* pe{ m_Entities[stIndex] };
					unsigned long w{ pe->properties.w }, h{ pe->properties.h };
					char* p{ buffer.data() };
					unsigned char* pEntData = pe->data.get();
				
					if (0 == decodePNG(img, w, h, (unsigned char*)(p), pe->properties.size))
					{
						std::copy(begin(img), end(img), pEntData);
						bgraToRgba(pEntData, pe->properties.size);
						return true;
					}
				}
			}

		}
		
		return false;
	}

	void Sprite::bgraToRgba(uint8_t* pbyBgra, size_t stSize)
	{
		uint32_t i = 0, e = 0;
		uint8_t pixel[4];
		
		for (i = 0; i < stSize; i++)
		{
			if (i && 0 == i % 4)
			{
				pbyBgra[i - 4] = pixel[2];
				pbyBgra[i - 2] = pixel[0];
				e = 0;
			}
			pixel[e++] = pbyBgra[i];
		}
	}

	void Sprite::pos(int x, int y)
	{

	}

	const Entity& Sprite::operator[](size_t i) const
	{
		return reinterpret_cast<const Entity&>(*m_Entities.at(i));
	}

}//draw

