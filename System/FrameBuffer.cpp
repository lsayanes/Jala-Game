

#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>

#include "Device.h"
#include "FrameBuffer.h"

namespace draw
{

	FrameBuffer::FrameBuffer(size_t w, size_t h, void* pWndHandle) :
		Device{ pWndHandle },
		m_stWidth{ w },
		m_stHeight{ h },
        m_stComponents { static_cast<size_t>(Device::m_byBitPerPixel >> 3) },
		m_BackBuffer{ std::make_unique<unsigned char[]>((m_stWidth * m_stComponents) * m_stHeight) },
        m_BackLine{ std::make_unique<unsigned char[]> (m_stWidth * m_stComponents) }
	{
		if (!create(m_stWidth, m_stHeight, Device::m_byBitPerPixel, m_BackBuffer.get()))
			m_BackBuffer = nullptr;
	}

	FrameBuffer::~FrameBuffer()
	{
	
	}


	bool FrameBuffer::isOk() const 
	{
		return m_WndHandle && m_BackBuffer.get() ? true : false;
	}

    
    void FrameBuffer::fill(unsigned long ulRgba)
    {

        unsigned char* pbyPix = m_BackBuffer.get();
        unsigned char* pbyLine = m_BackLine.get();
        /*
        unsigned char byPttn[FrameBuffer::MaxComponents]  = {
            (unsigned char)(ulRgba),
            (unsigned char)((unsigned int)(ulRgba) >> 8),
            (unsigned char)((unsigned int)(ulRgba) >> 16),
            (unsigned char)(ulRgba >> 24),
        };
          */


        unsigned char byPttn[FrameBuffer::MaxComponents] = { 255, 0, 0,0 };
        pattern(pbyLine, m_stWidth, byPttn, 3);

        size_t y = 0;
        size_t offset;
        while (y < m_stHeight)
        {
            offset = 3 * (y * m_stWidth);
            memcpy(pbyPix + offset, pbyLine, m_stWidth);
            y++;
        }

	}

    void FrameBuffer::pattern(
        unsigned char* pbBuff, size_t stBuffSize,
        unsigned char* pbPattern, size_t stPatterSize
    )
    {
        for (uint32_t ud = 0; ud < stBuffSize; ud += stPatterSize)
            memcpy(&pbBuff[ud], pbPattern, stPatterSize);
    }


}//draw