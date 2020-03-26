#pragma once

namespace game {

	template<typename T, typename... Comps>
	class Action : public T
	{
		using T::T;
	};
}