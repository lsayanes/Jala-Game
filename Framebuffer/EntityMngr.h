
#pragma once
#include <map>
namespace draw
{
	class EntityMngr
	{
	public:
		std::unordered_map<std::string, Entity*>					m_Entities{};	 //render Unordered
		std::unordered_map<std::string, std::vector<Entity*>*>		m_Text{};
		std::vector<Entity*>										m_RenderLayout{}; //render Ordered
		std::unordered_map<std::string, Sprite*>					m_Sprites{};

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
		void renderSprites() noexcept;


		inline void addText(std::string szName, std::vector<Entity*>* pVct) noexcept
		{
			m_Text[szName] = pVct;
		}

		[[nodiscard]]  		Entity *create(std::string szName, draw_t w, draw_t h) noexcept;
		[[nodiscard]] const Entity *create(std::string szName, draw_t w, draw_t h, std::string szPath) noexcept;
		[[nodiscard]]  		Entity *create(std::string szName, draw_t w, draw_t h, uint8_t *pData) noexcept;
		[[nodiscard]]  		bool 	create(std::string szName, Sprite *pSprite) noexcept;

		[[nodiscard]] const bool remove(std::string szName) noexcept;

		Entity& operator[](std::string szName) const;

		[[nodiscard]] inline const Entity* get(std::string szName) const noexcept
		{
			auto it = m_Entities.find(szName);
			if (it != m_Entities.end())
				return it->second;

			return nullptr;
		}

		[[nodiscard]] inline size_t size() { return m_Entities.size(); }
	};
};// draw