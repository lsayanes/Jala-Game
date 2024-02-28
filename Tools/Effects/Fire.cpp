
#include <iostream>

#include <stdint.h>
#include <memory>
#include <cstring>

#include <Config.h>
#include <Types.h>

#include "Effect.h"
#include "Fire.h"


namespace draw
{
    namespace effects
    {


        Fire::Fire():Effect(totalPaletteElements)
        {
            createPalette();
        }

        Fire::~Fire()
        {
        }

        void Fire::createPalette()
        {

            types::Pixel palette[totalPaletteElements] = {};

            uint32_t i = 0;
            uint32_t ulParts = totalPaletteElements / 3;
            uint32_t ulThird = ulParts * 2;

            types::Pixel pixel = {};

            for(; i <= ulParts; i++)
            {
                pixel.r = i * (0xFF / ulParts);
                palette[i] = pixel;
            }

            pixel.r = 0xFF;
            for(i = ulParts; i <= ulThird; i++)
            {
                pixel.g = (i - ulParts) * (0xFF / ulParts);
                palette[i] = pixel;
            }

            pixel.g = 0xFF;
            pixel.r = 0xFF;
            for(i = ulThird; i < totalPaletteElements; i++)
            {
                pixel.b = (i - ulThird) * (0xFF / ulParts);
                palette[i] = pixel;
            }

            copyPalette(palette);

        }

    };//effects
};//draw