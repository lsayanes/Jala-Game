#pragma once

namespace draw
{

	class Device
	{
	protected:
		void				*m_DevHandle;
		void				*m_DeviceContext;
		bool				m_bFullScreen;

		void				*m_BackBufferHandle;
		void				*m_BackBuffer;

		components::Properties<size_t> m_Properties;

	protected:
		
		explicit Device(void* pDevHandle);
		
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
		void* create();
	
	public:

		void flip();

		inline const void *getHandle() const { return m_DevHandle; };
		inline const components::Properties<size_t> &properties() const { return m_Properties; }

		static bool	getVideoMode(
			size_t& lWidth,
			size_t& lHeight,
			unsigned char& byBitPixel

		);

	};


}