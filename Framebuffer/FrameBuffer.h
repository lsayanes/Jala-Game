#pragma once

namespace draw
{

	class FrameBuffer : public Raster, public Device
	{
	public:
		explicit FrameBuffer(size_t w, size_t h, uint8_t bits, void *pWndHandle);
		
		virtual ~FrameBuffer();

		bool	isOk() const override;
		void	put(const Entity& e) const;
		void	put(const std::vector<const Entity*> &v) const;
	};


}//draw