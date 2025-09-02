#pragma once

#include <stdio.h>

namespace uaudio
{
	namespace logger
	{
		typedef enum LogSeverity {
			LOGSEVERITY_INFO,
			LOGSEVERITY_SUCCESS,
			LOGSEVERITY_WARNING,
			LOGSEVERITY_ERROR,
			LOGSEVERITY_ASSERT
		} LogSeverity;

		inline char arr[5][28] =
		{
			"INFO",
			"\033[0;32mSUCCESS\033[0;37m",
			"\033[0;33mWARNING\033[0;37m",
			"\033[31mERROR\033[0;37m",
			"\033[31mASSERT\033[0;37m"
		};

		class Logger
		{
		public:
			template<typename... Args>
			static void Print(LogSeverity a_Severity, const char* f, Args... args)
			{
				printf("[%s] ", arr[a_Severity]);
				printf(f, args...);
				printf("\n");
			}
		};
	}
}