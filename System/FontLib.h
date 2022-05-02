#pragma once



namespace draw
{

	class FontLib
	{
	private:
		static FontLib* m_pInst;
		
		FontLib() = default;
		bool	init() const;
	public:

		static FontLib* instance();

		void	free();
		void	*newFont(const char* szFontPath, size_t stSize) const;
	};

}//draw