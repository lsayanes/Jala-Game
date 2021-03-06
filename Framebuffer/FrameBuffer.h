#pragma once

namespace draw
{

	class FrameBuffer : public Raster, public Device
	{
	public:
		explicit FrameBuffer(size_t w, size_t h, uint8_t bits, void *pWndHandle);
	
		virtual ~FrameBuffer();

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;

		bool	isOk() const override final;
		
		void	put(const Entity& e) const;
		void	put(const std::vector<const Entity*> &v) const;
		void	put(CharSet& c) const;
	};


}//draw