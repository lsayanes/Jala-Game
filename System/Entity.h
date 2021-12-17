#pragma once

namespace draw
{

	struct Entity : public Raster
	{
		int								nX;
		int								nY;
		std::unique_ptr<unsigned char[]> data{};

		explicit Entity(size_t w, size_t h, unsigned char byBitPixels);
		virtual ~Entity();

	};

}