
#pragma once

namespace draw
{
	class CharSet 
	{
		std::vector<Entity*> m_vctText{};
		void		*m_pFont;
	
	private:
		/*
		inline void deleteVct()
		{
			std::for_each(m_vctText.begin(), m_vctText.end(),
				[&](auto it)
				{
					delete it;
				});

			m_vctText.clear();
		}
		*/
	public:
		explicit CharSet(void *pFont);
		virtual ~CharSet();

		CharSet(const CharSet&) = delete;
		CharSet(CharSet&&) = delete;
		CharSet& operator=(const CharSet&) = delete;
		CharSet& operator=(CharSet&&) = delete;

		[[nodiscard]] std::vector<Entity*> *flatText(const char* sText, int x, int y);
		[[nodiscard]] inline std::vector<Entity*>* get() { return &m_vctText; }
	};

}//draw