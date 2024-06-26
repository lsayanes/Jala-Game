
#pragma once

namespace draw
{
	class CharSet
	{
		Memory<uint64_t>		m_mem;
		std::vector<Entity*> m_vctText{};
		void		*m_pFont;
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