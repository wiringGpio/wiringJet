/*
 * wiringJet.c
 * 
 *- This file is modified for NVIDIA jetson from a copy paste of:
 *   wiringPi.c:
 *   http://wiringpi.com/
 * 
 *- which as we all know is the
 *	 Arduino like Wiring library for the Raspberry Pi.
 *- by Gordon Henderson
 *	 Copyright (c) 2012-2017 Gordon Henderson
 *- to whom myself and many others owe a debt of gratitude for the excellent work he shared
 *	   
 ***********************************************************************
 * wiringJet is an implementation of the wiringPi interface for control of Jetson board GPIO pins.
 * 
 * The parts of this code not copied and modified from wiringPi, such as the Jetson board specific GPIO implementation,
 *  were copied and modified from JETGPIO https://github.com/Rubberazer/JETGPIO 
 *
 *    wiringJet is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringJet is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringJet.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */


#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "wiringJet.h"


//  Logging
//
#pragma region Logging

void wiringJetSetLoggingCallback(wiringGpioLoggingCallback function)
{
	LogFunction = function;
}
void wiringPiSetLoggingCallback(wiringGpioLoggingCallback function)
{
	wiringJetSetLoggingCallback(function);
}

void wiringJetSetLoggingLevel(wiringGpioLogLevel level)
{
	LoggingLevel = level;
}
void wiringPiSetLoggingLevel(wiringGpioLogLevel level)
{
	wiringJetSetLoggingLevel(level);
}

#pragma endregion



//  Node Management
//
#pragma region NodeManagement

struct wiringJetNodeStruct* jetGpioNodes = NULL;

/*
 *	Create a new GPIO node into the wiringPi handling system
 *********************************************************************************
 */
#define	UNU	__attribute__((unused))
/**/
static         void pinModeDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int mode)  { return ; }
static         void pullUpDnControlDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int pud)   { return ; }
static          int digitalReadDummy(UNU struct wiringJetNodeStruct *node, UNU int UNU pin)            { return 0 ; }
static         void digitalWriteDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int value) { return ; }
static         void pwmWriteDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int value) { return ; }
static         void pwmSetFrequencyDummy(UNU struct wiringJetNodeStruct *node, UNU float value) { return; }
static		  float pwmGetFrequencyDummy(UNU struct wiringJetNodeStruct *node) {return 0.0;}
static			int pwmGetRangeDummy(UNU struct wiringJetNodeStruct *node, UNU int pin) {return 0;}
static			int isHardwarePwmDummy(UNU struct wiringJetNodeStruct *node, UNU int pin) {return 0;}
static          int analogReadDummy(UNU struct wiringJetNodeStruct *node, UNU int pin)            { return 0 ; }
static         void analogWriteDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int value) { return ; }

/*
 * wiringJetNewNode:
 *      Create a new node and return pointer to the struct
 *      return null if a node can not be created
 *********************************************************************************
 */
struct wiringJetNodeStruct *wiringJetNewNode(int pinBase, int numPins)
{
	int    pin;
	struct wiringJetNodeStruct *node ;

	// Minimum pin base is 64

	 if(pinBase < 64)
	{
		Log(LogLevelWarn, "wiringJet.c", "wiringJetNewNode", "You can not create a node with pin base < 64.");
		return NULL;
	}

	// Check all pins in-case there is overlap:
	for(pin = pinBase ; pin < (pinBase + numPins) ; ++pin)
	{
		if (wiringJetFindNode(pin) != NULL) 
		{
			LogFormatted(LogLevelWarn, "wiringJet.c", "wiringJetNewNode", "Pin %d overlaps with existing definition.", pin);
			return NULL;
		}
	}

	node = (struct wiringJetNodeStruct *)calloc(sizeof(struct wiringJetNodeStruct), 1);       	// calloc zeros
	if(node == NULL)
	{
		LogFormatted(LogLevelFatal, "wiringJet.c", "wiringJetNewNode", "Unable to allocate memory %s.", strerror(errno));
		return NULL;
	}

	node->pinBase			= pinBase;
	node->pinMax			= pinBase + numPins - 1;
	node->pinMode			= pinModeDummy;
	node->pullUpDnControl	= pullUpDnControlDummy;
	node->digitalRead		= digitalReadDummy;
	node->digitalWrite		= digitalWriteDummy;
	node->pwmWrite			= pwmWriteDummy;
	node->pwmSetFrequency	= pwmSetFrequencyDummy;
	node->pwmGetFrequency	= pwmGetFrequencyDummy;
	node->pwmGetRange		= pwmGetRangeDummy;
	node->isHardwarePwm	    = isHardwarePwmDummy;
	node->analogRead		= analogReadDummy;
	node->analogWrite		= analogWriteDummy;
	node->next				= jetGpioNodes;
	jetGpioNodes			= node;

