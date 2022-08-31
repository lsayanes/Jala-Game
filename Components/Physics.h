#pragma once

namespace draw
{
	namespace components
	{
		//using phy_selected_t = short;
		template <typename TYPE>
		struct Physics : public Component
		{
			TYPE x{ 0 };
			TYPE y{ 0 };
			TYPE vx{ 1 };
			TYPE vy{ 1 };

			explicit Physics(TYPE x_t, TYPE y_t, size_t eID):
				Component{eID},
				x{x_t},
				y{y_t}
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
						entity.x = TYPE &point
						prop_type point_reach = entity.w
						
						TYPE from = screenImg.x, 
						prop_type to = screenImg.x + screenImg.w
			*/
			inline void center(TYPE &point, prop_type point_reach,  TYPE from, prop_type to)
			{
				point = static_cast<TYPE>((from + ((to - from) / 2)) - (point_reach / 2));
			}

			inline void centerx(prop_type w, TYPE from, prop_type to)
			{
				center(x, w, from, to);
			}

			inline void centery(prop_type h, TYPE from, prop_type to)
			{
				center(y, h, from, to);
			}
		};
	}//components
} //draw