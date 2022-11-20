#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include "wiringJetTests.h"

int testPinInput(int argc, char *argv[])
{
	int outputPin = 7;	
	pinMode(outputPin, OUTPUT);
	
	int input1 = 11;
	pinMode(input1, INPUT);
	int input2 = 13;
	pinMode(input2, INPUT);
	

	int x = 0;
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