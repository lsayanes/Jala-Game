
#pragma once

namespace draw
{

    struct Raster 
    {

        size_t							        stW;
        size_t							        stH;
        const   uint8_t                         MaxComponents;
        size_t							        stSize;
        size_t	                                stSizeLine;
        uint8_t                                 *pbyBuffer;
        std::unique_ptr<unsigned char[]>		m_Line;

        
        explicit Raster(size_t w, size_t h, uint8_t byBitPixels):
            stW{ w },
            stH{ h },
            MaxComponents{ static_cast<const uint8_t>( byBitPixels >> 3) },
            stSize{ (w * (MaxComponents)) * h },
            stSizeLine { w * MaxComponents},
            pbyBuffer{ nullptr } ,
            m_Line{ std::make_unique<unsigned char[]>(stW * MaxComponents) }
        {

        }

        virtual ~Raster()
        {

        }

        inline void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            size_t y = 0;
            size_t offset;

            unsigned char* pbyPix = pbyBuffer;
            unsigned char* pbyLine = m_Line.get();

            uint32_t pixel = Raster::buildPixel(r, g, b, a);

            for (uint32_t ud = 0; ud < stSizeLine; ud += MaxComponents)
                std::memcpy(&pbyLine[ud], &pixel, MaxComponents);

            while (y < stH)
            {
                offset = (y * stSizeLine);
                std::memcpy(pbyPix + offset, pbyLine, stSizeLine);
                y++;
            }
        }

        inline void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            uint32_t pixel = Raster::buildPixel(r, g, b, a);
            std::memcpy(pbyBuffer + (std::abs(y) * stSizeLine) + (std::abs(x) * MaxComponents), &pixel, MaxComponents);
        }

        static uint32_t buildPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            return  ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint16_t)g << 8) | b;
        }

    };

}//draw