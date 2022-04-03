#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Lumen::Utils
{
	void Logger::SetLogLevel(const LogLevel& level)
	{
		spdlog::set_pattern("[%T.%e] [%s:%#] [%^%l%$] %v");

		switch (level)
		{
		case LogLevel::Off:
			spdlog::set_level(spdlog::level::level_enum::off);
			break;
		case LogLevel::Trace:
			spdlog::set_level(spdlog::level::level_enum::trace);
			break;
		case LogLevel::Debug:
			spdlog::set_level(spdlog::level::level_enum::debug);
			break;
		case LogLevel::Info:
			spdlog::set_level(spdlog::level::level_enum::info);
			break;
		case LogLevel::Warning:
			spdlog::set_level(spdlog::level::level_enum::warn);
			break;
		case LogLevel::Error:
			spdlog::set_level(spdlog::level::level_enum::err);
			break;
		case LogLevel::Fatal:
			spdlog::set_level(spdlog::level::level_enum::critical);
			break;
		}
	}
}
