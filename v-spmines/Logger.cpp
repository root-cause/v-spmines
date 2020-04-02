#include "pch.h"
#include "Logger.h"

Logger::Logger(std::string_view logFileName, LogLevel logLevel, bool truncate)
{
	m_logFilePath = std::filesystem::current_path().append(logFileName);
	m_logLevel = logLevel;

	if (truncate)
	{
		Clear();
	}
}

void Logger::SetFileName(std::string_view newLogFileName)
{
	m_logFilePath = std::filesystem::current_path().append(newLogFileName);
}

void Logger::SetLogLevel(LogLevel newLogLevel)
{
	m_logLevel = newLogLevel;
}

void Logger::Write(std::string_view text) const
{
	Write(m_logLevel, text);
}

void Logger::Write(LogLevel logLevel, std::string_view text) const
{
	if (m_logFilePath.empty() || m_logLevel == LogLevel::LOG_NONE || m_logLevel > logLevel)
	{
		return;
	}

	std::ofstream logFile(m_logFilePath, std::ofstream::out | std::ofstream::app);
	if (logFile)
	{
		std::time_t t = std::time(nullptr);
		logFile << fmt::format("[{::%Y-%m-%d %H:%M:%S}] [{}] {}", *std::localtime(&t), _logLevelPrefixes[static_cast<int>(logLevel)], text) << "\n";
	}
}

void Logger::Clear() const
{
	std::ofstream logFile(m_logFilePath, std::ofstream::out | std::ofstream::trunc);
}