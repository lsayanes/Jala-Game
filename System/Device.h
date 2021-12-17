#pragma once

namespace draw
{

	class Device
	{
	protected:
		void				*m_WndHandle;
		void				*m_DeviceContext;
		bool				m_bFullScreen;

		void				*m_BackBufferHandle;
		void				*m_BackBuffer;

		size_t				m_stWidth;
		size_t				m_stHeight;
		unsigned char		m_byBitPerPixel;


	protected:
		
		explicit Device(void* pWndHandle);
		
		void *createBackbuffer(size_t stWidth, size_t stHeight, unsigned short unPlanes, unsigned char byBitPerPixel);


		virtual bool	isOk() const = 0;
		virtual void	*beginPain();
		virtual void	endPaint();

		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;

		virtual ~Device();
	
		 bool setVideoMode(
			size_t	stWidth,
			size_t	stHeight,
			 unsigned char& byPixel,
			bool	bFullScreen
		);

		void	retoreVideo() const;

		bool getVideoMode();
		void *create(size_t stWidth, size_t stlHeight, unsigned char byBitPerPixel);
	
	public:

		void flip();

		inline const void *getHandle() const { return m_WndHandle; };

		static bool	getVideoMode(
			size_t& lWidth,
			size_t& lHeight,
			unsigned char& byBitPixel

		);

	};


}