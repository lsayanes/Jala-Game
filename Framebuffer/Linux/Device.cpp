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
	/*
	typedef struct MEMBMP
	{
		HBITMAP             hHandle;
		BITMAPINFOHEADER	bmih;
		BITMAPINFO			dbmi;
		void* pBits;
	}MEMBMP;
	*/

	Device::Device(void* pDevHandle):
		m_DevHandle{ pDevHandle },
		m_DeviceContext{nullptr},
		m_bFullScreen{false},
		m_BackBufferHandle{nullptr},
		m_BackBuffer{nullptr},
		m_Properties{ components::Properties {0, 0, 0, 0} },
		m_stWidth{ 0 }, m_stHeight{0}
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
		return m_DeviceContext;
	};

	void Device::endPaint() 
	{

		if (m_DevHandle && m_DeviceContext)
		{
			m_DeviceContext = nullptr;
		}
	}

	bool Device::setVideoMode(
								size_t	stWidth,
								size_t	stHeight,
								unsigned char& byPixel,
								bool	bFullScreen)
	{
		/*
		DEVMODE		Mode;
		DWORD		dwFlag = 0;

		memset(&Mode, 0, sizeof(DEVMODE));

		Mode.dmSize = sizeof(DEVMODE);
		Mode.dmPelsWidth = stWidth;
		Mode.dmPelsHeight = stHeight;
		Mode.dmBitsPerPel = static_cast<DWORD>(byPixel);
		Mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (bFullScreen)
			dwFlag = CDS_FULLSCREEN;

		return (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&Mode, dwFlag));
		*/

		return false;
	}

	void Device::retoreVideo() const
	{
		//ChangeDisplaySettings(NULL, 0);
	}

	bool Device::getVideoMode(
								size_t& stWidth,
								size_t& stHeight,
								unsigned char& byBitPixel
	)
	{
		/*
		BOOL		bRet = false;
		DEVMODE		Mode;
		memset(&Mode, 0, sizeof(Mode));
		if (TRUE == (bRet = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &Mode))) 
		{
			stWidth = Mode.dmPelsWidth;
			stHeight = Mode.dmPelsHeight;
			byBitPixel = static_cast<unsigned char>(Mode.dmBitsPerPel);
		}

		return static_cast<bool>(bRet);
		*/

		stWidth = 1920;
		stHeight = 1056;
		byBitPixel = 32;
		 

		return true;
	}

	bool Device::getVideoMode()
	{
		unsigned char b{0};

		bool bRet = Device::getVideoMode(m_stWidth, m_stHeight, b);
		m_Properties.bpp(b);

		return bRet;

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

	void *Device::create(size_t stWidth, size_t stHeight, unsigned char byBitPerPixel)
	{
		void* pRet{ nullptr };
		if (nullptr != (pRet = createBackbuffer(stWidth, stHeight, 1, byBitPerPixel)))
		{
			m_stWidth = stWidth;
			m_stHeight = stHeight;
			m_Properties.bpp(byBitPerPixel);

			ptc_open("...", m_stWidth, m_stHeight);
		}
		
		return pRet;
	}

	void *Device::create()
	{
		size_t w{0}, h{0};
		unsigned char b{0};
		
		void* pRet{ nullptr };

		if (Device::getVideoMode(w, h, b))
			pRet = create(w, h, b);

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


	 const int32_t Device::processEvent()
	 {
		m_mtxSync.lock();
		int32_t ret = ptc_process_events();
		m_mtxSync.unlock();
		
		return ret;
	 }

}//draw