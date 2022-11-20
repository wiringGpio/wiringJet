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
#include "wiringJetImplementation.h"
#include "Logging.h"

#pragma region Logging

void wiringJetSetLoggingCallback(LoggingCallback function)
{
	LogFunction = function;
}

void wiringJetSetLoggingLevel(LogLevel level)
{
	LoggingLevel = level;
}

#pragma endregion


#define	UNU	__attribute__((unused))

#pragma region NodeManagement

struct wiringJetNodeStruct* jetGpioNodes = NULL;

/*
 *	Create a new GPIO node into the wiringPi handling system
 *********************************************************************************
 */

static         void pinModeDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int mode)  { return ; }
static         void pullUpDnControlDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int pud)   { return ; }
static          int digitalReadDummy(UNU struct wiringJetNodeStruct *node, UNU int UNU pin)            { return 0 ; }
static         void digitalWriteDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int value) { return ; }
static         void pwmWriteDummy(UNU struct wiringJetNodeStruct *node, UNU int pin, UNU int value) { return ; }
static         void pwmSetFrequencyDummy(UNU struct wiringJetNodeStruct *node, UNU float value) { return; }
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

	node = (struct wiringJetNodeStruct *)calloc(sizeof(struct wiringJetNodeStruct), 1);      	// calloc zeros
	if(node == NULL)
	{
		LogFormatted(LogLevelFatal, "wiringJet.c", "wiringJetNewNode", "Unable to allocate memory %s.", strerror(errno));
		return NULL;
	}

	node->pinBase          = pinBase;
	node->pinMax           = pinBase + numPins - 1;
	node->pinMode          = pinModeDummy;
	node->pullUpDnControl  = pullUpDnControlDummy;
	node->digitalRead      = digitalReadDummy;
	node->digitalWrite     = digitalWriteDummy;
	node->pwmWrite         = pwmWriteDummy;
	node->pwmSetFrequency	= pwmSetFrequencyDummy;
	node->analogRead       = analogReadDummy;
	node->analogWrite      = analogWriteDummy;
	node->next             = jetGpioNodes;
	jetGpioNodes          = node;

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
}

#pragma endregion 





// JETGPIO function declarations
int gpioInitialise(void);
void gpioTerminate(void);
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
extern int wiringJetSetupPhys()
{
	return gpioInitialise();
}


//  ShutDown
//
extern void wiringJetTerminate()
{
	gpioTerminate();
}
	

//  Pin Mode
//
void pinMode(int pin, int mode)
{
	LogFormatted(LogLevelInfo, "wiringJet.c", "pinMode", "Setting pin %d to mode %d", pin, mode);
	
	struct wiringJetNodeStruct *node = jetGpioNodes;
	if (pin <= 40)		
	{
		//  read the carrier board device pin
		gpioSetMode(pin, mode);
	}
	else
	{
		//  read the external pin if it exists
		if((node = wiringJetFindNode(pin)) != NULL)
		{
			node->pinMode(node, pin, mode);
		}
	}
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
		struct wiringJetNodeStruct *node = jetGpioNodes;
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
		struct wiringJetNodeStruct* node = jetGpioNodes;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			node->digitalWrite(node, pin, value);
		}
	}
}


//  PWM Set Frequency
//
int pwmSetFrequency(int pin, float frequency)
{
	LogFormatted(LogLevelInfo, "wiringJet.c", "pwmSetFrequency", "Setting PWM frequency for pin %d to %.1f", pin, frequency);
	
	if (pin < 40)
	{
		return gpioSetPWMfrequency(pin, frequency);
	}
	else
	{
		struct wiringJetNodeStruct* node = jetGpioNodes;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			node->pwmSetFrequency(node, frequency);
		}
		return 0;
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
		struct wiringJetNodeStruct* node = jetGpioNodes;
		if ((node = wiringJetFindNode(pin)) != NULL)
		{
			node->pwmWrite(node, pin, value);
		}
	}
}
	

//  Analog Read
//
int  analogRead(int pin)
{
	//  read the external pin if it exists
	struct wiringJetNodeStruct *node = jetGpioNodes;
	if ((node = wiringJetFindNode(pin)) != NULL)
	{
		return node->analogRead(node, pin) ;
	}
	return 0;
}


//  Wait for Interrupt
//
extern int  waitForInterrupt(int pin, int mS)
{
	Log(LogLevelError, "wiringJet.c", "waitForInterrupt", "not implemented");
	return -1;
}


//  Set interrupt callback
//
static unsigned long interruptTimeStamp;
extern int  wiringJetISR(int pin, int mode, void(*function)(void))
{
	gpioSetISRFunc(pin, mode, &interruptTimeStamp, function);
}
//
extern int  wiringPiISR(int pin, int mode, void(*function)(void))
{
	wiringJetISR(pin, mode, function);
}

#pragma endregion