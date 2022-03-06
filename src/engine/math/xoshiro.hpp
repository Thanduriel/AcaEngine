#pragma once

#include <concepts>
#include <cstdint>
#include <array>

namespace math {
namespace random {

	// Family of xoshiro generators.
	// Based on the C version by David Blackman and Sebastiano Vigna [1].
	// [1] https://prng.di.unimi.it/

	template<std::unsigned_integral T, T Shift, T Rot, T Sub>
	class xoshiro_starstar_engine
	{
	public:
		using result_type = T;

		constexpr explicit xoshiro_starstar_engine() : xoshiro_starstar_engine(0) {}

		constexpr explicit xoshiro_starstar_engine(T seed)
		{
			static_assert(sizeof(m_state) % sizeof(uint64_t) == 0, 
				"Initialization is not implemented for this state size.");
			constexpr size_t numInit = sizeof(m_state) / sizeof(uint64_t);

			uint64_t x = seed;
			for (size_t i = 0; i < numInit; ++i)
				*(reinterpret_cast<uint64_t*>(m_state.data()) + i) = splitmix64(x);
		}

		static constexpr T min() { return std::numeric_limits<T>::min(); }
		static constexpr T max() { return std::numeric_limits<T>::max(); }

		constexpr T operator()()
		{
			const T result = rotl<7>(m_state[1] * 5) * 9;
			const T t = m_state[1] << Shift;
			m_state[2] ^= m_state[0];
			m_state[3] ^= m_state[1];
			m_state[1] ^= m_state[2];
			m_state[0] ^= m_state[3];
			m_state[2] ^= t;
			m_state[3] = rotl<Rot>(m_state[3]);
			return result;
		}
	private:
		std::array<T,4> m_state;

		// a different random generator used to construct a better seed sequence
		static constexpr uint64_t splitmix64(uint64_t& x)
		{
			uint64_t z = (x += 0x9e3779b97f4a7c15uLL);
			z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9uLL;
			z = (z ^ (z >> 27)) * 0x94d049bb133111ebuLL;
			return z ^ (z >> 31);
		}

		template<T K>
		static constexpr T rotl(T x)
		{
			return (x << K) | (x >> (Sub - K));
		}
	};

	using xoshiro256ss = xoshiro_starstar_engine<uint64_t, 17, 45, 64>;
	using xoshiro128ss = xoshiro_starstar_engine<uint32_t, 9, 11, 32>;
}
}