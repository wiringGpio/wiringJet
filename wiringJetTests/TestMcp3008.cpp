#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <mcp3004.h>
#include "wiringJetTests.h"


int testMcp3008(int argc, char *argv[])
{
	int ret = mcp3008Setup(400, 0);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp3008.cpp", "testMcp3008", "ADS1115 initialisation failed. Error code:  %d.", ret);
		return ret;
	}
	else
	{
		LogFormatted(LogLevelInfo, "TestMcp3008.cpp", "testMcp3008", "ADS1115 initialisation OK. Return code:  %d.", ret);
	}	
	
	//  read from pin 0 every two seconds for thirty seconds
	int pin = 400;
	int x = 0;
	while (x < 1000000)
	{
		for (int i = 400; i < 408; i++)
		{
			auto value = analogRead(i);
			LogFormatted(LogLevelDebug, "TestMcp3008.cpp", "testMcp3008", "Reading value from MCP pin %d:  %d.", i, value);
		}
		usleep(500000);
		x++;
	}

	return 0;
}