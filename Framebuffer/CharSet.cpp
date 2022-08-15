

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


#include <Config.h>

#include <Component.h>
#include <Properties.h>
#include <Physics.h>

#include <Tools.h>

#include "Entity.h"

#include "Sprite.h"
#include "FontLib.h"

#include "CharSet.h"

namespace draw
{

	CharSet::CharSet(void* pFont, uint8_t byBpp) :
		m_pFont{pFont},
		m_byBpp{byBpp}
	{
		if(!m_pFont)
			throw std::invalid_argument("pFont is nullptr");


		m_vctText.reserve(1024);
	}

	CharSet::~CharSet()
	{
		tools::deletePtrVct<std::vector<Entity*>>(m_vctText);
	}
	
	std::vector<Entity*>* CharSet::flatText(const char* sText, int x, int y)
	{
		FT_Face face = static_cast<FT_Face>(m_pFont);

		tools::deletePtrVct<std::vector<Entity*>>(m_vctText);
		
		size_t	stLen = strlen(sText);

		int32_t nY = y;
		int32_t nWidth = x;
		FT_UInt  glyphIndex;
		FT_GlyphSlot  Slot;
		int32_t dAdvance;
		Entity* pEntity;
		uint8_t* pbyData;
		int nGlyphIndexCmp;
			
		//bool bRgb = !(face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA);

			
		m_vctText.resize(stLen);
		for (size_t n = 0; n < stLen; n++)
		{
			glyphIndex = FT_Get_Char_Index(face, sText[n]);

			FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

			Slot = face->glyph;

			pEntity = new Entity{ face->glyph->bitmap.width, face->glyph->bitmap.rows,  m_byBpp, 0 };
			dAdvance = face->glyph->metrics.horiAdvance >> 6;
			pbyData = pEntity->data().get();
			auto& phyRef = pEntity->physics();
		 
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

			phyRef.x = nWidth;
			phyRef.y = nY - Slot->bitmap_top;

			m_vctText[n] = pEntity;

			nWidth += (uint32_t)(dAdvance - Slot->bitmap_left);

			nY += Slot->advance.y >> 6;
		}

		return &m_vctText;
	}


}//draw
