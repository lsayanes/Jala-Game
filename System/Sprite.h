

#pragma once

namespace draw
{
	class Sprite
	{
	private:
		std::vector<Entity*> m_Entities;

	public:
		explicit Sprite(size_t w, size_t h, uint8_t bits, size_t stTotal);
		virtual ~Sprite();

		bool	load(size_t stIndex, const char* szPath);
		void	pos(int x, int y);

		const Entity& operator[](size_t i) const;


		static void	bgraToRgba(uint8_t* pbyBgra, size_t stSize);
	};

}//draw