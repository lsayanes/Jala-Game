
#pragma once

namespace draw
{
    namespace Transform
    {
        //return a new rotated image
        uint8_t* rotate(
            uint8_t* prgbaData,          //Original rgba Image data
            int32_t width,             //width in pixels
            int32_t height,
            float angleToRotate        //angle to rotate
        );
    };
}; //draw