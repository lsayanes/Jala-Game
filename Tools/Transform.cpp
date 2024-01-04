
#include <stdint.h>
#include <cstring>
#include <cmath>

#include "Transform.h"


namespace draw
{
    namespace Transform
    {
        uint8_t* rotate(
                        uint8_t     *prgbaData,
                        int32_t     width,
                        int32_t     height,
                        float       angleToRotate,
                        uint8_t     *backgroundColor
                        )
        {
            // Calculate sine and cosine of the rotation angle
            float cosTheta = std::cos(angleToRotate);
            float sinTheta = std::sin(angleToRotate);

            // Calculate the center of the image
            float centerX = static_cast<float>(width) / 2.0f;
            float centerY = static_cast<float>(height) / 2.0f;

            uint64_t size = (width * 4) * height;
            uint8_t* pImageRotated = new uint8_t[size + 1];

            if(pImageRotated)
            {
                if(!backgroundColor)
                {
                    std::memset(pImageRotated, 0, size);
                }
                else
                {
                    //filling a pattern
                    for(uint64_t f = 0; f < size; f+=4)
                        std::memcpy(pImageRotated + f, backgroundColor, 4);
                }

                float offsetX, offsetY;
                int32_t rotatedY, rotatedX;
                int32_t dest, src;

                for (int32_t y = 0; y < height; ++y)
                {
                    for (int32_t x = 0; x < width; ++x)
                    {
                        // Translate the pixel coordinates to be relative to the center
                        offsetX = static_cast<float>(x) - centerX;
                        offsetY = static_cast<float>(y) - centerY;

                        // Apply the rotation transformation
                        rotatedX = static_cast<int>(centerX + (offsetX * cosTheta - offsetY * sinTheta));
                        rotatedY = static_cast<int>(centerY + (offsetX * sinTheta + offsetY * cosTheta));

                        // Check if the rotated coordinates are within bounds
                        if (rotatedX >= 0 && rotatedX < width && rotatedY >= 0 && rotatedY < height)
                        {
                            dest = (y * width + x) * 4;
                            src = (rotatedY * width + rotatedX) * 4;

                            pImageRotated[dest + 0] = prgbaData[src + 0];
                            pImageRotated[dest + 1] = prgbaData[src + 1];
                            pImageRotated[dest + 2] = prgbaData[src + 2];
                            pImageRotated[dest + 3] = prgbaData[src + 3];
                        }
                    }
                }

                return pImageRotated;
            }

            return nullptr;
        }
    }
}; //draw