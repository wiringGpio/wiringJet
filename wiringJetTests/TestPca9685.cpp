#include <iostream>
#include <unistd.h>
#include <wiringJet.h>
#include <pca9685.h>
#include <mcp23008.h>

#include "wiringJetTests.h"


int testPca9685(int argc, char *argv[])
{	
	int fd = pca9685Setup(0, 300, 0x40,50.0);
	if (fd < 0)
	{
		/* gpio setting up failed */
		LogFormatted(LogLevelError, "TestPCA9685.cpp", "testPca9685", "pca9885Setup failed. Error code:  %d", fd);
		return fd;
	}
	
	pca9685PWMReset(fd);
		
	// set pin 4 at 50% duty cycle for 10 seconds using wiringJet pwmWrite
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Setting 50% duty cycle at 50 Hz. with pwnWrite()");
	int pinBase = 300;
	int pinNumber = 4;
	int pin = pinBase + pinNumber;
	pwmWrite(pin, 4096/2);
	usleep(10000000);
	pwmWrite(pin, 0);
	
	usleep(5000000);
	
	// set pin 4 at 25% duty cycle for 10 seconds using pca9685PWMWrite, note use the 0 index pin number not the pin base pin number
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Setting 25% duty cycle at 50 Hz. with pca9685PWMWrite()");	 
	pca9685PWMWrite(fd, pinNumber, 0, 4096 / 4) ;
	usleep(10000000);
	pwmWrite(pin, 0);
	
	usleep(5000000);
	
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Setting frequency to 250 Hz. with pcs9685Freq().");	 
	pca9685PWMFreq(fd, 250.0);
	usleep(2000000);
	
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Setting 75% duty cycle at 250 Hz.");	 
	pwmWrite(pin, (3*4096) / 4);
	usleep(10000000);
	pwmWrite(pin, 0);
	
	usleep(5000000);
	
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Setting frequency to 175 Hz. with pwmSetFrequency().");	 
	pwmSetFrequency(pin, 175.0);
	usleep(2000000);
	
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Setting 33% duty cycle at 175 Hz.");	 
	pwmWrite(pin, ( 4096) / 3);
	usleep(10000000);
	pwmWrite(pin, 0);
	
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Full on");	 
	pca9685FullOn(fd, pinNumber, 1);
	usleep(5000000);
	Log(LogLevelDebug, "TestPCA9685.cpp", "testPca9685", "Full off");	 
	pca9685FullOff(fd, pinNumber, 1);
	usleep(5000000);
	pwmWrite(pin, 0);

	return 0;
}