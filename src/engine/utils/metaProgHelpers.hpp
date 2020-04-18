#pragma once

namespace utils {

	// Allows access to a functor's signature through template argument deduction.
	template<typename T, typename... Args>
	struct UnpackFunction
	{
		UnpackFunction(void(T::*)(Args ...) const) {};
		UnpackFunction(void(T::*)(Args ...)) {};
	};
}