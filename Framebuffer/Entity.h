#pragma once

namespace draw
{
	class Entity 
	{

		//std::optional<std::reference_wrapper<components::Physics<unsigned short>>> physics {};
		//components::Physics<unsigned short>		*physics {nullptr};

	protected:
		components::Physics<short>				m_Physics;
		components::Properties<size_t>			m_Properties;
		std::unique_ptr<unsigned char[]>		m_Data{};

	public:
		explicit Entity(prop_type w, prop_type h, unsigned char byBitPixels, size_t typeID);
		virtual ~Entity();

		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&&) = delete;


		bool loadPng(const char* szPath);

		inline components::Physics<phy_type>& physics()		{ return m_Physics; };
		inline components::Properties<prop_type>& properties() { return m_Properties; };
		inline std::unique_ptr<unsigned char[]>& data() { return m_Data; };

	//private:
	//	size_t		m_typID{0};
	};

}