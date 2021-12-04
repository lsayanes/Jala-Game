//leandro_say

#include <windows.h>
#include "../Device.h"

#define WHANDLE		static_cast<HWND>(m_WndHandle)
#define WDEVCON		static_cast<HDC>(m_DeviceContext) 

namespace draw
{

	Device::Device(void* pWndHandle):
		m_WndHandle{ pWndHandle },
		m_DeviceContext{nullptr},
		m_bFullScreen{false},
		m_BackBufferHandle{nullptr},
		m_stWidth{ 0 },
		m_stHeight{ 0 },
		m_byBitPerPixel{ 0 }
	{
		getVideoMode();
	}

	Device::~Device() 
	{
		retoreVideo();

		if (m_BackBufferHandle)
			DeleteObject(m_BackBufferHandle);
	}

	
	void *Device::beginPain() 
	{
		if(m_WndHandle)
			m_DeviceContext = GetDC(WHANDLE);
		return m_DeviceContext;
	};

	void Device::endPaint() 
	{

		if (m_WndHandle && WDEVCON)
		{
			ReleaseDC(WHANDLE, WDEVCON);
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
		return Device::getVideoMode(m_stWidth, m_stHeight, m_byBitPerPixel);
	}
	
	bool Device::createBackbuffer(size_t stWidth, size_t stlHeight, unsigned short unPlanes, unsigned char byBitPerPixel, unsigned char* pbyBuff)
	{
		HBITMAP		hDev;


		if (!m_BackBufferHandle)
		{
			m_BackBufferHandle = CreateCompatibleDC(GetDC(static_cast<HWND>(m_WndHandle)));
			hDev = CreateBitmap(
				stWidth,
				stlHeight,
				unPlanes,
				static_cast<UINT>(byBitPerPixel),
				pbyBuff);

			if (m_BackBufferHandle)
			{
				SelectObject(static_cast<HDC>(m_BackBufferHandle), hDev);
				return true;
			}
		}

		return false;

	}

	bool Device::create(size_t stWidth, size_t stHeight, unsigned char byBitPerPixel, unsigned char* pbyBuff)
	{
		if (createBackbuffer(stWidth, stHeight, 1, byBitPerPixel, pbyBuff))
		{
			m_stWidth = stWidth;
			m_stHeight = stHeight;
			m_byBitPerPixel = byBitPerPixel;

			return true;
		}

		return false;
	}

	void Device::setSystemText(int x, int y, const char* sz) const
	{
		TextOut(static_cast<HDC>(m_BackBufferHandle), x, y, sz, strlen(sz));
	}

	void Device::flip()
	{

		HDC hdc;

		if (WHANDLE && !IsIconic(WHANDLE))
		{
			hdc = static_cast<HDC>(beginPain());
			BitBlt(hdc, 0, 0, m_stWidth, m_stHeight, static_cast<HDC>(m_BackBufferHandle), 0, 0, SRCCOPY);
			endPaint();
		}

	};


	/*
		https://msdn.microsoft.com/en-us/library/windows/desktop/ms683200(v=vs.85).aspx
	*/

	HMODULE Device::getCurrentModule()
	{
		HMODULE hModule = NULL;
		GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			(LPCTSTR)getCurrentModule,
			&hModule);

		return hModule;
	}


}//draw