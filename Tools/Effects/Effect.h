
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

            public:
                explicit Effect(const uint16_t maxPaletteElements);


                Effect(const Effect&) = delete;
                Effect(Effect&&) = delete;
                Effect& operator=(const Effect&) = delete;
                Effect& operator=(Effect&&) = delete;

                virtual ~Effect();


                virtual void createPalette() = 0;

                void            drawPalette(uint8_t *pDest, uint16_t widthDest, uint16_t heightDest);
                const uint8_t   *palette() const;

                void printOutPalette() const;
        };
    }; //effects
}; //draw
