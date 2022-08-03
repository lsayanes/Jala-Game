namespace draw
{
	class EntityMngr 
	{
	private:
		std::unordered_map<std::string, const Entity*>						m_Entities{};
		std::unordered_map<std::string, const std::vector<const Entity*>*>	m_Text{};
		
		FrameBuffer															&m_FrameBufferRef;
		unsigned char														*m_pbyBackColor;
	public:
		static constexpr size_t INI_MAX_ENTITIES {1024};

		explicit EntityMngr( FrameBuffer &fbuffer);
		virtual ~EntityMngr();

		void renderAll();

		inline void add(std::string szName, const std::vector<const Entity*>* pVct)
		{
			m_Text[szName] = pVct;
		}

		inline void add(std::string szName, const Entity* pe)
		{
			m_Entities[szName] = pe;
		}

		inline const Entity* get(std::string szName)
		{
			auto it = m_Entities.find(szName);
			if (it != m_Entities.end())
				return it->second;

			return nullptr;
		}

		inline FrameBuffer& frameBuffer() const { return m_FrameBufferRef; };
	};
};// draw