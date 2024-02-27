
#include <iostream>
#include <iomanip>
#include <stdint.h>
#include <memory>
#include <cstring>

#include <Config.h>
#include <Types.h>

#include "Effect.h"

namespace draw
{
    namespace effects
    {

        Effect::Effect(const uint16_t maxPaletteElements):
            m_Palette { std::make_unique<types::Pixel[]>(maxPaletteElements) },
            paletteElements { maxPaletteElements }
        {

        }


        Effect::~Effect()
        {
        }

        void Effect::copyPalette(const types::Pixel *pPalette)
        {
            if(pPalette)
            {
                types::Pixel *pData = m_Palette.get();
                std::memcpy(pData, pPalette, sizeof(types::Pixel) * paletteElements);
            }
        }

        const uint8_t *Effect::palette() const
        {
            types::Pixel *pData = m_Palette.get();
            return (const uint8_t*)(pData);
        }

        void Effect::printPalette() const
        {
            const uint8_t *pPalette = palette();

            std::cout << std::endl << "Palette :" << std::endl;

            for(uint16_t i = 1; i < paletteElements + 1; i++)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(pPalette[i-1]);
                if(i && 0 == i%4)
                    std::cout << " ";
                if(i && 0 == i%16)
                    std::cout << std::endl;


            }
        }
    }; //effects
};