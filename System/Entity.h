#pragma once

namespace draw
{

	struct Entity
	{
		size_t							stW;
		size_t							stH;
		int								nX;
		int								nY;
		uint8_t							byBits;
		size_t							stSize;

		//uint8_t							*data{nullptr};
		std::unique_ptr<unsigned char[]> data{};

		explicit Entity(size_t w, size_t h, unsigned char byBitPixels);
		virtual ~Entity();

		void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		inline const size_t lineWidth() const { return static_cast<const size_t>(stW * byBits >> 3); }

	};

}