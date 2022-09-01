
#pragma once

namespace draw
{
    struct DBGW
    {
        phy_type x{ 0 };
        phy_type y{ 0 };
        void* pFont{ nullptr };
    };


    struct RECT
    {
        phy_type left{ 0 };   
        phy_type top{ 0 };    
        prop_type right{ 0 }; 
        prop_type bottom{ 0 };

        explicit RECT(phy_type x, phy_type y, prop_type w, prop_type h)
        {
            left = x;
            top = y;
            right = left + w;
            bottom = top + h;
        }

        inline void pos(phy_type x, phy_type y)
        {
            prop_type w = right - left;
            prop_type h = bottom - top;
          
            left = x;
            top = y;
            right = left + w;
            bottom = top + h;
        }
    };
}