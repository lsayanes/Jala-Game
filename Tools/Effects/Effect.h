
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

            public:
                explicit Effect(const uint16_t maxPaletteElements);


                Effect(const Effect&) = delete;
                Effect(Effect&&) = delete;
                Effect& operator=(const Effect&) = delete;
                Effect& operator=(Effect&&) = delete;

                virtual ~Effect();


                virtual void createPalette() = 0;

                void copyPalette(const types::Pixel *pPalette);

                const uint8_t *palette() const;

                void printPalette() const;
        };
    }; //effects
}; //draw
