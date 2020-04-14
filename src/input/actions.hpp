#pragma once

#include <array>
#include <unordered_map>
#include <string_view>

namespace input
{
	namespace details {
		template<typename>
		struct EnumId
		{
			template<typename T>
				requires std::is_enum_v<T>
			EnumId(const T& _id)
				: id(static_cast<unsigned>(_id))
			{}

			explicit EnumId(unsigned _id) : id(_id) {}

			bool operator==(const EnumId& _rhs) const { return id == _rhs.id; }

			unsigned id;
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
			return hash<unsigned>()(x.id);
		}
	};
}
