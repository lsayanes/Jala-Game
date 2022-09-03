#pragma once

namespace draw
{
	namespace components
	{
		struct Properties : Component
		{
			size_t size{ 0 };
			size_t lineSize{ 0 };
			unsigned char alpha{ 0 }; //if it have alpha channel

			explicit Properties(draw_t w, draw_t h, unsigned char b_t, size_t eID) : Component{eID}
			{
				bpp(b_t);
				size = (w * (m_components)) * h;
				lineSize = w * m_components;
			}

			Properties(const Properties&) = delete;
			Properties(Properties&&) = delete;
			Properties& operator=(const Properties&) = delete;
			Properties& operator=(Properties&&) = delete;

			virtual ~Properties()
			{
			}


			inline void bpp(unsigned char b)
			{
				m_bpp = b;
				m_components = static_cast<const unsigned char>(b >> 3);
			}

			inline const unsigned char bpp() const { return m_bpp; };
			inline const unsigned char components() const { return m_components; };

		private:
			unsigned char	m_bpp{ 0 };
			unsigned char	m_components{ 0 }; // bpp/8

		};
	}//components
} //draw