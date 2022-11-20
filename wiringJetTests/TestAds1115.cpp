#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <ads1115.h>
#include "wiringJetTests.h"


int testAds1115(int argc, char *argv[])
{
	int ret = ads1115Setup(1, 100, 0x48);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestAds1115.cpp", "testAds1115", "ADS1115 initialisation failed. Error code:  %d.", ret);
		return ret;
	}
	else
	{
		LogFormatted(LogLevelInfo, "TestAds1115.cpp", "testAds1115", "ADS1115 initialisation OK. Return code:  %d.", ret);
	}	
	
	//  read from pin 0 every two seconds for thirty seconds
	int pin = 100;
	int x = 0;
	while (x < 15)
	{
		auto value = analogRead(pin);
		LogFormatted(LogLevelDebug, "TestAds1115.cpp", "testAds1115", "Reading value from ADS pin %d:  %d.", pin, value);
		usleep(2000000);
		x++;
	}

	return 0;
}