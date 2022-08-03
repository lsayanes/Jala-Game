
#include <vector>
//#include <unordered_map>
#include <memory>
#include <stdint.h>

#include <Component.h>
#include <Properties.h>
#include <Physics.h>

#include "Entity.h"

namespace draw
{
	Entity::Entity(size_t w, size_t h, unsigned char byBitPixels, size_t typeID):
		physics{ components::Physics<unsigned short> {0, 0, typeID} },
		properties{ components::Properties<size_t> {w, h, byBitPixels, typeID} },
		data{ std::make_unique<unsigned char[]>(properties.size) } /*,
		m_typID{ typeID }*/
	{
		//fill(255, 0, 255);
	}


	Entity::~Entity()
	{
	}

} //draw