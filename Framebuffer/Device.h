#pragma once

#include <mutex>

namespace draw
{
	class Device
	{
	private:
		void				*m_DevHandle;	//window handle
		void				*m_Instance;	//just for Windows System
	protected:
		
		void				*m_SurfaceDev; //surface of client area of a specified window
		bool				m_bFullScreen;

		//Back buffer
		void				*m_BackBufferHandle;	//file handle
		void				*m_BackBuffer;			//buffer	

		const char 			*m_szName;


		std::mutex			m_mtxSync{};

		bool				m_bRunning;

	public:
		draw_t				width;
		draw_t				height;
		uint8_t				bpp;
	protected:
		
	
		[[nodiscard]] void *createBackbuffer(size_t stWidth, size_t stHeight, unsigned short unPlanes, unsigned char byBitPerPixel);


		virtual void	*beginPain();
		virtual void	endPaint();

		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;

		void	retoreVideo() const;

	public:

		explicit Device(void *pParam = nullptr);
		virtual ~Device();

		virtual void flip();

		[[nodiscard]] virtual void* create(draw_t w, draw_t h, draw_t bitPerPixel);
		[[nodiscard]] virtual bool isRunning() ;
		[[nodiscard]] virtual bool setVideoMode(
			draw_t	width,
			draw_t	height,
			draw_t 	bpp,
			bool	bFullScreen
		);

		[[nodiscard]] inline bool isCreated() { return m_DevHandle?true:false; } 
		[[nodiscard]] virtual bool getVideoMode();

		virtual void onClose() = 0;
		virtual void onKeyDown(int nKey) = 0;
		
	};

}//draw