#pragma once

#include <array>
#include <unordered_map>
#include <string_view>

namespace input
{
	namespace details {
		// dummy parameter to create unique specializations
		template<typename>
		struct EnumId
		{
			// Allow implicit construction from enum types.
			template<typename T>
				requires std::is_enum_v<T>
			constexpr EnumId(const T& _id)
				: m_id(static_cast<unsigned>(_id))
			{}

			constexpr explicit EnumId(unsigned _id) : m_id(_id) {}

			constexpr bool operator==(const EnumId& _rhs) const { return m_id == _rhs.m_id; }
			constexpr unsigned id() const { return m_id; }
		private:
			unsigned m_id;
		};

		struct Action {};
		struct Axis {};
	}

	using Action = details::EnumId<details::Action>;
	using Axis = details::EnumId<details::Axis>;

	// map axis to pairs of actions on keyboards
	struct VirtualAxis
	{
		Action low;
		Action high;
	};
}

namespace std {
	template <typename T> 
	struct hash<input::details::EnumId<T>>
	{
		size_t operator()(const input::details::EnumId<T>& x) const
		{
			return hash<unsigned>()(x.id());
		}
	};
}
