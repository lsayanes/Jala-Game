



#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdint.h>
#include <algorithm>
//#include <optional>

#include "Config.h"

#include "Types.h"

#include "../Components/Component.h"
#include "../Components/Physics.h"
#include "../Components/Properties.h"

#include "Device.h"
#include "Entity.h"
#include "FontLib.h"
#include "CharSet.h"

#include "FrameBuffer.h"

#include <debug.h>

#include "EntityMngr.h"

namespace draw
{
	EntityMngr::EntityMngr( FrameBuffer& fbuffer):
		m_FrameBufferRef{fbuffer}
	{
		m_Entities.reserve(INI_MAX_ENTITIES);
		m_Text.reserve(INI_MAX_ENTITIES);

		m_RenderLayout.reserve(INI_MAX_ENTITIES);
	}

	EntityMngr::~EntityMngr()
	{
		std::for_each(m_Entities.begin(), m_Entities.end(),
			[&](std::unordered_map<std::string, Entity*>::value_type& it)
			{
				delete it.second;
			});

		m_Entities.clear();
		m_RenderLayout.clear();
	}

	void EntityMngr::renderUnordered() noexcept
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

	void EntityMngr::renderOrdered() noexcept
	{
	
		m_FrameBufferRef.put(m_RenderLayout);
	
		std::for_each(m_Text.begin(), m_Text.end(),
			[&](std::unordered_map <std::string, std::vector<Entity*>*>::value_type& txt)
			{
				m_FrameBufferRef.put(*txt.second);
			});

	}	

	void EntityMngr::renderEntities() noexcept
	{
		std::for_each(m_Entities.begin(), m_Entities.end(),
			[&](std::unordered_map<std::string, Entity*>::value_type &it)
			{
				m_FrameBufferRef.put(*it.second); 
			});

	}

	
	void EntityMngr::renderText() noexcept
	{
		std::for_each(m_Text.begin(), m_Text.end(),
			[&](std::unordered_map <std::string, std::vector<Entity*>*>::value_type& txt)
			{
				m_FrameBufferRef.put(*txt.second);
			});
	}

	void EntityMngr::render(const std::string name) noexcept
	{
		m_FrameBufferRef.put(*m_Entities.at(name));
	}

	const size_t EntityMngr::create(std::string szName, draw_t w, draw_t h) noexcept
	{
		auto *pEntity = new Entity{ w, h, components::TC_NONE };
		return add(szName, pEntity);
	}

	const size_t EntityMngr::create(std::string szName, draw_t w, draw_t h, std::string szPath) noexcept
	{
		draw::Entity *img = new draw::Entity{ w, h, components::TC_NONE };
		if (img->loadPng(szPath.c_str()))
			return add(szName, img);
		
		return 0;
	}

	const size_t EntityMngr::add(std::string szName, Entity *pEntity)
	{
		m_Entities.emplace(std::make_pair(szName, pEntity));
		m_RenderLayout.push_back(pEntity);
		dbg("emplace :%s size: %llu", szName.c_str(), m_Entities.size()); 
		
		return m_Entities.size();
	}

	Entity& EntityMngr::operator[](std::string szName) const
	{
		auto it = m_Entities.find(szName);
		if (it != m_Entities.end())
			return *it->second;

		throw std::invalid_argument(szName.c_str());
	}



};// draw