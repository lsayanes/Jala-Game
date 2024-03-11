
#pragma once

namespace draw
{
    namespace effects
    {
        class Effect
        {
            private:
                std::unique_ptr<types::Pixel[]> m_Palette;
                const uint16_t paletteElements {0};

            protected:
                void copyPalette(const types::Pixel *pPalette);
                void drawPalette(uint8_t *pDest, uint16_t widthDest, uint16_t heightDest);


                const std::unique_ptr<types::Pixel[]> &palette() const  { return m_Palette; }

            public:
                explicit Effect(const uint16_t maxPaletteElements);


                Effect(const Effect&) = delete;
                Effect(Effect&&) = delete;
                Effect& operator=(const Effect&) = delete;
                Effect& operator=(Effect&&) = delete;

                virtual ~Effect();


                virtual void createPalette() = 0;


                const uint8_t   *bufferPalette() const;
                void printOutPalette() const;

                static void randomBuff(uint8_t *pBuff, uint32_t size, uint32_t ulRndLimit);
        };
    }; //effects
}; //draw
