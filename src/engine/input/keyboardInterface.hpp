#pragma once

#include "action.hpp"
#include "inputmanager.hpp"
#include "../utils/containers/hashmap.hpp"
#include <nlohmann/json_fwd.hpp>
#include <variant>

namespace input
{
	using MKBKey = std::variant<std::monostate, Key, MouseButton>;

	class KeyboardInterface : public InputInterface
	{
	public:
		KeyboardInterface(nlohmann::json& _config,
			const std::vector<std::pair<std::string, MKBKey>>& _defaults,
			const std::vector<VirtualAxis>& _axis);

		bool isKeyPressed(Action _action) const override;
		float getAxis(Axis _axis) const override;
		glm::vec2 getCursorPos() const override;
	private:
		static bool isKeyPressed(const MKBKey& _key);

		utils::HashMap< Action, MKBKey > m_inputMap;
		std::vector< VirtualAxis > m_axis;
	};
}