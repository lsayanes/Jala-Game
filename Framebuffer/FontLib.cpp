
#include <vector>
#include <memory>

#include <ft2build.h>
#include <freetype/ftcolor.h>

#include FT_FREETYPE_H
#include FT_STROKER_H


#include "Raster.h"
#include "Entity.h"

#include "FontLib.h"

namespace draw
{
	static FT_Library g_ftLibrary;
	FontLib* FontLib::m_pInst = nullptr;

	void FontLib::free()
	{
		FT_Done_FreeType(g_ftLibrary);

		if (FontLib::m_pInst)
		{
			delete FontLib::m_pInst;
			FontLib::m_pInst = nullptr;
		}
	}

	bool FontLib::init() const
	{
		return !FT_Init_FreeType(&g_ftLibrary);
	}

	void* FontLib::newFont(const char* szFontPath, size_t stSize) const
	{

		FT_Error err;
		FT_Face face;
		void* pRet = nullptr;

		if (0 == (err = FT_New_Face(g_ftLibrary, szFontPath, 0, &face)) && (0 == (err = FT_Set_Char_Size(face, stSize << 6, stSize << 6, 90, 90))))
			pRet = static_cast<void*>(face);


		return pRet;
	}

	FontLib* FontLib::instance()
	{
		if (nullptr == FontLib::m_pInst)
		{
			FontLib::m_pInst = new FontLib();
			if (!FontLib::m_pInst->init())
				FontLib::m_pInst->free();
		}

		return FontLib::m_pInst;
	}

	
}