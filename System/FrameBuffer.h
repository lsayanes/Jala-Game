#pragma once

namespace draw
{

	class FrameBuffer : public Device
	{
	private:
		void *m_BackBufferHandle;
	public:
		explicit FrameBuffer(void *pWndHandle);
		virtual ~FrameBuffer();

		bool	isOk() const override;

		bool create(size_t stWidth, size_t stlHeight, long lBitPerPixel);


	};


}//draw