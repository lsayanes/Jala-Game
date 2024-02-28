
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

        void Effect::drawPalette(uint8_t *pDest, uint16_t widthDest, uint16_t heightDest)
        {

            const uint8_t *pPalette = palette();

            uint32_t i, y = 0, x = 0;
            uint32_t y_start = heightDest - paletteElements;
            uint32_t ulPaletteJump;

            for(x = 0; x < widthDest; x++)
            {
                for(y = 0; y < paletteElements; y++)
                {
                    i = 4 * (x + ((y + y_start) * widthDest));
                    ulPaletteJump = 4 * y;

                    pDest[i + 0] = pPalette[ulPaletteJump + 2]; //r
                    pDest[i + 1] = pPalette[ulPaletteJump + 1]; //g
                    pDest[i + 2] = pPalette[ulPaletteJump + 0]; //b
                    pDest[i + 3] = pPalette[ulPaletteJump + 3]; //a
                }
            }
        }

        const uint8_t *Effect::palette() const
        {
            types::Pixel *pData = m_Palette.get();
            return (const uint8_t*)(pData);
        }

        void Effect::printOutPalette() const
        {
            const uint8_t *pPalette = palette();

            std::cout << std::endl << "Palette :" << std::endl;

            for(uint32_t i = 1; i < paletteElements + 1; i++)
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