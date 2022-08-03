


#include <memory>
#include <string>

#include <unordered_map>
#include <stdint.h>
#include <algorithm>

#include <Types.h>
#include <Component.h>
#include <Properties.h>
#include <Physics.h>

#include "Device.h"
#include "Entity.h"
#include "Sprite.h"
#include "FontLib.h"
#include "CharSet.h"

#include "FrameBuffer.h"

#include "EntityMngr.h"

namespace draw
{
	EntityMngr::EntityMngr( FrameBuffer& fbuffer):
		m_FrameBufferRef{fbuffer},
		m_pbyBackColor{new unsigned char[fbuffer.properties().components()]}
	{
		m_Entities.reserve(INI_MAX_ENTITIES);
		m_Text.reserve(INI_MAX_ENTITIES);

		for(int i = 0; i < fbuffer.properties().components(); i++)
			*(m_pbyBackColor + i) = 255;
	}

	EntityMngr::~EntityMngr()
	{
		delete[] m_pbyBackColor;
	}

	void EntityMngr::renderAll()
	{

		m_FrameBufferRef.fill(m_pbyBackColor[0], m_pbyBackColor[1], m_pbyBackColor[2]);

		std::for_each(m_Entities.begin(), m_Entities.end(),
			[&](std::unordered_map<std::string, const Entity*>::value_type &it)
			{
				m_FrameBufferRef.put(*it.second); 
			}
		);

		std::for_each(m_Text.begin(), m_Text.end(),
			[&](std::unordered_map <std::string, const std::vector<const Entity*>*>::value_type& txt)
			{
				m_FrameBufferRef.put(*txt.second);
			});

		/*

		std::unordered_map<std::string, const Entity*>::iterator  it = m_Entities.begin();

		while (it != m_Entities.end())
		{
			m_FrameBufferRef.put(*it->second);
			it++;
		}
		*/

	}



};// draw