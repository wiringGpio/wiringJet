#pragma once


//  Log Levels
//
typedef enum
{
	LogLevelAll,
	LogLevelTrace,
	LogLevelDebug,
	LogLevelInfo,
	LogLevelWarn, 
	LogLevelError,
	LogLevelFatal,
	LogLevelOff,
} wiringGpioLogLevel;
	
	
//  Log Event
//
typedef struct 
{
	unsigned long long LogUnixTimeMilliseconds;
	wiringGpioLogLevel Level;
	int Thread;
	const char* Sender;
	const char* Function;
	const char* Data;
		
} wiringGpioLogEvent;
	
//  Logging Callback
//
typedef void(*wiringGpioLoggingCallback)(wiringGpioLogEvent);