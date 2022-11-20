#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <algorithm>
#include "wiringJetTests.h"

using namespace std;

int testGpioPwmOutput(int argc, char *argv[])
{
	int pin = 32;	
	pinMode(pin, PWM_OUTPUT);

	//  flash the pin 5 times a second for ten seconds
	LogFormatted(LogLevelDebug, "TestGpioPwmOutput.cpp", "testGpioPwmOutput", "Pulse pin %d for two seconds ten times.", pin);
	int x = 0;
	while (x < 10)
	{
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