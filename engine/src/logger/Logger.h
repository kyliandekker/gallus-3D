#pragma once

#include "core/System.h"

#include <assert.h>
#include <queue>
#include <thread>
#include <string>
#include <mutex>
#include <chrono>

// core includes
#include "core/Event.h"

// utils includes
#include "utils/file_abstractions.h"

namespace gallus
{
	// Category defines
#define LOG_CATEGORY_MEMORY "MEMORY"
#define LOG_CATEGORY_CORE "CORE"
#define LOG_CATEGORY_ENGINE "ENGINE"
#define LOG_CATEGORY_GAME "GAME"
#define LOG_CATEGORY_INPUT "INPUT"
#define LOG_CATEGORY_WINDOW "WINDOW"
#define LOG_CATEGORY_LOGGER "LOGGER"
#define LOG_CATEGORY_DX12 "DX12"
#define LOG_CATEGORY_EDITOR "EDITOR"
#define LOG_CATEGORY_ECS "ECS"
#define LOG_CATEGORY_AUDIO "AUDIO"
#define LOG_CATEGORY_TEST "TEST"

#define ASSERT_LEVEL gallus::LogSeverity::LOGSEVERITY_ASSERT
	constexpr bool LOG_TO_FILE = false;
// NOTE: This one is out of the other namespaces so they can be easily used in other classes.
	typedef enum LogSeverity
	{
		LOGSEVERITY_ASSERT,
		LOGSEVERITY_ERROR,
		LOGSEVERITY_WARNING,
		LOGSEVERITY_INFO,
		LOGSEVERITY_TEST,
		LOGSEVERITY_SUCCESS,
		LOGSEVERITY_INFO_SUCCESS,
		LOGSEVERITY_AWESOME,
	} LogSeverity;

	/// <summary>
	/// Converts a log severity enumeration value to its corresponding string representation.
	/// </summary>
	/// <param name="a_LogSeverity">The log severity to convert.</param>
	/// <returns>A string representing the specified log severity.</returns>
	inline const std::string LogSeverityToString(LogSeverity a_LogSeverity)
	{
		switch (a_LogSeverity)
		{
			case LOGSEVERITY_ASSERT:
			{
				return "ASSERT";
			}
			case LOGSEVERITY_ERROR:
			{
				return "ERROR";
			}
			case LOGSEVERITY_WARNING:
			{
				return "WARNING";
			}
			case LOGSEVERITY_INFO:
			{
				return "INFO";
			}
			case LOGSEVERITY_TEST:
			{
				return "TEST";
			}
			case LOGSEVERITY_SUCCESS:
			{
				return "SUCCESS";
			}
			case LOGSEVERITY_INFO_SUCCESS:
			{
				return "INFO SUCCESS";
			}
			case LOGSEVERITY_AWESOME:
			{
				return "AWESOME";
			}
		}
		return "";
	}

	namespace logger
	{
		/// 0 = full path, 1 = filename, 2 = stem, 3 = parent path + filename
		#define LOG_SHORT_FILENAMES 3

		//---------------------------------------------------------------------
		// LoggerMessage
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents the logger message with variables for location, category and severity.
		/// </summary>
		class LoggerMessage
		{
		public:
			LoggerMessage(const std::string& a_sRawMessage, const std::string& a_sCategory, const fs::path& a_Location, uint32_t a_iLine, LogSeverity a_Severity, const std::chrono::system_clock::time_point& a_Time);

			/// <summary>
			/// Retrieves the raw message.
			/// </summary>
			/// <returns>A string containing the message without any category, location and severity information.</returns>
			const std::string& GetRawMessage() const;

			/// <summary>
			/// Retrieves the logging category.
			/// </summary>
			/// <returns>A string containing the logging category.</returns>
			const std::string& GetCategory() const;

			/// <summary>
			/// Retrieves the location the logging took place in.
			/// </summary>
			/// <returns>A string containing the location the logging took place in.</returns>
			const fs::path& GetLocation() const;

			/// <summary>
			/// Retrieves the line the logging took place at.
			/// </summary>
			/// <returns>A number containing the line the logging took place at.</returns>
			uint32_t GetLine() const;

			/// <summary>
			/// Retrieves the severity.
			/// </summary>
			/// <returns>The log severity enumeration.</returns>
			LogSeverity GetSeverity() const;

