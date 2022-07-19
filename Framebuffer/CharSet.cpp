

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
#include "FontLib.h"

#include "CharSet.h"

namespace draw
{

	CharSet::CharSet(uint8_t byBpp):
		m_byBpp{byBpp}
	{

	}

	CharSet::~CharSet()
	{
		std::for_each(m_Chars.begin(), m_Chars.end(), 
			[&](auto it) 
			{  
				delete it.second;
				it.second = nullptr;
			});
		
		m_Chars.clear();
	}

	const std::vector<const Entity*>& CharSet::flatText(void *pFont, const char* sText, int x, int y)
	{
		FT_Face face = static_cast<FT_Face>(pFont);

		m_vctText.clear();
		
		if (face)
		{
			size_t	stLen = strlen(sText);

			int32_t nY = y;
			int32_t nWidth = x;
			FT_UInt  glyphIndex;
			FT_GlyphSlot  Slot;
			int32_t dAdvance;
			Entity* pEntity;
			uint8_t* pbyData;
			int nGlyphIndexCmp;
			
			bool bRgb = !(face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA);
			
			m_vctText.resize(stLen);
			for (size_t n = 0; n < stLen; n++)
			{

				glyphIndex = FT_Get_Char_Index(face, sText[n]);

				FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
				FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

				Slot = face->glyph;

				pEntity = new Entity{ face->glyph->bitmap.width, face->glyph->bitmap.rows,  m_byBpp };
				dAdvance = face->glyph->metrics.horiAdvance >> 6;
				pbyData = pEntity->pbyBuffer;
		 
				if (pbyData)
				{
					if (bRgb)
					{
						for (int i = 0; i < (int)face->glyph->bitmap.rows; i++)
						{
							for (int j = 0; j < (int)face->glyph->bitmap.width; j++)
							{
								nGlyphIndexCmp = (i * face->glyph->bitmap.pitch + j);
								*(pbyData++) = (uint8_t)(~face->glyph->bitmap.buffer[nGlyphIndexCmp]);
								*(pbyData++) = (uint8_t)(~face->glyph->bitmap.buffer[nGlyphIndexCmp]);
								*(pbyData++) = (uint8_t)(~face->glyph->bitmap.buffer[nGlyphIndexCmp]);
								pbyData++;
							}
						}
					}
					else
					{
						//TODO RGBA support
					}


					pEntity->nX = nWidth;
					pEntity->nY = nY - Slot->bitmap_top;

					m_vctText[n] = pEntity;

					nWidth += (uint32_t)(dAdvance - Slot->bitmap_left);

					nY += Slot->advance.y >> 6;
				}
			}
		}

		return m_vctText;
	}

	
	const std::vector<const Entity*>& CharSet::format(const char* szTxt)
	{
		
		size_t	stLen = strlen(szTxt);
		m_vctText.clear();
		m_vctText.resize(stLen);
		for (size_t n = 0; n < stLen; n++)
		{
			std::unordered_map<const char, Entity*>::iterator it = m_Chars.find(szTxt[n]);
			if (it != m_Chars.end())
				m_vctText[n] = it->second;
		}

		return m_vctText;
	}

}//draw
