
#include <stdint.h>
#include <cstring>
#include <cmath>

#include "Transform.h"

#define M_PI           (3.14159265358979323846)

namespace draw
{
    namespace Transform
    {
        uint8_t* rotate(
                        uint8_t     *prgbaData,
                        int32_t     width,
                        int32_t     height,
                        float       angleToRotate
                        )
        {

            //convert to radian
            angleToRotate = static_cast<float>(static_cast<double>(angleToRotate) * M_PI / 180.0);

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

                //fill transparent
                uint8_t transparent[4] = { 0xFF, 0xFF, 0xFF, 0 };
                for (uint64_t f = 0; f < size; f += 4)
                    std::memcpy(pImageRotated + f, transparent, 4);

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
                            if (0 == prgbaData[src + 3])
                            {
                                //alpha
                                pImageRotated[dest + 0] = 0xFF;
                                pImageRotated[dest + 1] = 0xFF;
                                pImageRotated[dest + 2] = 0xFF;
                                pImageRotated[dest + 3] = 0x00;
                            }
                            else
                            {
                                pImageRotated[dest + 0] = prgbaData[src + 0];
                                pImageRotated[dest + 1] = prgbaData[src + 1];
                                pImageRotated[dest + 2] = prgbaData[src + 2];
                                pImageRotated[dest + 3] = prgbaData[src + 3];
                            }
                        }
                    }
                }

                return pImageRotated;
            }

            return nullptr;
        }
    }
}; //draw