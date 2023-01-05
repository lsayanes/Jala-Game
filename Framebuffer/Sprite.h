

#pragma once

namespace draw
{
	class Sprite
	{
	private:
		std::vector<Entity*> m_Entities;

	public:
		explicit Sprite(draw_t w, draw_t h, uint8_t bits, size_t stTotal = 1);
		virtual ~Sprite();

		bool	load(size_t stIndex, const char* szPath);
		void	pos(int x, int y);

		[[nodiscard]] const Entity& operator[](size_t i) const;
		[[nodiscard]] inline const Entity* at(size_t i) const { return m_Entities[i]; }
		
		[[nodiscard]] Entity* operator&(size_t i) const { return m_Entities.at(i); }
	};

}//draw