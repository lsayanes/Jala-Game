



#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <algorithm>
#include <optional>

#include <Config.h>

#include <Types.h>
#include <Component.h>
#include <Properties.h>
#include <Physics.h>

#include <Tools.h>

#include "Device.h"
#include "Entity.h"
#include "FontLib.h"
#include "CharSet.h"

#include "FrameBuffer.h"

#include "EntityMngr.h"

namespace draw
{
	EntityMngr::EntityMngr( FrameBuffer& fbuffer):
		m_FrameBufferRef{fbuffer}
	{
		m_Entities.reserve(INI_MAX_ENTITIES);
		m_Text.reserve(INI_MAX_ENTITIES);
	}

	EntityMngr::~EntityMngr()
	{
		std::for_each(m_Entities.begin(), m_Entities.end(),
			[&](std::unordered_map<std::string, Entity*>::value_type& it)
			{
				delete it.second;
			});

		m_Entities.clear();
	}

	void EntityMngr::renderAll() noexcept
	{
		std::for_each(m_Entities.begin(), m_Entities.end(),
			[&](std::unordered_map<std::string, Entity*>::value_type &it)
			{
				m_FrameBufferRef.put(*it.second); 
			});


		std::for_each(m_Text.begin(), m_Text.end(),
			[&](std::unordered_map <std::string, std::vector<Entity*>*>::value_type& txt)
			{
				m_FrameBufferRef.put(*txt.second);
			});
	}


	const prop_type EntityMngr::create(std::string szName, prop_type w, prop_type h) noexcept
	{
		m_Entities.emplace(std::make_pair(szName, new Entity{ w, h, bpp(), 0 }));
		return m_Entities.size();
	}

	const prop_type EntityMngr::create(std::string szName, prop_type w, prop_type h, std::string szPath) noexcept
	{
		draw::Entity *img = new draw::Entity{ w, h, bpp(), 0 };
		if (img->loadPng(szPath.c_str()))
		{
			m_Entities.emplace(std::make_pair(szName, img));
			return m_Entities.size();
		}
		
		return 0;
	}

	Entity& EntityMngr::operator[](std::string szName) const
	{
		auto it = m_Entities.find(szName);
		if (it != m_Entities.end())
			return *it->second;

		throw std::invalid_argument(szName.c_str());
	}



};// draw