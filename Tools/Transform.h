
#pragma once

namespace draw
{
    namespace Transform
    {
        //return a new rotated image
        uint8_t* rotate(
            uint8_t* prgbData,          //Original rgb Image data
            int32_t width,             //width in pixels
            int32_t height,
            float angleToRotate,        //angle to rotate
            uint8_t *backgroundColor = nullptr, //background color for the new rotated image
            uint8_t components = 4 //rgba typically
        );
    };
}; //draw