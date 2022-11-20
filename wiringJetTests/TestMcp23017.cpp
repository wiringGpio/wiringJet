#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <mcp23017.h>

#include "wiringJetTests.h"


int testMcp23017(int argc, char *argv[])
{	
	//  setup MCP chip on bus 1 (pins 3/5)
	int ret = mcp23017Setup(0, 500, 0x23);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestMcp23017.cpp", "testMcp23017", "mcp23017 setup at 0x20 failed. Error code:  %d.", ret);
		return ret;
	}
	
	for (int i = 500; i < 517; i++)
		pinMode(i, OUTPUT);
		
	
		 
	//  turn the pins on and off five times in 20 seconds
	Log(LogLevelDebug, "TestMcp23017.cpp", "testMcp23017", "Turn all pins on and off five times in 20 seconds.");
	int x = 0;
	while (x < 5) {
		for (int i = 0; i < 517; i++)
			digitalWrite(i, HIGH);
		usleep(2000000);
		for (int i = 0; i < 517; i++)
			digitalWrite(i, LOW);
		usleep(2000000);
		x++;
	}

	return 0;
}