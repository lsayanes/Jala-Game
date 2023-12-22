#pragma once


namespace draw
{
	class Entity
	{
	protected:
		components::Physics					m_Physics;
		components::Properties				m_Properties;
		std::unique_ptr<uint8_t[]>			m_Data{};

	public:
		explicit Entity(draw_t w, draw_t h, components::TYPE_COMPONENT typeID);
		explicit Entity(draw_t w, draw_t h, uint8_t *pData, components::TYPE_COMPONENT typeID);
		virtual ~Entity();

		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&&) = delete;

		[[nodiscard]]  bool loadPng(const char* szPath);

		[[nodiscard]] inline components::Physics&			physics()		{ return m_Physics; };
		[[nodiscard]] inline components::Properties&		properties()	{ return m_Properties; };
		[[nodiscard]] inline std::unique_ptr<uint8_t[]>&	data()				{ return m_Data; };

		inline void renderWithAlpha() { m_Properties.alpha = 1; };

	};

}