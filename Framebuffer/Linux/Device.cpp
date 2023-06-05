//leandro_say

#include <iostream>
#include <stdint.h>
#include <cstddef>
#include <stdlib.h>



#include <X11/X.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrandr.h>

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


	struct CallbackDeviceClass 
	{
		static Device *pCallbackInstance;

		
		static void keyPress(unsigned long lParam)
		{
			pCallbackInstance->onKeyDown(lParam);
		}
	};
	
	Device *CallbackDeviceClass::pCallbackInstance{nullptr}; 


	Device::Device(void* pDevHandle):
		m_DevHandle{ pDevHandle },
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
								bool	bFullScreen)
	{
		width = w;
		height = h;

		return false;
	}

	void Device::retoreVideo() const
	{
	}

	
	bool Device::getVideoMode(draw_t &w, draw_t &h, draw_t &b)
	{

    	Display *display = XOpenDisplay(NULL);
		if(display)
		{
			Window root = DefaultRootWindow(display);

			XRRScreenResources *screen_resources = XRRGetScreenResources(display, root);
			XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, screen_resources, screen_resources->crtcs[0]);
		
			w = static_cast<draw_t>(crtc_info->width);
			h = static_cast<draw_t>(crtc_info->height);
			
			b = static_cast<draw_t>(DefaultDepth(display, DefaultScreen(display)));
		
			std::cout << "DEBUG:Device::getVideoMode : " << __LINE__ << " Video Resolution: " << w << "x" << h << ":" << static_cast<int>(b) << std::endl;
		
			XRRFreeCrtcInfo(crtc_info);
			XRRFreeScreenResources(screen_resources);

			XCloseDisplay(display);

			return true;
		}

		return false;
	}

	bool Device::getVideoMode()
	{
		draw_t bpp {0};
		return Device::getVideoMode(width, height, bpp);
	}
	
	void *Device::createBackbuffer(draw_t w, draw_t h, draw_t bitPerPixel, draw_t planes)
	{

		if (!m_BackBuffer)
		{
			if(NULL == (m_BackBuffer = malloc(w * h * bpp)))
				std::cout << "m_BackBuffer malloc == NULL" << std::endl;	
			
		}

		return m_BackBuffer;
	}

	void *Device::create(draw_t w, draw_t h)
	{
		void* pRet{ nullptr };

		if (nullptr != (pRet = createBackbuffer(w, w, Device::bpp, 1)))
		{
			width = w;
			height = h;

			CallbackDeviceClass::pCallbackInstance = this;

			ptc_open("...", w, h);
		  	
			ptc_set_on_keypress(&CallbackDeviceClass::keyPress);
    		//ptc_set_on_keyrelease(onKeyRelease);	

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

/*

void onKeyPress(KeySym  k)
{
    if(XK_Escape == k)
    {
        bRun = false;
    }
  
}

void onKeyRelease(KeySym k)
{
    if(XK_Escape)
    {

    }
}

int main()
{

    ptc_open("windows", WIDTH, HEIGHT);
*/
}//draw