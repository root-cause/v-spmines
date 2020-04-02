#pragma once
#include "pch.h"

enum class LogLevel
{
	LOG_NONE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_ERROR
};

static const char* _logLevelPrefixes[4] = { "", "DEBUG", "INFO", "ERROR" };

class Logger
{
private:
	std::filesystem::path m_logFilePath;
	LogLevel m_logLevel = LogLevel::LOG_NONE;

public:
	Logger() = default;
	Logger(const Logger&) = delete;
	Logger(Logger&&) = delete;
	Logger& operator=(Logger&&) = delete;
	Logger& operator=(const Logger&) = delete;
	Logger(std::string_view logFileName, LogLevel logLevel, bool truncate);

	void SetFileName(std::string_view newLogFileName);
	void SetLogLevel(LogLevel newLogLevel);
	void Write(std::string_view text) const;
	void Write(LogLevel logLevel, std::string_view text) const;
	void Clear() const;
};