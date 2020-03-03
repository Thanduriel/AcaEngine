#include "assert.hpp"
#include <spdlog/spdlog.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace details
{
	void assertHandler(const std::string& file, long line, const std::string& functionName,
		const std::string& conditionName, const std::string& errorMessage)
	{
		std::string message("Assertion failed in file \"" + file + "\" line " + std::to_string(line) +
			" (function \"" + functionName + "\"): " +
			conditionName + "\n" + errorMessage);
#ifdef _WIN32
		MessageBoxA(0, message.c_str(), "Assertion!", MB_OK | MB_ICONERROR);
#endif
		spdlog::error(message);
	}
}