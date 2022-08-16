
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
        phy_type l{ 0 };    //left
        phy_type t{ 0 };    //top
        prop_type r{ 0 };   //rigth
        prop_type b{ 0 };   //bottom


        explicit RECT(phy_type x, phy_type y, prop_type w, prop_type h)
        {
            l = x;
            t = y;
            r = l + w;
            b = t + h;
        }
    };
}