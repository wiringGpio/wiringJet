#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <algorithm>
#include "wiringJetTests.h"

using namespace std;

//  Test GPIO header pwm output
//  Setup:
//		- Connect LED to a pin 32 or 33
//		- Remember, you will need to use a transistor, darlington, or similar: Jetson pins can not drive LED directly due to level shifting.
//		- Observe pin pulsing to verify pwm is working.
//
int testGpioPwmOutput(int argc, char *argv[])
{
	//  hardware setup configuration
	int pin = 32;	
	
	pinMode(pin, PWM_OUTPUT);

	//  flash the pin 5 times a second for ten seconds
	LogFormatted(LogLevelInfo, "TestGpioPwmOutput.cpp", "testGpioPwmOutput", "Pulse pin %d for two seconds ten times.", pin);
	int x = 0;
	while (x < 10)
	{
		Log(LogLevelDebug, "TestGpioPwmOutput.cpp", "testGpioPwmOutput", "Starting pulse cycle.");
		int pwm = 0;
		while (pwm < 256)
		{
			pwm += 255 / 10;
			pwm = min(pwm, 256);
			pwmWrite(pin, pwm);
			usleep(100000);
		}
		while (pwm > 0)
		{
			pwm -= 255 / 10;
			pwm = max(pwm, 0);
			pwmWrite(pin, pwm);
			usleep(100000);
		}
		
		pwmWrite(pin, 0);
		usleep(1000000);
		x++;
	}
	
	return 0;
}