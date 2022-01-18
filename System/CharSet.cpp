

#if defined(_WINDOWS)
#include <Windows.h>
#endif

#include <memory>
#include <vector>
#include <unordered_map>
#include <stdint.h>
#include <fstream>
#include <algorithm>


#include <ft2build.h>
#include <freetype/ftcolor.h>

#include FT_FREETYPE_H
#include FT_STROKER_H


#include "Raster.h"
#include "Entity.h"

#include "Sprite.h"
#include "Font.h"

#include "CharSet.h"

namespace draw
{

	CharSet::CharSet()
	{

	}

	CharSet::~CharSet()
	{

	}

	bool CharSet::create(const char* szFontPath, size_t stSize, uint8_t bpp)
	{
		if (Font::instance()->newFont(szFontPath, stSize))
		{
			FT_Face *face = (FT_Face*)Font::instance()->face();
			if (face)
			{
				const char* szText = "abcdefghijklmnñopqrstuvwxyzABCDEFGHIJKLMNÑOPQRSTUVWXYZ0123456789^[]{}()/&%$#?|\'¿¡!=\"";
				size_t	stLen = strlen(szText);
				FT_GlyphSlot  Slot = (*face)->glyph;
				int32_t dY = 100;
				char ch;
				for (size_t n = 0; n < stLen; n++)
				{
					FT_UInt  glyphIndex;
					ch = szText[n];
					glyphIndex = FT_Get_Char_Index(*face, ch);
					FT_Load_Glyph(*face, glyphIndex, FT_LOAD_DEFAULT);
					FT_Render_Glyph((*face)->glyph, FT_RENDER_MODE_NORMAL);
					int32_t dAdvance = (*face)->glyph->metrics.horiAdvance >> 6;
					uint32_t udSize = ((*face)->glyph->bitmap.width * (*face)->glyph->bitmap.rows);
					
					Entity* pEntity = new Entity{ (*face)->glyph->bitmap.width, (*face)->glyph->bitmap.rows, bpp };

					if (pEntity)
					{
						uint8_t* pData = pEntity->data.get();
						for (unsigned int i = 0; i < (*face)->glyph->bitmap.rows; i++)
						{
							for (unsigned int j = 0; j < (*face)->glyph->bitmap.width; j++)
								*(pData++) = (uint8_t)(~(*face)->glyph->bitmap.buffer[i * (*face)->glyph->bitmap.pitch + j]);
						}

						pEntity->nY = dY - Slot->bitmap_top;
						m_Chars[ch] = pEntity;
					}

					dY += Slot->advance.y >> 6;
				}
			}
		}

		return m_Chars.size()?true:false;
	}
	
	const std::vector<const Entity*>& CharSet::format(const char* szTxt)
	{
		
		size_t	stLen = strlen(szTxt);
		m_vctText.clear();
		m_vctText.resize(stLen);
		for (size_t n = 0; n < stLen; n++)
		{
			std::unordered_map<const char, const Entity*>::iterator it = m_Chars.find(szTxt[n]);
			if (it != m_Chars.end())
				m_vctText[n] = it->second;
		}

		return m_vctText;
	}

}//draw
