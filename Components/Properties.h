#pragma once

#include <stdio.h>
#include <stdint.h>

namespace draw
{
	namespace components
	{
		struct Properties : Component
		{
			private:
				uint8_t m_components{0}; // bpp/8

			public:

			size_t			size{0};
			size_t			lineSize{0};
			uint8_t			alpha{0}; //if it have alpha channel
			draw_t			width{0};
			draw_t			height{0};

			static constexpr uint8_t bpp{32};

			explicit Properties(draw_t w, draw_t h, TYPE_COMPONENT eID) : Component{eID}
			{
				m_components = static_cast<const uint8_t>(Properties::bpp >> 3);

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

			inline const uint8_t components() const { return m_components; };

			[[nodiscard]] const char *infoDebug()
			{
				static char ret[256];

				sprintf(ret, "Properties component id:%d, w:%d h:%d bpp:%d, comp:%d size:%lu, lineSize:%lu alpha:%d",
					componentID(), width, height, Properties::bpp, m_components,
					static_cast<unsigned long>(size), static_cast<unsigned long>(lineSize),
					alpha);

				return ret;
			}
		};
	}//components
} //draw