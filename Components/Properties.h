#pragma once

namespace draw
{
	namespace components
	{
		//using prop_selected_t = unsigned short int;
		template <typename TYPE>
		struct Properties : Component
		{
			TYPE w{ 0 };
			TYPE h{ 0 };
			TYPE size{ 0 };
			TYPE lineSize{ 0 };

			explicit Properties(TYPE w_t, TYPE h_t, unsigned char b_t, size_t eID) :
				Component{eID},
				w{w_t},
				h{h_t}
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