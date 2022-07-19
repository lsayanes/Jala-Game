#pragma once

namespace draw
{
	struct Entity : public Raster
	{
		short int						nX;
		short int						nY;
		std::unique_ptr<unsigned char[]> data{};

		explicit Entity(size_t w, size_t h, unsigned char byBitPixels);
		virtual ~Entity();

/*
		template<typename TYPE>
		static size_t getType() noexcept
		{
			static std::size_t type{ ++typeID }; //como es un template, solo se incializara una vez por TYPE, si paso el mismo TYPE ejecutara solo return type
			return type;
		}

		template<typename CMP_TYPE>
		void addComponent(CMP_TYPE &cmp) 
		{
			auto type = getType<CMP_TYPE>();
			m_Component[type] = &cmp;
		}

		template<typename CMP_TYPE>
		CMP_TYPE *getComponent()
		{
			auto type = getType<CMP_TYPE>();
			auto it = m_Component.find(type);
			if (it != m_Component.end())
				return static_cast<CMP_TYPE*>(it->second);
		}

	private:
		inline static size_t typeID{ 0 }; //contador de tipo
		std::unordered_map<std::size_t, void*> m_Component;
*/
	};

}