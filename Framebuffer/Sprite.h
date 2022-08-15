

#pragma once

namespace draw
{
	class Sprite
	{
	private:
		std::vector<Entity*> m_Entities;

	public:
		explicit Sprite(size_t w, size_t h, uint8_t bits, size_t stTotal = 1);
		virtual ~Sprite();

		bool	load(size_t stIndex, const char* szPath);
		void	pos(int x, int y);

		const Entity& operator[](size_t i) const;
		inline const Entity* at(size_t i) const { return m_Entities[i]; }
		
		Entity* operator&(size_t i) const { return m_Entities.at(i); }
	};

}//draw