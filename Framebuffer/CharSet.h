
#pragma once

namespace draw
{
	class CharSet 
	{
		std::vector<const Entity*> m_vctText{};
		std::unordered_map<char, Entity*> m_Chars{};
		uint8_t m_byBpp;
	
	public:
		explicit CharSet(uint8_t byBpp);
		virtual ~CharSet();

		const std::vector<const Entity*>& flatText(void* pFont, const char* sText, int x, int y);
		const std::vector<const Entity*> & format(const char *szTxt);

	};

}//draw