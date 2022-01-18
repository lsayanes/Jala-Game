
#include <vector>
#include <memory>

#include <ft2build.h>
#include <freetype/ftcolor.h>

#include FT_FREETYPE_H
#include FT_STROKER_H


#include "Raster.h"
#include "Entity.h"

#include "Font.h"

namespace draw
{
	static FT_Library g_ftLibrary;
	Font* Font::m_pInst = nullptr;
	void* Font::m_pFaceTag = nullptr;


	void Font::free()
	{
		FT_Done_FreeType(g_ftLibrary);
		
		if (Font::m_pFaceTag)
		{
			delete Font::m_pFaceTag;
			Font::m_pFaceTag = nullptr;
		}
		
		if (Font::m_pInst)
		{
			delete Font::m_pInst;
			Font::m_pInst = nullptr;
		}
	}

	bool Font::init() const
	{
		return !FT_Init_FreeType(&g_ftLibrary);
	}

	bool Font::newFont(const char* szFontPath, size_t stSize) const
	{

		FT_Error err;
		if (Font::m_pFaceTag)
			delete Font::m_pFaceTag;
		
		Font::m_pFaceTag = new FT_Face;
		FT_Face *face  = (FT_Face*)Font::m_pFaceTag;
		
		if (0 == (err = FT_New_Face(g_ftLibrary, szFontPath, 0, face)))
			err = FT_Set_Char_Size(*face, stSize << 6, stSize << 6, 90, 90);

		return !err;
	}

	Font* Font::instance()
	{
		if (nullptr == Font::m_pInst)
		{
			Font::m_pInst = new Font();
			if (!Font::m_pInst->init())
				Font::m_pInst->free();
		}

		return Font::m_pInst;
	}

	
}