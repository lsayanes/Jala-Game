#pragma once

namespace draw
{

	class FrameBuffer : public Device
	{
	private:
		components::Properties<prop_type>		m_Properties;
		uint8_t									*m_pbyBuffer;

		std::unique_ptr<unsigned char[]>		m_Line;
	private:
		void fill(components::Properties<prop_type>& Prop, unsigned char* pbySurface, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	
	public:
		
		explicit FrameBuffer(prop_type w, prop_type h, uint8_t bits, void *pDevHandle);
	
		virtual ~FrameBuffer();

		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;

		bool	isOk() const override final;
		
		void	put(Entity& e) const;
		void	put(std::vector<Entity*> &v) const;
		void	put(CharSet& c) const;

		void	fill(Entity &e, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void	fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		inline const components::Properties<prop_type>& properties() const { return m_Properties; };

        inline void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            uint32_t pixel = buildPixel(r, g, b, a);
            unsigned char comp = m_Properties.components();
            std::memcpy(m_pbyBuffer + (std::abs(y) * m_Properties.lineSize) + (std::abs(x) * comp), &pixel, comp);
        }

        static uint32_t buildPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            return  ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint16_t)g << 8) | b;
        }

	};


}//draw