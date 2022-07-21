
#pragma once

namespace draw
{
	class CharSet 
	{
		std::vector<const Entity*> m_vctText{};
		void		*m_pFont;
		uint8_t		m_byBpp;
	
	private:
		inline void deleteVct()
		{
			std::for_each(m_vctText.begin(), m_vctText.end(),
				[&](auto it)
				{
					delete it;
				});

			m_vctText.clear();
		}
	public:
		explicit CharSet(void *pFont, uint8_t byBpp);
		virtual ~CharSet();
		
		CharSet(const CharSet&) = delete;
		CharSet(CharSet&&) = delete;
		CharSet& operator=(const CharSet&) = delete;
		CharSet& operator=(CharSet&&) = delete;
		
		const std::vector<const Entity*> *flatText(const char* sText, int x, int y);
		inline const std::vector<const Entity*>* get() { return &m_vctText; }
	};

}//draw