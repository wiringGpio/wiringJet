#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <mcp23008.h>

#include "wiringJetTests.h"

//  Test MCP23008
//  Setup:
//		- Set the correct bus and address for three different MCP 23008 chips
//		- Connect one pin on each to a LED
//		- Observe LEDs to verify all chips are working properly for output
//
int testMcp23008(int argc, char *argv[])
{	
	//  hardware setup configuration
	int pinBase1 = 200;
	int address1 = 0x20;
	int bus1 = 1;
	int pinBase2 = 220;
	int address2 = 0x24;
	int bus2 = 0;
	int pinBase3 = 240;
	int address3 = 0x27;
	int bus3 = 0;
	
	int pin1 = pinBase1;
	int pin2 = pinBase2; 
	int pin3 = pinBase3;
	
	//  setup MCP chip 1
	int ret = mcp23008Setup(bus1, pinBase1, address1);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23008.cpp", "testMcp23008", "mcp23008 setup at 0x20 failed. Error code:  %d.", ret);
		return ret;
	}
	//  setup chip 2
	ret = mcp23008Setup(bus2, pinBase2, address2);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23008.cpp", "testMcp23008", "mcp23008 setup at 0x24 failed. Error code:  %d.", ret);
		return ret;
	}
	//  setup chip 3
	ret = mcp23008Setup(bus3, pinBase3, address3);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23008.cpp", "testMcp23008", "mcp23008 setup at 0x27 failed. Error code:  %d.", ret);
		return ret;
	}
	
	pinMode(pin1, OUTPUT);
	pinMode(pin2, OUTPUT);
	pinMode(pin3, OUTPUT);
		 
	//  turn the pins on and off five times in 20 seconds
	LogFormatted(LogLevelInfo, "TestMcp23008.cpp", "testMcp23008", "Turn on pins %d %d %d on and off five times in 20 seconds.", pin1 - pinBase1, pin2 - pinBase2, pin3 - pinBase3);
	int x = 0;
	while (x < 5)
	{
		LogFormatted(LogLevelDebug, "TestMcp23008.cpp", "testMcp23008", "Turn on pins %d %d %d ON", pin1 - pinBase1, pin2 - pinBase2, pin3 - pinBase3);
		digitalWrite(pin1, 1);
		digitalWrite(pin2, 1);
		digitalWrite(pin3, 1);
		usleep(2000000);
		LogFormatted(LogLevelDebug, "TestMcp23008.cpp", "testMcp23008", "Turn on pins %d %d %d OFF", pin1 - pinBase1, pin2 - pinBase2, pin3 - pinBase3);
		digitalWrite(pin1, 0);
		digitalWrite(pin2, 0);
		digitalWrite(pin3, 0);
		usleep(2000000);
		x++;
	}

	return 0;
}