
#pragma once
#include <map>
namespace draw
{
	class EntityMngr 
	{
	private:
		std::unordered_map<std::string, Entity*>					m_Entities{};	 //render Unordered	
		std::unordered_map<std::string, std::vector<Entity*>*>		m_Text{};
		std::vector<Entity*>										m_RenderLayout{}; //render Ordered

		FrameBuffer													&m_FrameBufferRef;
	
	private:
		[[nodiscard]] const size_t add(std::string szName, Entity *pEntity); 
	
	public:
		static constexpr size_t INI_MAX_ENTITIES {1024};

		explicit EntityMngr(FrameBuffer &fbuffer);
		virtual ~EntityMngr();

		void renderUnordered() noexcept;
		void renderOrdered() noexcept;

		void renderEntities() noexcept;
		void render(const std::string name) noexcept;
		void renderText() noexcept;
	

		inline void addText(std::string szName, std::vector<Entity*>* pVct) noexcept
		{
			m_Text[szName] = pVct;
		}

		[[nodiscard]] const size_t create(std::string szName, draw_t w, draw_t h) noexcept;
		[[nodiscard]] const size_t create(std::string szName, draw_t w, draw_t h, std::string szPath) noexcept;
		
		Entity& operator[](std::string szName) const;

		[[nodiscard]] inline Entity* get(std::string szName) const noexcept
		{
			auto it = m_Entities.find(szName);
			if (it != m_Entities.end())
				return it->second;

			return nullptr;
		}

		[[nodiscard]] inline size_t size() { return m_Entities.size(); }		
	};
};// draw