

#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <stdint.h>
#include <algorithm>

#include <Config.h>

#include <Component.h>
#include <Properties.h>
#include <Physics.h>

#include "Device.h"
#include "Entity.h"
#include "CharSet.h"

#include "FrameBuffer.h"

namespace draw
{

	FrameBuffer::FrameBuffer(prop_type w, prop_type h, uint8_t bits, void* pDevHandle) :
        Device{ pDevHandle },
        m_Properties{ components::Properties<prop_type> {w, h, bits, 0} }
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

    void FrameBuffer::put(Entity& e) const
    {
        auto &physicsRef = e.physics();
        auto &propRef = e.properties();
        auto data = e.data().get();

        size_t line = 0;
        size_t linSizeEnt = propRef.lineSize;
        size_t offset;
        
        unsigned char* pbyPix = m_pbyBuffer;
        const int x = (physicsRef.x * m_Properties.components());
        int y = physicsRef.y;

        while (line < propRef.h && y < static_cast<int>(m_Properties.h - 1) && y >= 0)
        {
            y = (line + physicsRef.y);
            offset = ((y) *  m_Properties.lineSize) + x;
            std::memcpy(pbyPix + offset, data + (line * linSizeEnt), linSizeEnt);
            line++;
        }
    }
        
    void FrameBuffer::put(std::vector<Entity*> &v) const
    {
        std::for_each(v.begin(), v.end(), [&](Entity* it) { put(*it); });
    }

    void FrameBuffer::put(CharSet& c) const
    {
        std::vector<Entity*>* v = c.get();
        std::for_each(v->begin(), v->end(), [&](Entity* it) { put(*it); });
    }

    void FrameBuffer::fill(Entity& e, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        size_t y = 0;
        size_t offset;
        size_t lineSize = e.properties().lineSize;
        unsigned char comp = e.properties().components();

        unsigned char* pbyPix = e.data().get();

        std::unique_ptr<unsigned char[]> line = std::make_unique<unsigned char[]>(e.properties().lineSize);
        unsigned char* pbyLine = line.get();

        uint32_t pixel = buildPixel(r, g, b, a);

        for (uint32_t ud = 0; ud < lineSize; ud += comp)
            std::memcpy(&pbyLine[ud], &pixel, comp);

        while (y < e.properties().h)
        {
            offset = (y * lineSize);
            std::memcpy(pbyPix + offset, pbyLine, lineSize);
            y++;
        }

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
}//draw