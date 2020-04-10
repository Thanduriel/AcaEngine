#include "keyboardInterface.hpp"
#include "utils/assert.hpp"
#include <nlohmann/json.hpp>

namespace input
{
	using namespace std::string_literals;
	using namespace nlohmann;

	void to_json(json& j, const MKBKey& _key) 
	{
		if (std::holds_alternative<MouseButton>(_key))
			j = -static_cast<int>(std::get<MouseButton>(_key)) - 1;
		else
			j = static_cast<int>(std::get<Key>(_key));
	}

	void from_json(const json& j, MKBKey& _key)
	{
		if (j.is_null()) _key = std::monostate{};
		else
		{
			const int i = j.get<int>();
			if (i >= -1) _key = static_cast<Key>(i);
			else _key = static_cast<MouseButton>(-(i + 1));
		}
	}

	KeyboardInterface::KeyboardInterface(nlohmann::json& _config,
		const std::vector<std::pair<std::string, MKBKey>>& _defaults,
		const std::vector<VirtualAxis>& _axis)
		: m_axis(_axis)
	{
		unsigned c = 0;
		for (const auto& [name, key] : _defaults)
		{
			MKBKey k = _config[name].get<MKBKey>();
			if (std::holds_alternative<std::monostate>(k))
			{
				_config[name] = key;
				k = key;
			}
			m_inputMap.add(Action(c), k);
			++c;
		}
	}


	bool KeyboardInterface::isKeyPressed(Action _action) const
	{
		auto hndl = m_inputMap.find(_action);
		ASSERT(!!hndl, "Action is not mapped to a key.");
		return isKeyPressed(hndl.data());
	}
	
	float KeyboardInterface::getAxis(Axis _axis) const
	{
		const VirtualAxis va = m_axis[_axis];

		float axis = 0.f;
		if (isKeyPressed(va.low))
			axis -= 1.f;
		if (isKeyPressed(va.high))
			axis += 1.f;

		return axis;
	}

	glm::vec2 KeyboardInterface::getCursorPos() const
	{
		return InputManager::getCursorPos();
	}

	bool KeyboardInterface::isKeyPressed(const MKBKey& _key)
	{
		if (std::holds_alternative<MouseButton>(_key))
			return InputManager::isButtonPressed(std::get<MouseButton>(_key));
		else
			return InputManager::isKeyPressed(std::get<Key>(_key));
	}
}