#pragma once

#include <array>
#include <unordered_map>
#include <string_view>

namespace input
{
	class ActionInfo
	{
		std::string_view name;
	};

	
	using Action = unsigned;
	using Axis = unsigned;

	/*enum struct Axis
	{
		UpDown,
		LeftRight,
		COUNT
	};

	enum struct Action
	{
		None,

		CastPrimary,

		MenuDown,
		MenuLeft,
		MenuRight,
		MenuUp,

		// axis actions
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight,

		COUNT
	};*/

	// map axis to pairs of actions on keyboards
	struct AxisAction
	{
		Action low;
		Action high;
	};
	/*
	constexpr std::array<AxisAction,  static_cast<size_t>(Axis::COUNT)> AXIS_ACTIONS =
	{ {
		{Action::MoveDown, Action::MoveUp},
		{Action::MoveLeft, Action::MoveRight}
	} };*/
}
