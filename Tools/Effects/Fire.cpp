
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

            uint16_t i = 0;
            types::Pixel pixel = {};

            pixel.a = 0xFF;
            pixel.r = 0;
            pixel.g = 0xFF;
            pixel.b = 0;

            for(; i < totalPaletteElements; i++)
                palette[i] = pixel;
/*
            for(; i <= 84; i++)
            {
                pixel.r = i * (0xFF / 85);
                palette[i] = pixel;
            }

            pixel.r = 0xFF;
            for(i = 85; i <= 169; i++)
            {
                pixel.g = (i - 85) * (0xFF / 85);
                palette[i] = pixel;
            }

            pixel.g = 0xFF;
            for(i = 170; i < 255; i++)
            {
                pixel.b = (i - 170) * (0xFF / 86);
                palette[i] = pixel;
            }
*/
            copyPalette(palette);

        }

    };//effects
};//draw