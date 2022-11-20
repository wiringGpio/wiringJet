#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include "wiringJetTests.h"

//  Test ISR
//  Setup:
//		- Connect three appropriate header pins together. One will be used for output, the other two for input
//		- Test verifies itself.
//
int testPinInput(int argc, char *argv[])
{
	//  hardware setup configuration
	int outputPin = 7;	
	int input1 = 11;
	int input2 = 13;
	
	pinMode(outputPin, OUTPUT);
	pinMode(input1, INPUT);
	pinMode(input2, INPUT);

	int x = 0;
	Log(LogLevelInfo, "TestPinInput.cpp", "testPinInput", "Starting pin input test");
	while (x < 5) 
	{
		digitalWrite(outputPin, 1);
		usleep(100);
		int in1 = digitalRead(input1);
		int in2 = digitalRead(input2);
		if (in1 && in2)
		{
			LogFormatted(LogLevelDebug, "TestPinInput.cpp", "testPinInput", "Pins %d and %d read HIGH.", input1, input2);
		}
		else
		{
			LogFormatted(LogLevelError, "TestPinInput.cpp", "testPinInput", "Error reading values: in1 %d in2 %d.", in1, in2);
		}
		
		usleep(100000);
		
		digitalWrite(outputPin, 0);
		usleep(100);
		
		usleep(100000);
		in1 = digitalRead(input1);
		in2 = digitalRead(input2);
		if (in1 || in2)
		{
			LogFormatted(LogLevelError, "TestPinInput.cpp", "testPinInput", "Error reading values: in1 %d in2 %d.", in1, in2);
		}
		else
		{
			LogFormatted(LogLevelDebug, "TestPinInput.cpp", "testPinInput", "Pins %d and %d read LOW.", input1, input2);
		}
		
		x++;
	}
	
	return 0;
}