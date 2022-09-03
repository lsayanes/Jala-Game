//leandro_say

#include <windows.h>

#include <stdint.h>

#include <Config.h>
#include <Types.h>
#include <Component.h>
#include <Physics.h>
#include <Properties.h>
#include <Device.h>


namespace draw
{

	typedef struct MEMBMP
	{
		HBITMAP             hHandle;
		BITMAPINFOHEADER	bmih;
		BITMAPINFO			dbmi;
		void* pBits;
	}MEMBMP;

	Device::Device(void* pDevHandle):
		m_DevHandle{ pDevHandle },
		m_DeviceContext{nullptr},
		m_bFullScreen{false},
		m_BackBufferHandle{nullptr},
		m_BackBuffer{nullptr},
		m_Properties{ components::Properties {0, 0, 0, 0} },
		m_stWidth{ 0 }, m_stHeight{0}
	{
		getVideoMode();
	}

	Device::~Device() 
	{
		retoreVideo();

		if (m_BackBufferHandle)
			DeleteObject(m_BackBufferHandle);

		if (m_BackBuffer)
			delete m_BackBuffer;
	}

	
	void *Device::beginPain() 
	{
		if(m_DevHandle)
			m_DeviceContext = GetDC(static_cast<HWND>(m_DevHandle));
		return m_DeviceContext;
	};

	void Device::endPaint() 
	{

		if (m_DevHandle && m_DeviceContext)
		{
			ReleaseDC(static_cast<HWND>(m_DevHandle), static_cast<HDC>(m_DeviceContext));
			m_DeviceContext = nullptr;
		}
	}

	bool Device::setVideoMode(
								size_t	stWidth,
								size_t	stHeight,
								unsigned char& byPixel,
								bool	bFullScreen)
	{

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
	}

	void Device::retoreVideo() const
	{
		ChangeDisplaySettings(NULL, 0);
	}

	bool Device::getVideoMode(
								size_t& stWidth,
								size_t& stHeight,
								unsigned char& byBitPixel
	)
	{

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

		if (!m_BackBufferHandle)
		{
			HBITMAP		hDev;
			
			//create a context device to the window handle
			m_BackBufferHandle = CreateCompatibleDC(GetDC(static_cast<HWND>(m_DevHandle)));
			
			/*
			* I created this temp bitmap just to select the device context with this measures
			*/
			hDev = CreateBitmap(
				stWidth,
				stHeight,
				unPlanes,
				static_cast<UINT>(byBitPerPixel),
				nullptr);

			if (m_BackBufferHandle)
			{
				//bind this temp bmp measures with the back context device handle
				SelectObject(static_cast<HDC>(m_BackBufferHandle), hDev);

				//this will be (on Windows) the back buffer
				size_t stRGBSize = stWidth * stHeight * (byBitPerPixel>>3);
				unsigned char*  pbPixels = new unsigned char[stRGBSize];	
				MEMBMP* pBitMap = new MEMBMP;

				for (size_t i = 0; i < stRGBSize; i++)
					pbPixels[i] = (i % 4 == 1) * 255;        // BGR

				pBitMap->bmih.biSize = sizeof(BITMAPINFOHEADER);
				pBitMap->bmih.biWidth = stWidth;
				pBitMap->bmih.biHeight = (-1) * stHeight;
				pBitMap->bmih.biPlanes = 1;
				pBitMap->bmih.biBitCount = byBitPerPixel;
				pBitMap->bmih.biCompression = BI_RGB;
				pBitMap->bmih.biSizeImage = 0;
				pBitMap->bmih.biXPelsPerMeter = 10;
				pBitMap->bmih.biYPelsPerMeter = 10;
				pBitMap->bmih.biClrUsed = 0;
				pBitMap->bmih.biClrImportant = 0;

				ZeroMemory(&pBitMap->dbmi, sizeof(pBitMap->dbmi));
				pBitMap->dbmi.bmiHeader = pBitMap->bmih;
				pBitMap->dbmi.bmiColors->rgbBlue = 0;
				pBitMap->dbmi.bmiColors->rgbGreen = 0;
				pBitMap->dbmi.bmiColors->rgbRed = 0;
				pBitMap->dbmi.bmiColors->rgbReserved = 0;
				pBitMap->pBits = (void*)&(pbPixels[0]);
					
				pBitMap->hHandle = CreateDIBSection(static_cast<HDC>(m_BackBufferHandle), &pBitMap->dbmi, DIB_RGB_COLORS, &pBitMap->pBits, nullptr, 0);
					
				m_BackBuffer = pBitMap;

				delete[] pbPixels;

				return static_cast<MEMBMP*>(m_BackBuffer)->pBits;

				}
		}

		return nullptr;

	}

	void *Device::create(size_t stWidth, size_t stHeight, unsigned char byBitPerPixel)
	{
		void* pRet{ nullptr };
		if (nullptr != (pRet = createBackbuffer(stWidth, stHeight, 1, byBitPerPixel)))
		{
			m_stWidth = stWidth;
			m_stHeight = stHeight;
			m_Properties.bpp(byBitPerPixel);
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
		if (m_DevHandle && !IsIconic(static_cast<HWND>(m_DevHandle)))
		{
			HDC hdc;
			//copy the fake buffer (BMP) on back context device
			HDC hMem = CreateCompatibleDC(0);
			MEMBMP* pBitMap = static_cast<MEMBMP*>(m_BackBuffer);
			SelectObject(hMem, pBitMap->hHandle);
			BitBlt(static_cast<HDC>(m_BackBufferHandle), 0, 0, m_stWidth, m_stHeight, hMem, 0, 0, SRCCOPY);
			DeleteDC(hMem);
			
			//copy the the back context device on primary context device windows
			hdc = static_cast<HDC>(beginPain());
			BitBlt(hdc, 0, 0, m_stWidth, m_stHeight, static_cast<HDC>(m_BackBufferHandle), 0, 0, SRCCOPY);
			endPaint();
		}
	};

}//draw