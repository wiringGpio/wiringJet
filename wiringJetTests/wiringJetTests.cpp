#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/time.h>
#include <stdarg.h>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <wiringJet.h>
#include "wiringJetTests.h"


using namespace std;

int testFailures = 0;

#pragma region Logging

//  Get string for log level
//
string LogLevelString(wiringGpioLogLevel level)
{
	switch (level)
	{
	case 	LogLevelTrace:
		return "TRACE";
	case LogLevelDebug:
		return "DEBUG";
	case 	LogLevelInfo:
		return "INFO";
	case 	LogLevelWarn: 
		return "WARN";
	case 	LogLevelError:
		return "ERROR";
	case 	LogLevelFatal:
		return "FATAL";
	default:
		return "Unknown";
	}
}


//  Write the log to the console
//
void LoggingFunction(wiringGpioLogEvent log)
{
	timeval logTime;
	double seconds;
	double milliseconds = modf((double)log.LogUnixTimeMilliseconds / 1000, &seconds) * 1000;
	logTime.tv_sec = (int)seconds;
	logTime.tv_usec = (int)(milliseconds * 1000);
	tm* logLocalTime = localtime(&(logTime.tv_sec));

	ostringstream os;		
	os <<  setfill('0') << setw(2) << logLocalTime->tm_hour << ":" << setw(2) << logLocalTime->tm_min << ":" << setw(2) << logLocalTime->tm_sec <<  "." << std::setw(3) << logTime.tv_usec / 1000;
	os << setfill(' ') << "   "  << left << setw(7) << LogLevelString(log.Level) << " " << left << setw(25) << log.Sender << "  " << setw(35) << log.Function << "  " <<  log.Data;			

	
	cout << os.str() << endl;
	
	if (log.Level > LogLevelWarn)
		testFailures++;
}


//  Get the unix time in long long milliseconds
//
unsigned long long GetUnixTimeMilliseconds()
{
	struct timeval tv;
	gettimeofday(&tv, 0x00);
			
	return ((unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000);
}


//  Log function for the test app
//
void Log(wiringGpioLogLevel level, const char* sender, const char* function, const char* data)
{
	if (LogFunction && level >= LoggingLevel)
	{
		wiringGpioLogEvent logItem;
		logItem.LogUnixTimeMilliseconds = GetUnixTimeMilliseconds();
		logItem.Level = level;
		logItem.Thread = 0;
		logItem.Sender = sender;
		logItem.Function = function;
		logItem.Data = data;
		
		LogFunction(logItem);
	}
}


//  Log function for the test app with string format
//
void LogFormatted(wiringGpioLogLevel level, const char* sender, const char* function, const char* format, ...)
{
	if (LogFunction && level >= LoggingLevel)
	{
		char* data;
		va_list args;

		va_start(args, format);
		if (0 > vasprintf(&data, format, args)) data = 0x00;          //this is for logging, so failed allocation is not fatal
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


//  Logging callback function
wiringGpioLoggingCallback LogFunction = 0x00;
//  Log level
wiringGpioLogLevel LoggingLevel = LogLevelAll;

#pragma endregion	//  Logging


//  Setup library by calling wiringJetSetupPhys() if it has not been called already
//
bool setupLibrary = false;
bool SetupLibrary()
{
	if (!setupLibrary)
	{
		setupLibrary = true;
		int ret = wiringJetSetupPhys();
		if (ret < 0)
		{
			LogFormatted(LogLevelError, "wiringJetTests.cpp", "SetupLibrary", "wiringJet initialisation failed. Error code:  %d", ret);
			return false;
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "SetupLibrary", "wiringJet initialisation OK.");
		}	
	}
	
	return true;
}

//  Tear down library when you are done
//
void TerminateLibrary()
{
	if (setupLibrary)
		wiringJetTerminate();
}


//  Main - test functions
//
int main(int argc, char *argv[])
{
	//  set library 
	wiringJetSetLoggingCallback(LoggingFunction);
	LogFunction = LoggingFunction;
	Log(LogLevelInfo, "wiringJetTests.cpp", "main", "Starting wiringJetTests.");
	
	int testsFailed = 0;
	int testReturn = -1;
	string argument = "all";
	if (argc > 1)
		argument = string(argv[1]);

	//  test pin output "pinOutput"
	if(argument.compare("all") == 0 || argument.compare("pinOutput") == 0)
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testPinOutput");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testPinOutput(argc, argv);
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testPinOutput. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testPinOutput with ZERO errors.");
		}
	}
	//  test pin input "pinInput"
	if(argument.compare("all") == 0 || argument.compare("pinInput") == 0)
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testPinInput");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testPinInput(argc, argv);
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testPinInput. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testPinInput with ZERO errors.");
		}
	}
	//  test gpioPwmOutput "gpioPwmOutput"
	if(argument.compare("all") == 0 || argument.compare("gpioPwmOutput") == 0)
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testGpioPwmOutput");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testGpioPwmOutput(argc, argv);
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testGpioPwmOutput. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testGpioPwmOutput with ZERO errors.");
		}
	}
	//  test ISR "isr"	
	if(argument.compare("all") == 0 || argument.compare("isr") == 0)
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testISR");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testISR(argc, argv);
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testISR. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testISR with ZERO errors.");
		}
	}
	//  test MCP23008 "mcp23008"
	if(argument.compare("all") == 0 || argument.compare("mcp23008") == 0)
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testMcp23008");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testMcp23008(argc, argv);
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testMcp23008. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testMcp23008 with ZERO errors.");
		}
	}
	//  test MCP23017 "mcp23017"
	if(argument.compare("all") == 0 || argument.compare("mcp23017") == 0)
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testMcp23017");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testMcp23017(argc, argv);
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testMcp23017. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testMcp23017 with ZERO errors.");
		}
	}
	//  test ADS1115 "ads1115"
	if(argument.compare("all") == 0 || argument.compare("ads1115") == 0) 	
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testAds1115");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testAds1115(argc, argv);	
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testAds1115. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testAds1115 with ZERO errors.");
		}
	}
	//  test PCS9685 "pca9685"
	if(argument.compare("all") == 0 || argument.compare("pca9685") == 0)
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testPca9685");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testPca9685(argc, argv);
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testPca9685. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testPca9685 with ZERO errors.");
		}
	}
	//  test Mcp3008 "mcp3008"
	if(argument.compare("all") == 0 || argument.compare("mcp3008") == 0) 	
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   Starting testMcp3008");
		testFailures = 0;
		if (!SetupLibrary())
			return -1;
		testReturn = testMcp3008(argc, argv);	
		if (testReturn != 0 || testFailures > 0)
		{
			testsFailed++;
			LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<(  testMcp3008. %d failures reported.", testFailures);	
		}
		else
		{
			Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :)  Finished testMcp3008 with ZERO errors.");
		}
	}
	
	TerminateLibrary();
	
	if (testsFailed > 0)
	{
		LogFormatted(LogLevelInfo, "wiringJetTests.cpp", "main", "********   FAILURE :<( %d tests failed. Press review output before you press [Enter] to quit the program.", testsFailed);
	}
	else
	{
		Log(LogLevelInfo, "wiringJetTests.cpp", "main", "********   SUCCESS :) All tests completd with no failures. Press [Enter] to quit the program.");
	}
	getchar();
}



