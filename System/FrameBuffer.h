#pragma once

namespace draw
{

	class FrameBuffer : public Device
	{
	public:
		explicit FrameBuffer(void *pWndHandle);
		virtual ~FrameBuffer();

		bool	isOk() const override;
	};


}//draw