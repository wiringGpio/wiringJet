#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <mcp3004.h>
#include "wiringJetTests.h"

//  Test MCP3008
//  Setup:
//		- Connect MCP3008 to the SPI bus
//		- Set the correct bus number
//		- Provide some safe input to the pins and verify the readings are correct
//
int testMcp3008(int argc, char *argv[])
{
	//  hardware setup configuration
	int bus = 0;
	int pinBase = 400;
	
	int ret = mcp3008Setup(pinBase, bus);
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
	LogFormatted(LogLevelInfo, "TestMcp3008.cpp", "testMcp3008", "Reading from MCP pins for the next 30 seconds.");
	int x = 0;
	while (x < 15)
	{
		for (int i = pinBase; i < pinBase + 8; i++)
		{
			auto value = analogRead(i);
			LogFormatted(LogLevelDebug, "TestMcp3008.cpp", "testMcp3008", "Reading value from MCP pin %d:  %d.", i - pinBase, value);
		}
		usleep(500000);
		x++;
	}

	return 0;
}