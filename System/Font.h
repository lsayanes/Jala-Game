#pragma once



namespace draw
{

	class Font
	{
	private:
		static void *m_pFaceTag;
		static Font* m_pInst;
		
		Font() = default;
		bool	init() const;
	public:

		static Font* instance();

		void	free();
		bool	newFont(const char* szFontPath, size_t stSize) const;
		
		inline void *face() const { return Font::m_pFaceTag; }
	};

}//draw