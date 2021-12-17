#pragma once

#include <vector>
#include <memory>
#include <stdint.h>

#include "Raster.h"
#include "Entity.h"

namespace draw
{
	Entity::Entity(size_t w, size_t h, unsigned char byBitPixels) : Raster { w, h, byBitPixels},
		nX{ 0 },
		nY{ 0 },
		data{ std::make_unique<unsigned char[]>(stSize) }
	{
		pbyBuffer = data.get();
	}


	Entity::~Entity()
	{
	}

} //draw