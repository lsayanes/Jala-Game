
#include <Windows.h>

#include "../Device.h"
#include "../FrameBuffer.h"

namespace draw
{

	FrameBuffer::FrameBuffer(void* pWndHandle) :
		Device{ pWndHandle },
		m_BackBufferHandle{nullptr}
	{

	}

	FrameBuffer::~FrameBuffer()
	{
		if(m_BackBufferHandle)
			DeleteObject(m_BackBufferHandle);
	}


	bool FrameBuffer::isOk() const 
	{
		return m_WndHandle ? true : false;
	}

	bool FrameBuffer::create(size_t stWidth, size_t stlHeight, long lBitPerPixel)
	{
		HBITMAP		hDev;
		

		if (!m_BackBufferHandle)
		{
			m_BackBufferHandle = CreateCompatibleDC(GetDC(static_cast<HWND>(m_WndHandle)));
			hDev = CreateBitmap(
				stWidth,
				stlHeight,
				lBitPerPixel,
				1,
				NULL);

			SelectObject(static_cast<HDC>(m_BackBufferHandle), hDev);

			return true;
		}

		return false;

	}


}//draw