

#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <stdint.h>
#include <algorithm>

#include <Component.h>
#include <Properties.h>
#include <Physics.h>

#include "Device.h"
#include "Entity.h"
#include "CharSet.h"

#include "FrameBuffer.h"

namespace draw
{

	FrameBuffer::FrameBuffer(size_t w, size_t h, uint8_t bits, void* pDevHandle) :
        Device{ pDevHandle },
        m_Properties{ components::Properties<size_t> {w, h, bits, 0} }
    {

        unsigned char comp = m_Properties.components();

        m_Line = std::make_unique<unsigned char[]>(m_Properties.lineSize);
        m_pbyBuffer = static_cast<unsigned char*>(Device::create(w, h, bits));

    }

	FrameBuffer::~FrameBuffer()
	{
	
	}

	bool FrameBuffer::isOk() const 
	{
		return getHandle() && m_pbyBuffer ? true : false;
	}

    void FrameBuffer::put(const Entity& e) const
    {
        
        size_t line = 0;
        size_t linSizeEnt = e.properties.lineSize;
        size_t offset;
        
        unsigned char* pbyPix = m_pbyBuffer;
        const int x = (e.physics.x * m_Properties.components()); 
        int y = e.physics.y;

        while (line < e.properties.h && y < static_cast<int>(m_Properties.h - 1) && y >= 0)
        {
            y = (line + e.physics.y);
            offset = ((y) *  m_Properties.lineSize) + x;
            std::memcpy(pbyPix + offset, e.data.get() + (line * linSizeEnt), linSizeEnt);
            line++;
        }

    }

    void FrameBuffer::put(const std::vector<const Entity*> &v) const
    {
        std::for_each(v.begin(), v.end(), [&](const Entity* it) { put(*it); });
    }

    void FrameBuffer::put(CharSet& c) const
    {
        const std::vector<const Entity*>* v = c.get();
        std::for_each(v->begin(), v->end(), [&](const Entity* it) { put(*it); });
    }

    void FrameBuffer::fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        size_t y = 0;
        size_t offset;
        size_t lineSize = m_Properties.lineSize;
        unsigned char comp = m_Properties.components();

        unsigned char* pbyPix = m_pbyBuffer;
        unsigned char* pbyLine = m_Line.get();

        uint32_t pixel = buildPixel(r, g, b, a);

        for (uint32_t ud = 0; ud < lineSize; ud += comp)
            std::memcpy(&pbyLine[ud], &pixel, comp);

        while (y < m_Properties.h)
        {
            offset = (y * lineSize);
            std::memcpy(pbyPix + offset, pbyLine, lineSize);
            y++;
        }
    }

    void FrameBuffer::bgraToRgba(uint8_t* pbyBgra, size_t stSize)
    {
        uint32_t i = 0, e = 0;
        uint8_t pixel[4];

        for (i = 0; i < stSize; i++)
        {
            if (i && 0 == i % 4)
            {
                pbyBgra[i - 4] = pixel[2];
                pbyBgra[i - 2] = pixel[0];
                e = 0;
            }
            pixel[e++] = pbyBgra[i];
        }
    }

}//draw