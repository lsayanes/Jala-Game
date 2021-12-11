#pragma once

#include <vector>
#include <memory>
#include <stdint.h>

#include "Entity.h"

namespace draw
{
	Entity::Entity(size_t w, size_t h, unsigned char byBitPixels) :
		stW{ w },
		stH{ h },
		nX{ 0 },
		nY{ 0 },
		byBits{ byBitPixels },
		stSize{ (w * (byBits >> 3)) * h },
		data{ std::make_unique<unsigned char[]>(stSize) }
	{
	}


	Entity::~Entity()
	{
	}


	void Entity::fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		uint32_t pixel = ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint16_t)g << 8) | b;
		for (size_t i = 0; i < stSize; i += sizeof(pixel))
			std::memcpy(&data[i], &pixel, sizeof(pixel));

	}

}