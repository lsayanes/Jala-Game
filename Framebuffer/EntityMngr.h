namespace draw
{
	class EntityMngr 
	{
	private:
		std::unordered_map<std::string, Entity*>						m_Entities{};
		std::unordered_map<std::string, std::vector<Entity*>*>			m_Text{};
		
		FrameBuffer															&m_FrameBufferRef;
	public:
		static constexpr size_t INI_MAX_ENTITIES {1024};

		explicit EntityMngr(FrameBuffer &fbuffer);
		virtual ~EntityMngr();

		void renderAll() noexcept;
		void renderEntities() noexcept;
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

		[[nodiscard]] inline FrameBuffer& frameBuffer() const noexcept  { return m_FrameBufferRef; };
		inline void fill(uint8_t r, uint8_t g, uint8_t b) noexcept { m_FrameBufferRef.fill(r, g, b); };

	};
};// draw