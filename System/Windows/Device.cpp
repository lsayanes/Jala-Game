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
		m_bFullScreen{false}
	{
	
	}

	Device::~Device() 
	{
		retoreVideo();
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
	};

	bool Device::setVideoMode(
								size_t	stWidth,
								size_t	stHeight,
								long	lPixel,
								bool	bFullScreen)
	{

		DEVMODE		Mode;
		DWORD		dwFlag = 0;

		memset(&Mode, 0, sizeof(DEVMODE));

		Mode.dmSize = sizeof(DEVMODE);
		Mode.dmPelsWidth = stWidth;
		Mode.dmPelsHeight = stHeight;
		Mode.dmBitsPerPel = lPixel;
		Mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (bFullScreen)
			dwFlag = CDS_FULLSCREEN;

		return (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&Mode, dwFlag));
	}

	void Device::retoreVideo() const
	{
		if (isOk()) 
			ChangeDisplaySettings(NULL, 0);
	}

	bool Device::getVideoMode(
								size_t& stWidth,
								size_t& stHeight,
								long& lBitPixel
	)
	{

		BOOL		bRet = false;
		DEVMODE		Mode;
		memset(&Mode, 0, sizeof(Mode));
		if (TRUE == (bRet = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &Mode))) 
		{
			stWidth = Mode.dmPelsWidth;
			stHeight = Mode.dmPelsHeight;
			lBitPixel = Mode.dmBitsPerPel;
		}

		return static_cast<bool>(bRet);

	}

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