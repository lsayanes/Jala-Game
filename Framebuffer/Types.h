
#pragma once

namespace draw
{
    struct DBGW
    {
        draw_t x{ 0 };
        draw_t y{ 0 };
        void* pFont{ nullptr };
    };


    struct Rect
    {
        draw_t left{ 0 };
        draw_t top{ 0 };
        draw_t right{ 0 };
        draw_t bottom{ 0 };
        
        const draw_t width;
        const draw_t height;

        explicit Rect(draw_t x, draw_t y, draw_t w, draw_t h):
            width{w},
            height{h}
        {
            left = x;
            top = y;
            right = left + w;
            bottom = top + h;
        }

        explicit Rect(const Rect& rc):
            width{ rc.right- rc.left },
            height{ rc.bottom - rc.top }
        {
            left = rc.left;
            top = rc.top;
            right = rc.right;
            bottom = rc.bottom;
        }

        inline void pos(draw_t x, draw_t y)
        {
            left = x;
            top = y;
            right = left + width;
            bottom = top + height;
        }

        inline void posx(draw_t x)
        {
            left = x;
            right = left + width;
        }

        inline void posy(draw_t y)
        {
            top = y;
            bottom = top + height;
        }

        inline void moveUp(draw_t points)
        {
            top = top - points;
            bottom = top + height;
        }

        inline void moveDown(draw_t points)
        {
            top = top + points;
            bottom = top + height;
        }

        inline void moveLeft(draw_t point)
        {
            left = left - point;
            right = left + width;
        }

        inline void moveRight(draw_t point)
        {
            left = left + point;
            right = left + width;
        }
    };
}