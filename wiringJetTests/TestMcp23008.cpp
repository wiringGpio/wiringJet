#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <mcp23008.h>

#include "wiringJetTests.h"


int testMcp23008(int argc, char *argv[])
{	
	//  setup MCP chip on bus 1 (pins 3/5)
	int ret = mcp23008Setup(1, 200, 0x20);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23008.cpp", "testMcp23008", "mcp23008 setup at 0x20 failed. Error code:  %d.", ret);
		return ret;
	}
	int pin1 = 200;	
	pinMode(pin1, OUTPUT);
		
	//  setup two MCP chips on bus 0 (pins 27/28)
	ret = mcp23008Setup(0, 220, 0x24);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23008.cpp", "testMcp23008", "mcp23008 setup at 0x24 failed. Error code:  %d.", ret);
		return ret;
	}
	int pin2 = 221; 
	pinMode(pin2, OUTPUT);
	//
	ret = mcp23008Setup(0, 240, 0x27);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23008.cpp", "testMcp23008", "mcp23008 setup at 0x27 failed. Error code:  %d.", ret);
		return ret;
	}
	int pin3 = 242;
	pinMode(pin3, OUTPUT);
		 
	//  turn the pins on and off five times in 20 seconds
	LogFormatted(LogLevelDebug, "TestMcp23008.cpp", "testMcp23008", "Turn on pins %d %d %d on and off five times in 20 seconds.", pin1, pin2, pin3);
	int x = 0;
	while (x < 5) {
		digitalWrite(pin1, 1);
		digitalWrite(pin2, 1);
		digitalWrite(pin3, 1);
		usleep(2000000);
		digitalWrite(pin1, 0);
		digitalWrite(pin2, 0);
		digitalWrite(pin3, 0);
		usleep(2000000);
		x++;
	}

	return 0;
}