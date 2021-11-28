#pragma once

#include <concepts>
#include <cstdint>
#include <array>

namespace math {
namespace random {

	// Familiy of xoshiro generators.
	// Based on the C version [1] by David Blackman and Sebastiano Vigna.
	// [1] https://prng.di.unimi.it/

	template<std::unsigned_integral T, T Shift, T Rot, T Sub>
	class xoshiro_starstar_engine
	{
	public:
		using result_type = T;

		constexpr explicit xoshiro_starstar_engine() : xoshiro_starstar_engine(0) {}

		constexpr explicit xoshiro_starstar_engine(T seed)
		{
			static_assert(sizeof(s) % sizeof(uint64_t) == 0, 
				"Initialization is not implemented for this state size.");
			constexpr size_t numInit = sizeof(s) / sizeof(uint64_t);

			uint64_t x = seed;
			for (size_t i = 0; i < numInit; ++i)
				*(reinterpret_cast<uint64_t*>(s.data()) + i) = splitmix64(x);
		}

		static constexpr T min() { return std::numeric_limits<T>::min(); }
		static constexpr T max() { return std::numeric_limits<T>::max(); }

		constexpr T operator()()
		{
			const T result = rotl(s[1] * 5, 7) * 9;
			const T t = s[1] << Shift;
			s[2] ^= s[0];
			s[3] ^= s[1];
			s[1] ^= s[2];
			s[0] ^= s[3];
			s[2] ^= t;
			s[3] = rotl(s[3], Rot);
			return result;
		}
	private:
		std::array<T,4> s;

		static constexpr uint64_t splitmix64(uint64_t& x)
		{
			uint64_t z = (x += 0x9e3779b97f4a7c15uLL);
			z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9uLL;
			z = (z ^ (z >> 27)) * 0x94d049bb133111ebuLL;
			return z ^ (z >> 31);
		}

		static constexpr T rotl(T x, int k)
		{
			return (x << k) | (x >> (Sub - k));
		}
	};

	using xoshiro256ss = xoshiro_starstar_engine<uint64_t, 17, 45, 64>;
	using xoshiro128ss = xoshiro_starstar_engine<uint32_t, 9, 11, 32>;
}
}