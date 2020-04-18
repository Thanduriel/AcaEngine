#include "config.hpp"
#include <fstream>

namespace utils {

	using json = nlohmann::json;

	bool Config::s_isLoaded = false;
	json Config::s_config;

	void Config::load()
	{
		try {
			std::ifstream file("config.json");
			file >> s_config;
		}
		catch (...)
		{
		}

		s_isLoaded = true;
	}

	json& Config::get()
	{
		if (!s_isLoaded) load();

		return s_config;
	}

	void Config::save()
	{
		std::ofstream file("config.json");
		file << s_config;
	}
}