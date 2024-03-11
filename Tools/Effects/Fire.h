
#pragma once


namespace draw
{
    namespace effects
    {

        class Fire : public Effect
        {
            private:
                draw::Entity    &m_Entity;
                uint64_t        m_ullFireSize{0};
                uint8_t         *m_pbyFireBuffer {nullptr};
                uint8_t         *m_pbyOldFireBuffer {nullptr};

            private:
                void makeAshes();
            public:
                static constexpr uint32_t totalPaletteElements {256};
                explicit Fire(draw::Entity &EntityRf);
                ~Fire();

                void createPalette();
                void drawPalette();

                void doFrame();

        };
    };//effects
};//draw