#pragma once

namespace utils {

	// Allows access to a functor's signature through template argument deduction.
	// For a usage example see Registry::execute().
	template<typename T, typename... Args>
	struct UnpackFunction
	{
		UnpackFunction(void(T::*)(Args ...) const) {};
		UnpackFunction(void(T::*)(Args ...)) {};
	};

	template<typename T>
	struct TypeHolder
	{
		using type = T;
	};

	// determine whether a parameter pack contains a specific type
	template<typename What, typename ... Args>
	struct contains_type 
	{
		static constexpr bool value{ (std::is_same_v<What, Args> || ...) };
	};
}