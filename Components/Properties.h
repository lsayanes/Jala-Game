#pragma once

#include <stdio.h>
#include <stdint.h>
#include <cstring>

namespace draw
{
	namespace components
	{
		enum ALLOC_TYPE_COMPONENT { ATC_BUFFER_DEV, ATC_MALLOC_BUFFER, ATC_NONE };

		struct Properties
		{
			protected:
				ALLOC_TYPE_COMPONENT	m_allocType{ ATC_NONE };

			private:
				uint8_t 	m_components{0}; // bpp/8
				char 		*m_szName {nullptr};


			public:
				size_t			size{0};
				size_t			lineSize{0};
				uint8_t			alpha{0}; //if it have alpha channel
				draw_t			width{0};
				draw_t			height{0};

				static constexpr uint8_t bpp{32};

				explicit Properties(draw_t w, draw_t h, ALLOC_TYPE_COMPONENT aType, const char *szName):
					m_allocType{aType},
					m_szName{ szName?new char[strlen(szName) + 1]: nullptr }
				{

					if(m_szName)
						strcpy(m_szName, szName);

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

					if(m_szName)
						delete[] m_szName;

					m_szName = nullptr;

				}

				inline const uint8_t components() const { return m_components; };
				inline const ALLOC_TYPE_COMPONENT allocType() const { return m_allocType; }

				[[nodiscard]] const char *infoDebug()
				{
					static char ret[256];

					sprintf(ret, "Properties component name:%s, w:%d h:%d bpp:%d, comp:%u size:%lu, lineSize:%lu alpha:%d",
						m_szName, width, height, Properties::bpp, m_components,
						static_cast<unsigned long>(size), static_cast<unsigned long>(lineSize),
						alpha);

					return ret;
				}
		};
	}//components
} //draw