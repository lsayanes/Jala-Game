//leandro_say

#pragma warning(disable:4244)

#include <windows.h>

#include <stdint.h>
#include <iostream>

#include <Config.h>
#include <Types.h>
#include <Component.h>
#include <Physics.h>
#include <Properties.h>
#include <Device.h>

namespace draw
{

	struct FunctionClass
	{

		static Device *pDev;

		FunctionClass() = default;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_KEYDOWN:
				pDev->onKeyDown(static_cast<WPARAM>(wParam));
				break;
			case WM_COMMAND:
				break;
			case WM_DESTROY:
				pDev->onClose();
				PostQuitMessage(0);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			return FALSE;
		}
	}WindProcClass;

	Device* FunctionClass::pDev{ nullptr };

	typedef struct MEMBMP
	{
		HBITMAP             hHandle;
		BITMAPINFOHEADER	bmih;
		BITMAPINFO			dbmi;
		void* pBits;
	}MEMBMP;

	Device::Device(void* pParam):
		m_SurfaceDev{nullptr},
		m_Instance{pParam},
		m_bFullScreen{false},
		m_BackBufferHandle{nullptr},
		m_BackBuffer{nullptr},
		width{0}, height{0}
	{
		if (!getVideoMode())
			throw "Device::getVideoMode FAIL";
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
			m_SurfaceDev = GetDC(static_cast<HWND>(m_DevHandle));
		return m_SurfaceDev;
	};

	void Device::endPaint() 
	{

		if (m_DevHandle && m_SurfaceDev)
		{
			ReleaseDC(static_cast<HWND>(m_DevHandle), static_cast<HDC>(m_SurfaceDev));
			m_SurfaceDev = nullptr;
		}
	}

	bool Device::setVideoMode(
								draw_t	stWidth,
								draw_t	stHeight,
								bool	bFullScreen)
	{

		DEVMODE		Mode;
		DWORD		dwFlag = 0;

		memset(&Mode, 0, sizeof(DEVMODE));

		Mode.dmSize = sizeof(DEVMODE);
		Mode.dmPelsWidth = static_cast<DWORD>(stWidth);
		Mode.dmPelsHeight = static_cast<DWORD>(stHeight);
		Mode.dmBitsPerPel = static_cast<DWORD>(bpp);
		Mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (bFullScreen)
			dwFlag = CDS_FULLSCREEN;

		return (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettings(&Mode, dwFlag));
	}

	void Device::retoreVideo() const
	{
		ChangeDisplaySettings(NULL, 0);
	}


	bool Device::getVideoMode()
	{
		unsigned char b{0};


		BOOL		bRet = false;
		DEVMODE		Mode;
		memset(&Mode, 0, sizeof(Mode));
		if (TRUE == (bRet = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &Mode)))
		{
			width = static_cast<draw_t>(Mode.dmPelsWidth);
			height = static_cast<draw_t>(Mode.dmPelsHeight);
			//bpp = static_cast<draw_t>(Mode.dmBitsPerPel);
			bRet = true;
		}


		return bRet;

	}
	
	void *Device::createBackbuffer(draw_t w, draw_t h, draw_t bitPerPixel)
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
				static_cast<int>(w),
				static_cast<int>(h),
				1,
				static_cast<UINT>(bitPerPixel),
				nullptr);

			if (m_BackBufferHandle)
			{
				//bind this temp bmp measures with the back context device handle
				SelectObject(static_cast<HDC>(m_BackBufferHandle), hDev);

				//this will be (on Windows) the back buffer
				size_t stRGBSize = w * h * (bitPerPixel >>3);
				unsigned char*  pbPixels = new unsigned char[stRGBSize];	
				MEMBMP* pBitMap = new MEMBMP;

				for (size_t i = 0; i < stRGBSize; i++)
					pbPixels[i] = (i % 4 == 1) * 255;        // BGR

				pBitMap->bmih.biSize = sizeof(BITMAPINFOHEADER);
				pBitMap->bmih.biWidth = w;
				pBitMap->bmih.biHeight = static_cast<LONG>((-1) * h);
				pBitMap->bmih.biPlanes = 1;
				pBitMap->bmih.biBitCount = bitPerPixel;
				pBitMap->bmih.biBitCount = bitPerPixel;
				pBitMap->bmih.biBitCount = bitPerPixel;
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

	void *Device::create(draw_t w, draw_t h)
	{
		void* pRet{ nullptr };
		if (nullptr != (pRet = createBackbuffer(w, h, bpp)))
		{
			width = w;
			height = h;


			WNDCLASSEX wcex;

			WindProcClass.pDev = this;

			wcex.cbSize = sizeof(WNDCLASSEX);

			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WindProcClass.WndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = (HINSTANCE)m_Instance;
			wcex.hIcon = NULL;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
			wcex.lpszMenuName = NULL;
			wcex.lpszClassName = "JalaGameDeviceClass";
			wcex.hIconSm = NULL;

			if (RegisterClassEx(&wcex))
			{

				HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "JalaGameDeviceClass", "...", WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT,  CW_USEDEFAULT, width,  height + 50, nullptr, nullptr, (HINSTANCE)m_Instance, nullptr);

				if (!hWnd)
					return nullptr;

				m_DevHandle = hWnd;
				m_bRunning = true;

				ShowWindow(hWnd, SW_SHOW);
				UpdateWindow(hWnd);


			}
		}
		
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
			BitBlt(static_cast<HDC>(m_BackBufferHandle), 0, 0, static_cast<int>(width), static_cast<int>(height), hMem, 0, 0, SRCCOPY);
			DeleteDC(hMem);
			
			//copy the the back context device on primary context device windows
			hdc = static_cast<HDC>(beginPain());
			BitBlt(hdc, 0, 0, width, height, static_cast<HDC>(m_BackBufferHandle), 0, 0, SRCCOPY);
			endPaint();
		}
	};

	bool Device::isRunning()
	{
		MSG msg;
		HWND hWnd = static_cast<HWND>(m_DevHandle);

		if(PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE))
		//if(GetMessage(&msg, hWnd, 0, 0) > 0)
		{
			if (
					WM_QUIT == msg.message ||
					WM_DESTROY == msg.message ||
					WM_CLOSE == msg.message
				)
			{
				return false;

			}
			else
			{
				if (GetMessage(&msg, hWnd, 0, 0) > 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		return m_bRunning;
				
	}

}//draw
