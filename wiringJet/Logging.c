/*
 * wiringJet Logging.c
 * Simple logging framework for the wiringJet library
 * 
 ***********************************************************************
 *
 *    wiringJet is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringJet is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringJet.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#define __STDC_WANT_LIB_EXT2__ 1  //Define you want TR 24731-2:2010 extensions

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdarg.h>
#include "wiringJetLogging.h"


LoggingCallback LogFunction = 0x00;
LogLevel LoggingLevel = LogLevelAll;


//  Get the unix time in long long milliseconds
//
unsigned long long GetUnixTimeMilliseconds()
{
	struct timeval tv;
	gettimeofday(&tv, 0x00);
			
	return ((unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000);
}


//  Log Function
//
void Log(LogLevel level, const char* sender, const char* function, const char* data)
{
	if (LogFunction)
	{
		LogEvent logItem;
		logItem.LogUnixTimeMilliseconds = GetUnixTimeMilliseconds();
		logItem.Level = level;
		logItem.Thread = 0;
		logItem.Sender = sender;
		logItem.Function = function;
		logItem.Data = data;
		
		if ( level >= LoggingLevel )
			LogFunction(logItem);
	}
}


//  Log function with string format
//
void LogFormatted(LogLevel level, const char* sender, const char* function, const char* format, ...)
{
	if (LogFunction)
	{
		char* data;
		va_list args;

		va_start(args, format);
		if (0 > vasprintf(&data, format, args)) data = 0x00;     //this is for logging, so failed allocation is not fatal
		va_end(args);

		if (data) {
			Log(level, sender, function, data);
			free(data);
		}
		else {
			Log(LogLevelWarn, sender, function, "Error while logging a message: Memory allocation failed.");
		}
	}
}