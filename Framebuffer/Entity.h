#pragma once


namespace draw
{
	class Entity
	{
	protected:
		components::Physics					m_Physics;
		components::Properties				m_Properties;
		//std::unique_ptr<uint8_t[]>			m_Data{};
		uint8_t								*m_Data;
	public:
		static constexpr const char *NonameEntity {"noname"};
	public:
		explicit Entity(draw_t w, draw_t h, const char *szName = NonameEntity);
		explicit Entity(draw_t w, draw_t h, uint8_t *pData, const char *szName = NonameEntity);
		virtual ~Entity();

		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&&) = delete;

		[[nodiscard]]  bool loadPng(const char* szPath);

		[[nodiscard]] inline components::Physics&			physics()		{ return m_Physics; };
		[[nodiscard]] inline components::Properties&		properties()	{ return m_Properties; };
		//[[nodiscard]] inline uint8_t 						*data()			{ return m_Data.get(); };
		[[nodiscard]] inline uint8_t						*data() const  { return m_Data; };

		operator size_t() const
		{
			return sizeof(*this) + m_Properties.size; 
		}

		inline void renderWithAlpha() { m_Properties.alpha = 1; };

	};

}