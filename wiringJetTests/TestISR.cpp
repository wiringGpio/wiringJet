#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include "wiringJetTests.h"

bool valueOne = false;
void PinCallbackOne()
{
	valueOne = !valueOne;
	LogFormatted(LogLevelDebug, "TestISR.cpp", "PinCallbackOne", "PinCallbackOne valueOne after change %d", valueOne);
}

bool valueTwo = true;
void PinCallbackTwo()
{
	valueTwo = !valueTwo;
	LogFormatted(LogLevelDebug, "TestISR.cpp", "PinCallbackOne", "PinCallbackTwo valueTwo after change %d", valueTwo);
}

int callbackDelayUSeconds = 1000;


//  Test ISR
//  Setup:
//		- Connect three appropriate header pins together. One will be used for output, the other two for input
//		- Test verifies itself.
//
int testISR(int argc, char *argv[])
{
	//  hardware setup configuration
	int outputPin = 7;	
	int input1 = 11;
	int input2 = 13;
	
	pinMode(outputPin, OUTPUT);
	pinMode(input1, INPUT);
	pinMode(input2, INPUT);
	
	int ret = wiringJetISR(input1, INT_EDGE_RISING, PinCallbackOne);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestISR.cpp", "testISR", "wiringJetISR failed. Error code:  %d", ret);
		return ret;
	}
	
	ret = wiringJetISR(input2, INT_EDGE_FALLING, PinCallbackTwo);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestISR.cpp", "testISR", "wiringJetISR failed. Error code:  %d", ret);
		return ret;
	}

	Log(LogLevelInfo, "TestISR.cpp", "testISR", "Starting ISR test.");
	int x = 0;
	while (x < 5) 
	{
		valueOne = false;
		valueTwo = false;
		digitalWrite(outputPin, 1);
		usleep(callbackDelayUSeconds);
		if (valueOne && !valueTwo)
		{
			LogFormatted(LogLevelDebug, "TestISR.cpp", "testISR", "Rising Edge: Pin %d callback true and pin %d callback false.", input1, input2);
		}
		else
		{
			LogFormatted(LogLevelError, "TestISR.cpp", "testISR", "Callback error: valueOne %d value2 %d.", valueOne, valueTwo);
		}
		usleep(100000);
		
		digitalWrite(outputPin, 0);
		usleep(callbackDelayUSeconds);
		if (valueOne && valueTwo)
		{
			LogFormatted(LogLevelDebug, "TestISR.cpp", "testISR", "Falling Edge: Pin %d callback true and pin %d callback true.", input1, input2);
		}
		else
		{
			LogFormatted(LogLevelError, "TestISR.cpp", "testISR", "Callback error: valueOne %d value2 %d.", valueOne, valueTwo);
		}
		usleep(1000000);
	
		x++;
	}
	
	return 0;
}