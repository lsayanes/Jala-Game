

#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <stdint.h>

#include "Device.h"
#include "Raster.h"
#include "Entity.h"
#include "FrameBuffer.h"

namespace draw
{

	FrameBuffer::FrameBuffer(size_t w, size_t h, uint8_t bits, void* pWndHandle) :
        Raster{ w, h, bits },
        Device{ pWndHandle }
    {

        Raster::pbyBuffer = static_cast<unsigned char*>(Device::create(m_stWidth, m_stHeight, bits));
    }

	FrameBuffer::~FrameBuffer()
	{
	
	}

	bool FrameBuffer::isOk() const 
	{
		return m_WndHandle && Raster::pbyBuffer ? true : false;
	}

    void FrameBuffer::put(const Entity& e)
    {
        
        size_t line = 0;
        size_t offset;
        
        unsigned char* pbyPix = Raster::pbyBuffer;
        const int x = (e.nX * FrameBuffer::MaxComponents);
        const size_t stLimitH = (m_stHeight - e.stH) - 1;

        size_t y = e.nY;

        while (line < e.stH && y < stLimitH)
        {
            y = (line + e.nY);
            offset = ((y) *  stSizeLine) + x;
            std::memcpy(pbyPix + offset, e.data.get() + (line * e.stSizeLine), e.stSizeLine);
            line++;
        }

    }


}//draw