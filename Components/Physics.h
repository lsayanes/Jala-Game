#pragma once

namespace draw
{
	namespace components
	{
		struct Physics : public Component
		{
			Rect	rc;
			draw_t	vx;
			draw_t	vy;

			explicit Physics(draw_t w, draw_t h, size_t eID):
				Component{eID},
				rc{0, 0, w, h},
				vx{ 0 }, vy{ 0 }
			{
			}

			Physics(const Physics&) = delete;
			Physics(Physics&&) = delete;
			Physics& operator=(const Physics&) = delete;
			Physics& operator=(Physics&&) = delete;

			virtual ~Physics()
			{
			}

			/*
				horizontal example: 
						entity.x = draw_t &point
						prop_type point_reach = entity.w
						
						draw_t from = screenImg.x, 
						draw_t to = screenImg.x + screenImg.w
			*/
			inline void center(draw_t &point, draw_t point_reach, draw_t from, draw_t to)
			{
				point = (from + ((to - from) / 2)) - (point_reach / 2);
			}

			inline void centerx(draw_t w, draw_t from, draw_t to)
			{
				draw_t x = rc.left;
				center(x, w, from, to);
				rc.posx(x);
			}

			inline void centery(draw_t h, draw_t from, draw_t to)
			{
				draw_t y = rc.top;
				center(y, h, from, to);
				rc.posy(y);
			}
		};
	}//components
} //draw