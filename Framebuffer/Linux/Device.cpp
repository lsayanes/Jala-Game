//leandro_say

#include <iostream>
#include <stdint.h>
#include <cstddef>
#include <stdlib.h>



#include <X11/X.h>
#include <X11/keysym.h>

extern "C"
{
    #include <tinyptc.h>
}

#include "../Config.h"
#include "../Types.h"

#include "../Components/Component.h"
#include "../Components/Physics.h"
#include "../Components/Properties.h"




#include "../Device.h"


namespace draw
{

	Device::Device(void* pDevHandle):
		m_DevHandle{ pDevHandle },
		m_bFullScreen{false},
		m_BackBufferHandle{nullptr},
		m_BackBuffer{nullptr},
		width{ 0 }, height{0}
	{

		if (!getVideoMode())
			throw "Device::getVideoMode FAIL";
	}

	Device::~Device() 
	{

		ptc_close();

		retoreVideo();

		if (m_BackBuffer)
		{
			free(m_BackBuffer);
			m_BackBuffer = nullptr;
		}

	}

	
	void *Device::beginPain() 
	{
		return nullptr;
	};

	void Device::endPaint() 
	{

		if (m_DevHandle)
		{
		}
	}

	bool Device::setVideoMode(
								draw_t	w,
								draw_t	h,
								draw_t 	bitpp,
								bool	bFullScreen)
	{
		width = w;
		height = h;
		bpp = bitpp;
		
		return false;
	}

	void Device::retoreVideo() const
	{
	}

	
	bool Device::getVideoMode()
	{
		width = 1920;
		height = 1056;
		bpp = 32;

		return true;

	}
	
	void *Device::createBackbuffer(size_t stWidth, size_t stHeight, unsigned short unPlanes, unsigned char byBitPerPixel)
	{

		if (!m_BackBuffer)
		{
			if(NULL == (m_BackBuffer = malloc(stHeight * stWidth * byBitPerPixel)))
				std::cout << "m_BackBuffer malloc == NULL" << std::endl;	
			
		}

		return m_BackBuffer;
	}

	void *Device::create(draw_t w, draw_t h, draw_t bitPerPixel)
	{
		void* pRet{ nullptr };
		if (nullptr != (pRet = createBackbuffer(w, h, 1, bitPerPixel)))
		{
			width = w;
			height = h;

			ptc_open("...", width, height);
		}
		
		return pRet;
	}

	void Device::flip()
	{
		if(m_BackBuffer)
		{
			m_mtxSync.lock();
			ptc_update(m_BackBuffer);
			m_mtxSync.unlock();
		}
	};


	 bool Device::isRunning() 
	 {
		m_mtxSync.lock();
		int32_t ret = !ptc_process_events();
		m_mtxSync.unlock();
		return ret;
	 }
}//draw