	return node ;
}

int i2cBusSpeed = I2CBUSSPEED400k;


/*
 * wiringJetFindNode:
 *      Locate our device node
 *********************************************************************************
 */
struct wiringJetNodeStruct *wiringJetFindNode(int pin)
{
	struct wiringJetNodeStruct *node = jetGpioNodes ;

	while (node != NULL)
		if ((pin >= node->pinBase) && (pin <= node->pinMax))
			return node ;
		else
			node = node->next;

	return NULL ;
};


/*
 * wiringJetGetPinBaseForNode
 * Find the pin base for a given pin number
 **/
int wiringJetGetPinBaseForNode(int pin)
{ 
	struct wiringJetNodeStruct *nodeExists = wiringJetFindNode(pin);
	if (nodeExists != NULL)
		return nodeExists->pinBase;
	else
		return -1;
}
//
int wiringPiGetPinBaseForNode(int pin)
{
	return wiringJetGetPinBaseForNode(pin);
}

extern int wiringJetGetFileDescriptorForNode(int pin)
{
	struct wiringJetNodeStruct *nodeExists = wiringJetFindNode(pin);
	if (nodeExists != NULL)
		return nodeExists->fd;
	else
		return -1;
}


extern int wiringPiGetFileDescriptorForNode(int pin)
{
	return wiringJetGetFileDescriptorForNode(pin);
}

#pragma endregion 





// JETGPIO function declarations
int gpioInitialise(void);
void gpioTerminate(int);
int gpioSetMode(unsigned gpio, unsigned mode); 
int gpioRead(unsigned gpio);
int gpioAnalogRead(unsigned pin);
int gpioWrite(unsigned gpio, unsigned level);
int gpioSetPWMfrequency(unsigned gpio, float frequency);
int gpioPWM(unsigned gpio, unsigned dutycycle);
int gpioSetISRFunc(unsigned gpio, unsigned edge, unsigned long *timestamp, void(*f)());

#pragma region PublicInterface

//  Setup
//

extern int  wiringPiSetup(void)
{
	Log(LogLevelFatal, "wiringJet.c", "wiringPiSetup", "Not implemented");
	return -1;
}

extern int  wiringPiSetupSys(void)
{
	Log(LogLevelFatal, "wiringJet.c", "wiringPiSetupSys", "Not implemented");
	return -1;
}

extern int  wiringPiSetupGpio(void)
{
	Log(LogLevelFatal, "wiringJet.c", "wiringPiSetupGpio", "Not implemented");
	return -1;
}

int wiringJetSetupPhys()
{
	return gpioInitialise();
}
//
int wiringPiSetupPhys()
{
	return wiringJetSetupPhys();
}

//  ShutDown
//
void wiringJetTerminate()
{
	gpioTerminate(0);
}
//
void wiringPiTerminate()
{
	wiringJetTerminate();
}
	

//  Pin Mode Alt
//
void pinModeAlt(int pin, int mode)
{
	Log(LogLevelFatal, "wiringJet.c", "pinModeAlt", "Not implemented");
}


//  Pin Mode
//
void pinMode(int pin, int mode)
{
	LogFormatted(LogLevelInfo, "wiringJet.c", "pinMode", "Setting pin %d to mode %d", pin, mode);
	if (pin <= 40)		
	{
		//  set the carrier board device pin mode
		gpioSetMode(pin, mode);
	}
	else
	{
		//  set the external pin if it exists
		struct wiringJetNodeStruct *node = NULL;
		if((node = wiringJetFindNode(pin)) != NULL)
		{
			node->pinMode(node, pin, mode);
		}
	}
}


//  Set pull up / down control
//		- not implemented in wiringJet
void pullUpDnControl(int pin, int pud)
{
	Log(LogLevelFatal, "wiringJet.c", "pullUpDnControl", "Not implemented");
}


//  Digital Read
//
int  digitalRead(int pin)
{
	if (pin <= 40)		
	{
		//  read the carrier board device pin
		return gpioRead(pin);
	}
	else
	{
		//  read the external pin if it exists
		struct wiringJetNodeStruct *node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			return node->digitalRead(node, pin) ;
		}
		return LOW ;
	}
}


//  Digital Write
//
void digitalWrite(int pin, int value)
{
	if (pin <= 40)
	{
		//  write to the carrier board pin
		gpioWrite(pin, value);
	}
	else
	{
		//  write to the external pin if it exists
		struct wiringJetNodeStruct* node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			node->digitalWrite(node, pin, value);
		}
	}
}



