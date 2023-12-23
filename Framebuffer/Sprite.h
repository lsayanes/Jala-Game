

#pragma once

namespace draw
{
	class Sprite
	{
	private:
		draw_t 		width;
		draw_t 		height;
		uint32_t 	curr;

		std::vector<Entity*> m_Entities;

	public:
		explicit Sprite(draw_t w, draw_t h);
		virtual ~Sprite();

		Sprite(const Sprite&) = delete;
		Sprite(Sprite&&) = delete;
		Sprite& operator=(const Sprite&) = delete;
		Sprite& operator=(Sprite&&) = delete;

		void 	add(Entity *pEntity);
		void	pos(int x, int y);

		[[nodiscard]]  Entity &get();

		[[nodiscard]] inline const draw_t h() const { return height; }
		[[nodiscard]] inline const draw_t w() const { return width; }
		[[nodiscard]] inline const uint32_t total() const  { return m_Entities.size(); }
/*
		[[nodiscard]] const Entity& operator[](size_t i) const;
		[[nodiscard]] inline const Entity* at(size_t i) const { return m_Entities[i]; }

		[[nodiscard]] Entity* operator&(size_t i) const { return m_Entities.at(i); }
		*/
	};

}//draw