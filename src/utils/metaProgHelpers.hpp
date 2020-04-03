#pragma once

namespace utils {

/*	template<bool Const, typename... Args>
	struct UnpackFunction { };

	template<typename T, typename... Args>
	struct UnpackFunction<false, T, Args...>
	{
		using Type = void(T::*)(Args ...);
	};*/

	template<typename T, typename... Args>
	struct UnpackFunction
	{
		UnpackFunction(void(T::*)(Args ...) const) {};
		UnpackFunction(void(T::*)(Args ...)) {};
	};
}