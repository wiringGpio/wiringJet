#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include "wiringJetTests.h"

int testPinOutput(int argc, char *argv[])
{
	int pin = 40;	
	pinMode(pin, OUTPUT);

	//  flash the pin 5 times a second for ten seconds
	LogFormatted(LogLevelDebug, "TestPinOutput.cpp", "testPinOutput", "Flash pin %d five times a second for ten seconds", pin);
	int x = 0;
	while (x < (5 * 10)) {
		digitalWrite(pin, 1);
		usleep(100000);
		digitalWrite(pin, 0);
		usleep(100000);
		x++;
	}
	
	return 0;
}