#pragma once

#include <stdio.h>

namespace draw
{
	namespace components
	{
		struct Properties : Component
		{
			size_t			size{0};
			size_t			lineSize{0};
			uint8_t			alpha{0}; //if it have alpha channel
			draw_t			width{0};
			draw_t			height{0};

			explicit Properties(draw_t w, draw_t h, uint8_t b_t, TYPE_COMPONENT eID) : Component{eID}
			{
				bpp(b_t);
				size = (w * m_components) * h;
				lineSize = w * m_components;

				width = w;
				height = h;
			}

			Properties(const Properties&) = delete;
			Properties(Properties&&) = delete;
			Properties& operator=(const Properties&) = delete;
			Properties& operator=(Properties&&) = delete;

			virtual ~Properties()
			{
			}


			inline void bpp(uint8_t b)
			{
				m_bpp = b;
				m_components = static_cast<const uint8_t>(b >> 3);
			}

			inline const uint8_t bpp() const { return m_bpp; };
			inline const uint8_t components() const { return m_components; };

			[[nodiscard]] const char *infoDebug()
			{
				static char ret[256];

				sprintf(ret, "Properties component id:%d, w:%d h:%d bpp:%d, comp:%d size:%lu, lineSize:%lu alpha:%d", 
					componentID(), width, height, m_bpp, m_components, size, lineSize, alpha);

				return ret;
			}

		private:
			uint8_t	m_bpp{ 0 };
			uint8_t m_components{ 0 }; // bpp/8

		};
	}//components
} //draw