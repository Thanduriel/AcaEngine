#pragma once

#include <nlohmann/json.hpp>

namespace utils {

	class Config
	{
	public:
		static void load();
		static nlohmann::json& get();
		static void save();
		// Read a value if it exists. If not the default is returned and set in the config.
		template<typename T>
		static T getValue(nlohmann::json& _json, const std::string& _key, const T& _default)
		{
			if (_json.contains(_key))
				return _json[_key].get<T>();
			_json[_key] = _default;
			return _default;
		}
	private:
		static bool s_isLoaded;
		static nlohmann::json s_config;
	};
}