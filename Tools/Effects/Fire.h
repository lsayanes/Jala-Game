
#pragma once


namespace draw
{
    namespace effects
    {

        struct Weighting
        {
            explicit Weighting() :
                    upLeft{10},
                    up{20},
                    upRight{10},
                    downLeft{160},
                    down{320},
                    downRight{160}
            {
            }

            virtual ~Weighting() = default;

            uint16_t  upLeft;
            uint16_t  up;
            uint16_t  upRight;
            uint16_t  downLeft;
            uint16_t  down;
            uint16_t  downRight;


            inline uint16_t  calc()
            {
                return (upLeft + up + upRight + downLeft + down + downRight);
            }

        };

        class Fire : public Effect
        {
            private:
                draw::Entity    &m_Entity;
                uint64_t        m_ullFireSize{0};
                uint8_t         *m_pbyFireBuffer {nullptr};
                uint8_t         *m_pbyOldFireBuffer {nullptr};

            public:
                Weighting   wiegth;

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