			/// <summary>
			/// Retrieves the time the message was logged.
			/// </summary>
			/// <returns>The time point containing information about when the message was logged.</returns>
			const std::chrono::system_clock::time_point& GetTime() const;
		private:
			std::string m_sRawMessage; /// The message without any category, location and severity information.
			std::string m_sCategory; /// The logging category.
			fs::path m_Location; /// The file the logging took place in.
			uint32_t m_iLine = 0; /// The line of the file the logging took place in.
			LogSeverity m_Severity; /// The severity of the log message.
			std::chrono::system_clock::time_point m_Time; /// The time the logging was done.
		};

		//---------------------------------------------------------------------
		// Logger
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents the logger system that outputs log messages to the console and a log file.
		/// </summary>
		class Logger : public core::ThreadedSystem
		{
		public:
			~Logger();

			/// <summary>
			/// Loop method for the thread.
			/// </summary>
			void Loop() override;

			/// <summary>
			/// Signals the thread to stop.
			/// </summary>
			bool Destroy() override;

			/// <summary>
			/// Logs a message with category, file and line info to the console and a file.
			/// </summary>
			/// <param name="a_Severity">Severity of the message.</param>
			/// <param name="a_sCategory">The category the message is in (used for class recognition).</param>
			/// <param name="a_sMessage">The message that will be logged.</param>
			/// <param name="a_sFile">The file it was logged from.</param>
			/// <param name="a_iLine">The line of the file it was logged from.</param>
			void Log(LogSeverity a_Severity, const char* a_sCategory, const char* a_sMessage, const char* a_sFile, int a_iLine);

			/// <summary>
			/// Logs a formatted message with category, file and line info to the console and a file.
			/// </summary>
			/// <param name="a_Severity">Severity of the message.</param>
			/// <param name="a_sCategory">The category the message is in (used for class recognition).</param>
			/// <param name="a_sMessage">The message that will be logged.</param>
			/// <param name="a_sFile">The file it was logged from.</param>
			/// <param name="a_iLine">The line of the file it was logged from.</param>
			void LogF(LogSeverity a_Severity, const char* a_sCategory, const char* a_sMessage, const char* a_sFile, int a_iLine, ...);

			/// <summary>
			/// Prints a message to the console.
			/// </summary>
			/// <param name="a_Severity">Severity of the message.</param>
			/// <param name="a_sCategory">The category the message is in (used for class recognition).</param>
			/// <param name="a_sMessage">The message that will be logged.</param>
			/// <param name="a_sFile">The file it was logged from.</param>
			/// <param name="a_iLine">The line of the file it was logged from.</param>
			void PrintMessage(LogSeverity a_Severity, const char* a_sCategory, const char* a_sMessage, const char* a_sFile, int a_iLine);

			/// <summary>
			/// Retrieves the on message logged event.
			/// </summary>
			/// <returns>Reference to the on quit event.</returns>
			const SimpleEvent<LoggerMessage>& OnMessageLogged() const;
		protected:
			bool Sleep() const override;
		private:
			/// <summary>
			/// Called once on the thread to perform initialization steps.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			bool InitThreadWorker() override;

			/// <summary>
			/// Destroys the system, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			void Finalize() override;

			SimpleEvent<LoggerMessage> m_eOnMessageLogged;
			std::queue<LoggerMessage> m_Messages; /// Queue of messages that will be logged.
			mutable std::mutex m_MessagesMutex; /// The mutex used for synchronization between the threads for stopping or initializing.
			std::condition_variable m_MessageCondVar; /// Used for lazy thread.
		};
		inline extern Logger LOGGER = {};
	}
}

// Messages should be like this: "STATUS ACTION", so "Created x" or "Failed creating x"
#define LOGF(a_Severity, a_sCategory, a_sMessage, ...)\
do{\
	gallus::logger::LOGGER.LogF(a_Severity, a_sCategory, a_sMessage, __FILE__, __LINE__, __VA_ARGS__);\
	if (a_Severity <= ASSERT_LEVEL)\
		assert(false);\
} while (0)

// Messages should be like this: "STATUS ACTION", so "Created x" or "Failed creating x"
#define LOG(a_Severity, a_sCategory, a_sMessage)\
do{\
	gallus::logger::LOGGER.Log(a_Severity, a_sCategory, a_sMessage, __FILE__, __LINE__);\
	if (a_Severity <= ASSERT_LEVEL)\
		assert(false);\
} while (0)

#define TEST(a_sMessage)\
do{\
	gallus::logger::LOGGER.Log(LOGSEVERITY_TEST, LOG_CATEGORY_TEST, a_sMessage, __FILE__, __LINE__);\
} while (0)

#define TESTF(a_sMessage, ...)\
do{\
	gallus::logger::LOGGER.LogF(LOGSEVERITY_TEST, LOG_CATEGORY_TEST, a_sMessage, __FILE__, __LINE__, __VA_ARGS__);\
} while (0)