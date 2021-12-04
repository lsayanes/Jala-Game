#pragma once

namespace draw
{

	class FrameBuffer : public Device
	{
	private:
		size_t									m_stWidth;
		size_t									m_stHeight;
		const size_t							m_stComponents;
		std::unique_ptr<unsigned char[]>		m_BackBuffer;
		std::unique_ptr<unsigned char[]>		m_BackLine;


		static constexpr size_t						MaxComponents{ 4 };

	public:
		explicit FrameBuffer(size_t w, size_t h, void *pWndHandle);
		
		virtual ~FrameBuffer();

		bool	isOk() const override;

		void fill(unsigned long ulRgba);

		static unsigned long rgb(unsigned char r, unsigned char g, unsigned char b) 
		{ 
			return ((unsigned long)(((unsigned char)(r) | ((unsigned short)((unsigned char)(g)) << 8)) | (((unsigned long)(unsigned char)(b)) << 16))); 
		}

		static void pattern(
			unsigned char* pbBuff, size_t stBuffSize,
			unsigned char* pbPattern, size_t stPatterSize
		);

	};


}//draw