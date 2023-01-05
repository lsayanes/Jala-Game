#pragma once

namespace draw
{
	class Entity 
	{

		//std::optional<std::reference_wrapper<components::Physics<unsigned short>>> physics {};
		//components::Physics<unsigned short>		*physics {nullptr};

	protected:
		components::Physics					m_Physics;
		components::Properties				m_Properties;
		std::unique_ptr<unsigned char[]>	m_Data{};

	public:
		explicit Entity(draw_t w, draw_t h, unsigned char byBitPixels, size_t typeID);
		virtual ~Entity();

		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&&) = delete;

		[[nodiscard]]  bool loadPng(const char* szPath);

		[[nodiscard]] inline components::Physics&					physics()		{ return m_Physics; };
		[[nodiscard]] inline components::Properties&				properties()	{ return m_Properties; };
		[[nodiscard]] inline std::unique_ptr<unsigned char[]>&	data()				{ return m_Data; };
		
		inline void renderWithAlpha() { m_Properties.alpha = 1; };

	//private:
	//	size_t		m_typID{0};
	};

}