#pragma once

#include <nlohmann/json.hpp>

namespace utils {

	class Config
	{
	public:
		static void load();
		static nlohmann::json& get();
		static void save();

	private:
		static bool s_isLoaded;
		static nlohmann::json s_config;
	};
}