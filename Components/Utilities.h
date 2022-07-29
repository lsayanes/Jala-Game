
#pragma once

namespace draw
{
	namespace utilities
	{
		size_t type{ 0 }; //contador de tipo

		template<typename TYPE>
		size_t getType()
		{
			static std::size_t type{ ++type }; //como es un template, solo se incializara una vez por TYPE, si paso el mismo TYPE ejecutara solo return type
			return type;
		}
	};
};