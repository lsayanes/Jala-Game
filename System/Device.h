#pragma once

namespace draw
{

	class Device
	{
	protected:
		void		*m_WndHandle;
		void		 *m_DeviceContext;
		bool		m_bFullScreen;

	protected:
		explicit Device(void *pWndHandle);

		virtual bool	isOk() const = 0;
		virtual void	*beginPain();
		virtual void	endPaint();

		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;

		virtual ~Device();
	
	public:
		 bool setVideoMode(
			size_t	stWidth,
			size_t	stHeight,
			long	lPixel,
			bool	bFullScreen
		);

		void	retoreVideo() const;


#if defined(_WINDOWS)
		static HMODULE	getCurrentModule();
#endif

		inline const void *getHandle() const { return m_WndHandle; };


		static bool	getVideoMode(
			size_t& lWidth,
			size_t& lHeight,
			long& lBitPixel

		);

	};


}