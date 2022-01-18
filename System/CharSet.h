
#pragma once

namespace draw
{
	class CharSet 
	{
		std::vector<const Entity*> m_vctText{};
		std::unordered_map<char, const Entity*> m_Chars{};
	
	public:
		explicit CharSet();
		virtual ~CharSet();

		bool create(const char * szFontPath, size_t stSize, uint8_t bpp);
		
		const std::vector<const Entity*> & format(const char *szTxt);

	};

}//draw