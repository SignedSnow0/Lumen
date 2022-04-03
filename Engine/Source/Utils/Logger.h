#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include "Core/Types.h"

namespace Lumen::Utils
{
	enum class LogLevel : u32
	{
		Off,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};
	
	class Logger
	{
	public:
		static void SetLogLevel(const LogLevel& level);
	};
}

#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_FATAL(...) SPDLOG_FATAL(__VA_ARGS__)