#pragma once

namespace draw
{
	struct Entity 
	{

		components::Physics<unsigned short>		physics;
		components::Properties<size_t>			properties;
		std::unique_ptr<unsigned char[]>		data{};

		explicit Entity(size_t w, size_t h, unsigned char byBitPixels);


		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity& operator=(Entity&&) = delete;

		virtual ~Entity();
	};

}