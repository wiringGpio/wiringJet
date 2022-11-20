#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <mcp23017.h>

#include "wiringJetTests.h"

//  Test MCP23017
//  Setup:
//		- Set the correct bus and address for a MCP23107 chip
//		- Connect chip pins to LEDs
//		- Observe LEDs to verify chip is working properly for output
//
int testMcp23017(int argc, char *argv[])
{	
	//  hardware setup configuration
	int pinBase = 500;
	int bus = 0;
	int address = 0x23;
	
	int ret = mcp23017Setup(bus, pinBase, address);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23017.cpp", "testMcp23017", "mcp23017 setup at 0x20 failed. Error code:  %d.", ret);
		return ret;
	}
	
	for (int i = pinBase; i < pinBase + 16; i++)
		pinMode(i, OUTPUT);
		
	
	//  turn the pins on and off five times in 20 seconds
	Log(LogLevelInfo, "TestMcp23017.cpp", "testMcp23017", "Turn all pins on and off five times in 20 seconds.");
	int x = 0;
	while (x < 5)
	{
		Log(LogLevelDebug, "TestMcp23017.cpp", "testMcp23017", "Turning pins ON.");
		for (int i = pinBase; i < pinBase + 16; i++)
			digitalWrite(i, HIGH);
		usleep(2000000);
		Log(LogLevelDebug, "TestMcp23017.cpp", "testMcp23017", "Turning pins OFF.");
		for (int i = pinBase; i < pinBase + 16; i++)
			digitalWrite(i, LOW);
		usleep(2000000);
		x++;
	}

	return 0;
}