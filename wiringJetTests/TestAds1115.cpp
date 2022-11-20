#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <ads1115.h>
#include "wiringJetTests.h"

//  Test ADS1115
//  Setup:
//		- Connect ADS 1115 to the I2C bus
//		- Set the correct bus and address
//		- Provide some safe input to the pins and verify the readings are correct
//
int testAds1115(int argc, char *argv[])
{
	//  hardware setup configuration
	int bus = 1;
	int address = 0x48;
	int pinBase = 100;
	
	int ret = ads1115Setup(bus, pinBase, address);
	if (ret < 0)
	{
		LogFormatted(LogLevelError, "TestAds1115.cpp", "testAds1115", "ADS1115 initialisation failed. Error code:  %d.", ret);
		return ret;
	}
	else
	{
		LogFormatted(LogLevelInfo, "TestAds1115.cpp", "testAds1115", "ADS1115 initialisation OK. Return code:  %d.", ret);
	}	
	
	//  read from each pin every two seconds for thirty seconds
	LogFormatted(LogLevelInfo, "TestAds1115.cpp", "testAds1115", "Reading from ADS pins for the next 30 seconds.");
	int pinStart = 100;
	int x = 0;
	while (x < 15)
	{
		for (int i = pinBase; i < pinBase + 8; i++)
		{
			auto value = analogRead(i);
			LogFormatted(LogLevelDebug, "TestAds1115.cpp", "testAds1115", "Reading value from ADS pin %d:  %d.", i - pinBase, value);
		}
		usleep(2000000);
		x++;
	}

	return 0;
}