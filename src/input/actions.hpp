#pragma once

#include <array>
#include <unordered_map>
#include <string_view>

namespace input
{
	struct Action
	{
		template<typename T>
			requires std::is_enum_v<T>
		Action(const T& _id)
			: id(static_cast<unsigned>(_id))
		{}

		explicit Action(unsigned _id) : id(_id) {}

		bool operator==(const Action& _rhs) const { return id == _rhs.id; }

		unsigned id;
	};

	using Axis = unsigned;

	// map axis to pairs of actions on keyboards
	struct VirtualAxis
	{
		Action low;
		Action high;
	};
}

namespace std {
	template <> struct hash<input::Action>
	{
		size_t operator()(const input::Action& x) const
		{
			return hash<unsigned>()(x.id);
		}
	};
}
