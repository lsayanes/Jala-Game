


#pragma once
#include <iostream>
#include <chrono>

namespace draw
{
	class Sprite
	{
	private:
		draw_t 		width;
		draw_t 		height;
		uint32_t 	curr;

		std::vector<Entity*> m_Entities;


		std::chrono::steady_clock::time_point	lastTime;

	public:
		int64_t	 	m_dElapsed ;

	public:
		explicit Sprite(draw_t w, draw_t h);
		virtual ~Sprite();

		Sprite(const Sprite&) = delete;
		Sprite(Sprite&&) = delete;
		Sprite& operator=(const Sprite&) = delete;
		Sprite& operator=(Sprite&&) = delete;

		void 	add(Entity *pEntity);
		void 	add(uint8_t* prgbaData, int32_t width, int32_t height, bool useAlpha);
		void	pos(int x, int y);

		[[nodiscard]]  Entity &get();

		[[nodiscard]] inline const draw_t h() const { return height; }
		[[nodiscard]] inline const draw_t w() const { return width; }
		[[nodiscard]] inline const uint32_t total() const  { return static_cast<uint32_t>(m_Entities.size()); }
/*
		[[nodiscard]] const Entity& operator[](size_t i) const;
		[[nodiscard]] inline const Entity* at(size_t i) const { return m_Entities[i]; }

		[[nodiscard]] Entity* operator&(size_t i) const { return m_Entities.at(i); }
		*/
	};

}//draw