



#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdint.h>
#include <algorithm>

#include "Config.h"

#include "Types.h"

#include "../Components/Physics.h"
#include "../Components/Properties.h"

#include "Entity.h"
#include <Sprite.h>
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
		m_Sprites.reserve(INI_MAX_ENTITIES);

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
		m_Sprites.clear();
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

		std::for_each(m_Sprites.begin(), m_Sprites.end(),
		[&](std::unordered_map <std::string, Sprite*>::value_type& spt)
		{
			m_FrameBufferRef.put(spt.second->get());
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

	void EntityMngr::renderSprites() noexcept
	{
		std::for_each(m_Sprites.begin(), m_Sprites.end(),
		[&](std::unordered_map <std::string, Sprite*>::value_type& it)
		{
			m_FrameBufferRef.put(it.second->get());
		});
	}

	void EntityMngr::render(const std::string name) noexcept
	{
		m_FrameBufferRef.put(*m_Entities.at(name));
	}

	Entity *EntityMngr::create(std::string szName, draw_t w, draw_t h) noexcept
	{
		auto *pEntity = new Entity{ w, h, components::ATC_NONE, szName.c_str() };
		if(pEntity)
		{
			auto currt = m_Entities.size();
  			if(add(szName, pEntity) == currt)
			{
				delete pEntity;
				pEntity = nullptr;
			}
		}

		return pEntity;
	}

	const Entity * EntityMngr::create(std::string szName, draw_t w, draw_t h, std::string szPath) noexcept
	{
		draw::Entity *img = new draw::Entity{ w, h, components::ATC_NONE, szName.c_str() };
		if (img->loadPng(szPath.c_str()))
		{
			if(img)
			{
				auto currt = m_Entities.size();
				if(add(szName, img) == currt)
				{
					delete img;
					img = nullptr;
				}
			}
		}

		return img;
	}

	Entity * EntityMngr::create(std::string szName, draw_t w, draw_t h, uint8_t *pData) noexcept
	{
		draw::Entity *pEntity{nullptr};
		if(pData)
		{
			pEntity = new draw::Entity{ w, h, pData, components::ATC_NONE, szName.c_str() };
			if(pEntity)
			{
				auto currt = m_Entities.size();
				if(add(szName, pEntity) == currt)
				{
					delete pEntity;
					pEntity = nullptr;
				}
			}
		}

		return pEntity;
	}

	bool EntityMngr::create(std::string szName, Sprite *pSprite) noexcept
	{
		auto ret {m_Sprites.emplace(std::make_pair(szName, pSprite)) };
		return ret.second;
	}

	const bool EntityMngr::remove(std::string szName) noexcept
	{
		Entity *pEntity;
		auto it = m_Entities.find(szName);
		if(it != m_Entities.end())
		{
			pEntity = it->second;

			auto itLayout = std::find_if(m_RenderLayout.begin(), m_RenderLayout.end(),
			[&](std::vector<Entity*>::value_type &ite)
			{
				return (pEntity == ite);
			});

			if(itLayout != m_RenderLayout.end())
				m_RenderLayout.erase(itLayout);

			m_Entities.erase(it);
			return true;
		}

		return false;
	}

	const size_t EntityMngr::add(std::string szName, Entity *pEntity)
	{
		m_Entities.emplace(std::make_pair(szName, pEntity));
		m_RenderLayout.push_back(pEntity);
		dbg("emplace :%s size: %lu", szName.c_str(), m_Entities.size());
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