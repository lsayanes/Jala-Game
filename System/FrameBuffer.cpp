

#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <stdint.h>

#include "Device.h"

#include "Entity.h"
#include "FrameBuffer.h"

namespace draw
{

	FrameBuffer::FrameBuffer(size_t w, size_t h, void* pWndHandle) :
		Device{ pWndHandle },
		m_stWidth{ w },
		m_stHeight{ h },
        m_stComponents { static_cast<size_t>(Device::m_byBitPerPixel >> 3) },
        m_pbyBuffer{nullptr},
        m_Line{ std::make_unique<unsigned char[]> (m_stWidth * m_stComponents) },
        m_stSizeLine{ m_stWidth * FrameBuffer::MaxComponents }
	{
        m_pbyBuffer = static_cast<unsigned char*>(create(m_stWidth, m_stHeight, Device::m_byBitPerPixel));
	}

	FrameBuffer::~FrameBuffer()
	{
	
	}


	bool FrameBuffer::isOk() const 
	{
		return m_WndHandle && m_pbyBuffer ? true : false;
	}

    void FrameBuffer::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        unsigned char byPixel[FrameBuffer::MaxComponents] = { b, g, r };

        if (FrameBuffer::MaxComponents > 3)
            byPixel[3] = a;

        std::memcpy(m_pbyBuffer + (y * m_stSizeLine) + (x * FrameBuffer::MaxComponents), byPixel, FrameBuffer::MaxComponents);
    }
    
    void FrameBuffer::fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        size_t y = 0;
        size_t offset;

        unsigned char* pbyPix = m_pbyBuffer;
        unsigned char* pbyLine = m_Line.get();
        
        unsigned char byPttn[FrameBuffer::MaxComponents]  = {b, g, r};

        if (FrameBuffer::MaxComponents > 3)
            byPttn[3] = a;
          
        pattern(pbyLine, m_stSizeLine, byPttn, FrameBuffer::MaxComponents);

        while (y < m_stHeight)
        {
            offset = (y * m_stSizeLine);
            std::memcpy(pbyPix + offset, pbyLine, m_stSizeLine);
            y++;
        }
	}

    void FrameBuffer::put(const Entity& e)
    {
        
        const size_t sizeLine { e.lineWidth() };

        size_t line = 0;
        size_t offset;
        
        unsigned char* pbyPix = m_pbyBuffer;
        const int x = (e.nX * FrameBuffer::MaxComponents);
        const size_t stLimitH = (m_stHeight - e.stH) - 1;

        size_t y = e.nY;

        while (line < e.stH && y < stLimitH)
        {
            y = (line + e.nY);
            offset = ((y) * m_stSizeLine) + x;
            std::memcpy(pbyPix + offset, e.data.get() + (line * e.stW), sizeLine);
            line++;
        }

    }

    void FrameBuffer::pattern(
        unsigned char* pbBuff, size_t stBuffSize,
        unsigned char* pbPattern, size_t stPatterSize
    )
    {
        for (uint32_t ud = 0; ud < stBuffSize; ud += stPatterSize)
            std::memcpy(&pbBuff[ud], pbPattern, stPatterSize);
    }


}//draw