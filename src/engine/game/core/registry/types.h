/**
 * Project Untitled
 */

#pragma once

#include <bitset>
#include <concepts>
#include <iterator>
#include <array>
#include <algorithm>

namespace registry {
	template<typename T>
	concept Component = std::is_move_constructible<T>::value;

	template<typename T>
	concept ForwardReadIterator = std::indirectly_readable<T> && std::forward_iterator<T>;

	using size_t               = unsigned;
	constexpr size_t HASH_SIZE = 128;
	using type_hash_t          = std::bitset<HASH_SIZE>;
	using typeid_t             = unsigned;
	using generation_t 		   = unsigned;
	using move_fn_t            = void ( * )( void*, void* );
	using copy_fn_t            = move_fn_t;
	using destructor_t         = void ( * )( void* );

	namespace utils {

		/** get functor signature with template deduction */
		template<typename Fn, Component... Cs>
		struct UnpackFunctor
		{
			UnpackFunctor( void ( Fn::* )( Cs... ) ){};
			UnpackFunctor( void ( Fn::* )( Cs... ) const ){};
		};

		template<typename, typename> struct prepend;
		template<typename I, template<typename...>class S, typename ... Is>
		struct prepend<I, S<Is...>> {
			using type = S<I, Is...>;
		};

		template<template <typename> class F, typename S> struct filter;
		template<template<typename> class F, template<typename...> class S> struct filter<F, S<>> {
			using type = S<>;
		};
		template<template <typename> class F, template<typename ...> class S, typename I, typename ...Is>
		struct filter<F, S<I, Is...>> {
			using type = std::conditional_t< F<I>{},
				  typename prepend<I, typename filter<F, S<Is...>>::type>::type,
				  typename filter<F, S<Is...>>::type>;
		};

		template<typename ...T> struct sequence{};

	}   // namespace utils
}   // end of namespace registry
