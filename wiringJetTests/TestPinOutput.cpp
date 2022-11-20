#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include "wiringJetTests.h"

//  Test Pin Output
//  Setup:
//		- Connect LED to a pin 
//		- Remember, you will need to use a transistor, darlington, or similar: Jetson pins can not drive LED directly due to level shifting.
//		- Observe the LED to verify pin turns on and off
//
int testPinOutput(int argc, char *argv[])
{
	//  hardware setup configuration
	int pin = 40;	
	
	pinMode(pin, OUTPUT);

	//  flash the pin 5 times in ten seconds
	LogFormatted(LogLevelInfo, "TestPinOutput.cpp", "testPinOutput", "Flash pin %d five times in ten seconds.", pin);
	int x = 0;
	while (x < 5) 
	{
		Log(LogLevelDebug, "TestPinOutput.cpp", "testPinOutput", "Turning pin ON.");
		digitalWrite(pin, 1);
		usleep(1000000);
		Log(LogLevelDebug, "TestPinOutput.cpp", "testPinOutput", "Turning pin OFF.");
		digitalWrite(pin, 0);
		usleep(1000000);
		x++;
	}
	
	return 0;
}