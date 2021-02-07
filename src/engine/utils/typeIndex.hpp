#pragma once

namespace utils {

	class TypeIndex
	{
		static int s_counter;
	public:
		template<typename T>
		static int value()
		{
			static int id = s_counter++;
			return id;
		}
	};

}