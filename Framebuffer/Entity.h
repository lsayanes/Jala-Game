#pragma once

namespace draw
{
	struct Entity 
	{

		//std::optional<std::reference_wrapper<components::Physics<unsigned short>>> physics {};
		//components::Physics<unsigned short>		*physics {nullptr};


		components::Physics<unsigned short>     physics;
		components::Properties<size_t>			properties;
		std::unique_ptr<unsigned char[]>		data{};

		explicit Entity(size_t w, size_t h, unsigned char byBitPixels, size_t typeID);
		

		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&&) = delete;

		virtual ~Entity();

	//private:
	//	size_t		m_typID{0};
	};

}