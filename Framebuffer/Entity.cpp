
#include <vector>
//#include <unordered_map>
#include <memory>
#include <stdint.h>

#include <Properties.h>
#include <Physics.h>

#include "Entity.h"

namespace draw
{
	Entity::Entity(size_t w, size_t h, unsigned char byBitPixels):
		physics{ components::Physics<unsigned short> {0, 0} },
		properties{ components::Properties<size_t> {w, h, byBitPixels} },
		data{ std::make_unique<unsigned char[]>(properties.size) } 
	{
		//fill(255, 0, 255);
	}


	Entity::~Entity()
	{
	}

} //draw