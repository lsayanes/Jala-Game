
#pragma once


namespace draw
{
    namespace effects
    {

        class Fire : public Effect
        {
            public:
                static constexpr uint16_t totalPaletteElements {256};
                explicit Fire();
                ~Fire();

                void createPalette();

        };
    };//effects
};//draw