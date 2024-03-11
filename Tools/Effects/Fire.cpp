
#include <iostream>

#include <stdint.h>
#include <memory>
#include <cstring>

#include <Config.h>
#include <Types.h>
#include <Properties.h>
#include <Physics.h>
#include <Entity.h>

#include "Effect.h"
#include "Fire.h"


namespace draw
{
    namespace effects
    {


        Fire::Fire(draw::Entity &EntityRf):Effect(totalPaletteElements),
            m_Entity { EntityRf },
            m_ullFireSize {m_Entity.properties().size / m_Entity.properties().components()},
            m_pbyFireBuffer { new uint8_t[m_ullFireSize] },
            m_pbyOldFireBuffer { new uint8_t[m_ullFireSize] }
        {
            createPalette();
        }

        Fire::~Fire()
        {
            if(m_pbyFireBuffer)
                delete[] m_pbyFireBuffer;

            m_pbyFireBuffer = nullptr;

        }

        void Fire::createPalette()
        {

            types::Pixel palette[totalPaletteElements] = {};

            uint32_t i = 0;
            uint32_t ulParts = totalPaletteElements / 3;
            uint32_t ulThird = ulParts * 2;

            types::Pixel pixel = {};

            pixel.a = 0xFF;
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
            std::memset(m_pbyFireBuffer, 0, m_ullFireSize);
        }

        void Fire::drawPalette()
        {
            components::Properties& prop { m_Entity.properties() };
            Effect::drawPalette(m_Entity.data(), prop.width, prop.height);
        }

        void Fire::makeAshes()
        {
            components::Properties& prop { m_Entity.properties() };
            uint8_t *pFireBuff = m_Entity.data() + (prop.size - prop.lineSize);
            const std::unique_ptr<types::Pixel[]> &pixels = palette();
            types::Pixel pixel;

            for(size_t i = 0; i < prop.lineSize; i+=4)
            {
                pixel = pixels[rand() % 0xFF];

                *(pFireBuff + i + 0) = pixel.r;
                *(pFireBuff + i + 1) = pixel.g;
                *(pFireBuff + i + 2) = pixel.b;

            }
        }

        void Fire::doFrame()
        {

            components::Properties& prop { m_Entity.properties() };
            const std::unique_ptr<types::Pixel[]> &pixels = palette();

            uint8_t *pEntityBuffer = m_Entity.data();

            //ashes
            uint64_t ulLastRow = (prop.height  * prop.width) - prop.width;
            uint64_t ulEnd = ulLastRow + prop.width;

            Effect::randomBuff(&m_pbyFireBuffer[ulLastRow], prop.width, totalPaletteElements);

            types::Pixel pixel;
            uint64_t i = ulLastRow;

            for(; i < ulEnd - 1; i++)
            {
                pixel = pixels[m_pbyFireBuffer[i]];
                std::memcpy(&pEntityBuffer[i * sizeof(pixel)], &pixel, sizeof(pixel));
            }

            std::memcpy(m_pbyOldFireBuffer, m_pbyFireBuffer, m_ullFireSize);

            uint64_t pix;
            for(draw_t y = 1; y < (prop.height-1); y++)
            {
                pix = y * prop.width;
                for(draw_t x = 1; x < (prop.width-1); x++)
                {
                    i = pix + x;
                    m_pbyFireBuffer[i] = (uint8_t)((
                    10 * m_pbyOldFireBuffer[i - 1]
                    +  20 * m_pbyOldFireBuffer[i + 0]
                    +  10 * m_pbyOldFireBuffer[i + 1]
                    + 160 * m_pbyOldFireBuffer[i - 1 + prop.width]
                    + 320 * m_pbyOldFireBuffer[i + 0 + prop.width]
                    + 160 * m_pbyOldFireBuffer[i + 1 + prop.width]
                    ) / 680);
                }
            }


            for(i = 0; i < m_ullFireSize; i++)
            {
                pixel = pixels[m_pbyFireBuffer[i]];
                std::memcpy(&pEntityBuffer[i * sizeof(pixel)], &pixel, sizeof(pixel));
            }

        }

    };//effects
};//draw