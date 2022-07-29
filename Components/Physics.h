#pragma once

namespace draw
{
	namespace components
	{
		//using phy_selected_t = short;
		template <typename TYPE>
		struct Physics
		{
			TYPE x{ 0 };
			TYPE y{ 0 };

			explicit Physics(TYPE x_t, TYPE y_t):
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
		};
	}//components
} //draw