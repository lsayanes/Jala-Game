
#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <stdint.h>
#include <fstream>
#include <algorithm>


#include <Config.h>

#include <Component.h>
#include <Properties.h>
#include <Physics.h>
#include <Tools.h>
#include "Device.h"
#include "Entity.h"

#include "Sprite.h"
//#include "FrameBuffer.h"

namespace draw
{
	Sprite::Sprite(size_t w, size_t h, uint8_t bits, size_t stTotal):
		m_Entities {stTotal}
	{
		if(stTotal <= 0)
			throw std::invalid_argument("Total entities==0");

		for (size_t i = 0; i < stTotal; i++)
			m_Entities[i] = new Entity{ w, h, bits, 0 };
	}

	Sprite::~Sprite()
	{
		std::for_each(m_Entities.begin(), m_Entities.end(), [&](Entity *it) { delete it; it = nullptr; });
		m_Entities.clear();
	}

	bool Sprite::load(size_t stIndex, const char* szPath)
	{
		/*
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
					unsigned long w{ pe->properties().w }, h{ pe->properties().h };
					char* p{ buffer.data() };
					unsigned char* pEntData = pe->data().get();
				
					if (0 == decodePNG(img, w, h, (unsigned char*)(p), pe->properties().size))
					{
						std::copy(begin(img), end(img), pEntData);
						draw::tools::bgraToRgba(pEntData, pe->properties().size);
						return true;
					}
				}
			}

		}
		*/
		return false;
	}

	void Sprite::pos(int x, int y)
	{

		std::for_each(m_Entities.begin(), m_Entities.end(), [&](Entity* it)
			{
				it->physics().x = x; 
				it->physics().y = y;
			});
	}

	const Entity& Sprite::operator[](size_t i) const
	{
		return reinterpret_cast<const Entity&>(*m_Entities.at(i));
	}

}//draw