//  PWM Write
//
void pwmWrite(int pin, int value)
{
	if (pin < 40)
	{
		gpioPWM(pin, value);	
	}
	else
	{
		struct wiringJetNodeStruct* node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			node->pwmWrite(node, pin, value);
		}
	}
}


void pwmWriteUnit(int pin, float value)
{
	if (pin < 40)
	{
		int rangeValue = 255* value;
		gpioPWM(pin, rangeValue);	
	}
	else
	{
		struct wiringJetNodeStruct* node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			int rangeValue = node->pwmGetRange(node, pin) * value;
			node->pwmWrite(node, pin, rangeValue);
		}
	}
}
	

extern          void gpioClockSet(int pin, int freq)
{
	Log(LogLevelFatal, "wiringJet.c", "gpioClockSet", "Not implemented");
}
	

extern          void pwmSetMode(int mode)
{
	Log(LogLevelFatal, "wiringJet.c", "pwmSetMode", "Not implemented");
}


extern          void pwmSetClock(int divisor)
{
	Log(LogLevelFatal, "wiringJet.c", "pwmSetClock", "Not implemented");
}
	

//  PWM Set Frequency
//
float pwmFrequency0 = 0;
float pwmFrequency2 = 0;

int pwmSetFrequency(int pin, float frequency)
{
	LogFormatted(LogLevelInfo, "wiringJet.c", "pwmSetFrequency", "Setting PWM frequency for pin %d to %.1f", pin, frequency);
	
	if (pin < 40)
	{
		int ret = gpioSetPWMfrequency(pin, frequency);
		if (ret >= 0)
		{
			switch (pin)
			{
			case 31:
				pwmFrequency0 = frequency;
				break;
			case 32:
				pwmFrequency2 = frequency;
				break;
			default:
				LogFormatted(LogLevelError, "wiringJet.c", "pwmSetFrequency", "Pin %d is not a hardware PWM pin", pin);
				return 0;
			}
		}
	}
	else
	{
		struct wiringJetNodeStruct* node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			node->pwmSetFrequency(node, frequency);
		}
		return 0;
	}
}


//  PWM Get Frequency
//
float pwmGetFrequency(int pin)
{
	if (pin < 40)
	{
		switch (pin)
		{
		case 31:
			return pwmFrequency0;
		case 32:
			return pwmFrequency2;
		default:
			LogFormatted(LogLevelError, "wiringJet.c", "pwmGetFrequency", "Pin %d is not a hardware PWM pin", pin);
			return 0;
		}
	}
	else
	{
		struct wiringJetNodeStruct* node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			node->pwmGetFrequency(node);
		}
		return 0;
	}
}
		

//  PWM Set Range
//
extern          void pwmSetRange(unsigned int range)
{
	Log(LogLevelFatal, "wiringJet.c", "pwmSetRange", "Not implemented");
}

//  Get the PWM range for the given pin
//
int pwmGetRange(int pin)
{
	if (pin < 40)
	{
		switch (pin)
		{
		case 32:
		case 33:
			return 255;
		default:
			return 0;
		}
	}
	else
	{
		struct wiringJetNodeStruct* node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			return node->pwmGetRange(node, pin);
		}
		return 0;
	}
}



//  Is Hardware PWM pin
//
int pwmIsHardwarePwmPin(int pin)
{
	if (pin <= 40)
	{
		switch (pin) 
		{
		case 31:
		case 32:
			return 1;
		default:
			return 0;
		}
	}
	else
	{
		struct wiringJetNodeStruct *node = NULL;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			return node->isHardwarePwm(node, pin) ;
		}
		return 0;
	}
}


//  Analog Read
//
int  analogRead(int pin)
{
	//  read the external pin if it exists
	struct wiringJetNodeStruct *node = NULL;
	if ((node = wiringJetFindNode(pin)) != NULL)
	{
		return node->analogRead(node, pin) ;
	}
	return 0;
}


//  Analog Write
//
void analogWrite(int pin, int value)
{
	//  read the external pin if it exists
	struct wiringJetNodeStruct *node = NULL;
	if ((node = wiringJetFindNode(pin)) != NULL)
	{
		node->analogWrite(node, pin,value);
	}
}


//  Wait for Interrupt
//
int  waitForInterrupt(int pin, int mS)
{
	Log(LogLevelFatal, "wiringJet.c", "waitForInterrupt", "Not implemented");
	return -1;
}


//  Set interrupt callback
//
static unsigned long interruptTimeStamp;
int  wiringJetISR(int pin, int mode, void(*function)(void))
{
	gpioSetISRFunc(pin, mode, &interruptTimeStamp, function);
}
//
int  wiringPiISR(int pin, int mode, void(*function)(void))
{
	wiringJetISR(pin, mode, function);
}

#pragma endregion