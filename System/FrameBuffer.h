#pragma once

namespace draw
{

	class FrameBuffer : public Device
	{
	private:
		size_t									m_stWidth;
		size_t									m_stHeight;
		const size_t							m_stComponents;
		unsigned char							*m_pbyBuffer;
		std::unique_ptr<unsigned char[]>		m_Line;
		const size_t							m_stSizeLine;



		static constexpr size_t						MaxComponents{ 4 };

	public:
		explicit FrameBuffer(size_t w, size_t h, void *pWndHandle);
		
		virtual ~FrameBuffer();

		bool	isOk() const override;
		
		void	pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0);
		void	fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0);

		void put(const Entity& e);

		static void pattern(
			unsigned char* pbBuff, size_t stBuffSize,
			unsigned char* pbPattern, size_t stPatterSize
		);

	};


}//draw