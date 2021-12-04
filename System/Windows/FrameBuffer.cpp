
#include <Windows.h>

#include "../Device.h"
#include "../FrameBuffer.h"

namespace draw
{

	FrameBuffer::FrameBuffer(void* pWndHandle) :
		Device{ pWndHandle }
	{

	}

	FrameBuffer::~FrameBuffer()
	{
	
	}


	bool FrameBuffer::isOk() const 
	{
		return m_WndHandle ? true : false;
	}

	

}//draw