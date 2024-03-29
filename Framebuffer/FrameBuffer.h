#pragma once

#include <cstring>

namespace draw
{
	class FrameBuffer
	{
	private:
		components::Properties			m_Properties;
		uint8_t							*m_pbyBuffer;

		std::unique_ptr<uint8_t[]>		m_Line;

	private:
		void fill(components::Properties& Prop, draw_t w, draw_t h, uint8_t* pbySurface, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

	public:

		explicit FrameBuffer(draw_t w, draw_t h);
		explicit FrameBuffer(uint8_t *pBuffer, draw_t w, draw_t h);

		virtual ~FrameBuffer();

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;


		void	put(Entity& e) const;
		void	put(std::vector<Entity*> &v) const;
		void	put(CharSet& c) const;

	    void	fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		static void	fill(Entity &e, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		[[nodiscard]]  inline const components::Properties& properties() const { return m_Properties; };

        inline void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            uint32_t pixel = buildPixel(r, g, b, a);
            unsigned char comp = m_Properties.components();
            std::memcpy(m_pbyBuffer + (std::abs(y) * m_Properties.lineSize) + (std::abs(x) * comp), &pixel, comp);
        }

		[[nodiscard]]  static uint32_t buildPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            return  ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint16_t)g << 8) | b;
        }

	};
}//draw