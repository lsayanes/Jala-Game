
#include <stdint.h>
#include <cstring>
#include <cmath>

#include "Transform.h"


namespace draw
{
    namespace Transform
    {
        uint8_t* rotate(
                        uint8_t     *prgbData,
                        int32_t     width,
                        int32_t     height,
                        float       angleToRotate,
                        uint8_t     *backgroundColor,
                        uint8_t     components
                        )
        {
            // Calculate sine and cosine of the rotation angle
            float cosTheta = std::cos(angleToRotate);
            float sinTheta = std::sin(angleToRotate);

            // Calculate the center of the image
            float centerX = static_cast<float>(width) / 2.0f;
            float centerY = static_cast<float>(height) / 2.0f;

            uint64_t size = (width * components) * height;
            uint8_t* pImageRotated = new uint8_t[size + 1];

            if(pImageRotated)
            {
                if(!backgroundColor)
                {
                    std::memset(pImageRotated, 0, size);
                }
                else
                {
                    for(uint64_t f = 0; f < size; f+=components)
                        std::memcpy(pImageRotated + f, backgroundColor, components);
                }

                for (int32_t y = 0; y < height; ++y)
                {
                    for (int32_t x = 0; x < width; ++x)
                    {
                        // Translate the pixel coordinates to be relative to the center
                        float offsetX = static_cast<float>(x) - centerX;
                        float offsetY = static_cast<float>(y) - centerY;

                        // Apply the rotation transformation
                        int32_t rotatedX = static_cast<int>(centerX + (offsetX * cosTheta - offsetY * sinTheta));
                        int32_t rotatedY = static_cast<int>(centerY + (offsetX * sinTheta + offsetY * cosTheta));

                        // Check if the rotated coordinates are within bounds
                        if (rotatedX >= 0 && rotatedX < width && rotatedY >= 0 && rotatedY < height)
                        {
                            for(uint8_t c = 0; c < components; c++)
                                pImageRotated[(y * width + x) * components + c] = prgbData[(rotatedY * width + rotatedX) * components + c];
                        }
                    }
                }

                return pImageRotated;
            }

            return nullptr;
        }
    }
}; //draw