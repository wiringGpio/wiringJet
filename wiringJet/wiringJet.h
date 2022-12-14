#pragma once

/*
 * wiringJet.h
 * 
 *- This file is modified to work with the NVIDIA jetson from a copy paste of:
 *   wiringPi.h:
 *   http://wiringpi.com/
 * 
 *- which as we all know is the
 *	 Arduino like Wiring library for the Raspberry Pi.
 *- by Gordon Henderson
 *	 Copyright (c) 2012-2017 Gordon Henderson
 *- to whom myself and many others owe a debt of gratitude for the excellent work he shared.
 *	   
 ***********************************************************************
 * wiringJet is an implementation of the wiringPi interface to control the GPIO pins of the NVIDIA Jetson Nano,
 *   as well using the I2C and SPI device extensions available in wiringPi.
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

//  Logging Defines
#include "wiringGpioLogging.h"


#ifdef __cplusplus
extern "C" {
#endif
	
//  Defines
//
#pragma region Defines

//  Pin Mode
#define	INPUT				(0)
#define	OUTPUT				(1)
#define	PWM_OUTPUT			(2)
#define	GPIO_CLOCK			(3)
#define	SOFT_PWM_OUTPUT		(4)
#define	SOFT_TONE_OUTPUT	(5)
#define	PWM_TONE_OUTPUT		(6)

//  Pin Digital Value
#define	LOW				(0)
#define	HIGH			(1)

//  Pull Up/Down Resistor Mode
#define	PUD_OFF			(0)
#define	PUD_DOWN		(1)
#define	PUD_UP			(2)

//  Interrupt Callback Edge
//		Note: jetgpio reversal here from wiringPi to match gpio.h definition of rising/fallilng
#define	INT_EDGE_SETUP		(0)
#define	INT_EDGE_FALLING	(2)
#define	INT_EDGE_RISING		(1)
#define	INT_EDGE_BOTH		(3)

//  I2C Bus Speeds
#define I2CBUSSPEED100k (0)
#define I2CBUSSPEED400k (1)
#define I2CBUSSPEED1000k (2)
	
#pragma endregion	//Defines


#pragma region PublicInterface
	
	//  Setup Function
	//    You must call the setup function before using the library
	extern int wiringJetSetupPhys();
	extern int wiringPiSetupPhys();
	
	//  not supported wiringPi setup modes
	extern int  wiringPiSetup(void);
	extern int  wiringPiSetupSys(void);
	extern int  wiringPiSetupGpio(void);
	
	//  Tear Down Function
	//    It is recommended to call terminate function when your program exits
	extern void wiringJetTerminate();
	extern void wiringPiTerminate();

	//  GPIO Pin Control
	
	//  Set Pin Mode Alt
	//		- Note: not implemented in wiringJet
	extern          void pinModeAlt(int pin, int mode);
	
	//  Set Pin Mode
	//		INPUT, OUTPUT, or PWM_OUTPUT
	extern          void pinMode(int pin, int mode);
	
	//  Set pull up / down control
	//		- Note: not implemented in wiringJet
	extern          void pullUpDnControl(int pin, int pud);
	
	//  Read the Pin
	//		returns LOW (0) if the pin is low, and HIGH (1) if the pin is high
	extern          int  digitalRead(int pin);
	
	// Write to the pin
	//		Set the pin to be LOW (0) or HIGH (1)
	extern          void digitalWrite(int pin, int value);
	
	// Read analog value from the pin
	//		The Jetson does not have built in ADC.
	//		This function is used I2C or SPI devices with this feature.
	extern          int  analogRead(int pin);

	//  Write multi bit value to the pin
	//		The Jetson does not have built in analog hardware.
	//		This function is used by I2C or SPI devices with this feature.
	extern          void analogWrite(int pin, int value);
		
	//  Write PWM 
	//		Set the duty cycle, between 0 and the max range for the PWM pin controller
	extern          void pwmWrite(int pin, int value);
	
	//  Write PWM as a unit vector
	//		Set the desired duty cycle as a unit value
	//		0.0 will be full off, 1.0 will be full on using the range of the PWM pin controller
	extern			void pwmWriteUnit(int pin, float value);
	
	
	//  PWM Hardware Control
	
	//  Set GPIO clock
	//		- Note: not implemented in wiringJet
	extern          void gpioClockSet(int pin, int freq);
	
	//  Set PWM Mode
	//		- Note: not implemented in wiringJet
	extern          void pwmSetMode(int mode);
	
	//  Set PWM Clock
	//		- Note: not implemented in wiringJet
	extern          void pwmSetClock(int divisor);
	
	//  Set PWM Frequency
	//		Set the PWM frequency for the pin, valid only for pins that support hardware PWM
	extern			int pwmSetFrequency(int pin, float frequency);
	
	//  Get PWM Frequency
	//		Get the PWM frequency for the pin, valid only for pins that support hardware PWM
	extern			float pwmGetFrequency(int pin);
	
	//  Set PWM Range
	//		- Note: not implemented in wiringJet
	extern          void pwmSetRange(unsigned int range);
	
	//  Get the PWM range for the given pin
	//
	extern			int pwmGetRange(int pin);
	
	//  Get is hardware PWM pin
	//		return 1 if is hardware PWM, otherwise 0
	extern			int pwmIsHardwarePwmPin(int pin);
	
	
	//  Software PWM
	
	//		- Not implemented yet
	extern int  softPwmCreate(int pin, int value, int range);
	extern void softPwmWrite(int pin, int value);
	extern void softPwmStop(int pin);
	//
		
	
	//  Interrupts
	
	//  Wait for Interrupt
	//		Note: - not implemented in wiringJet
	extern int  waitForInterrupt(int pin, int mS);
	
	//  Pin Edge Detection
	//		Register a function to be called when the pin changes from high to low
	//		Mode is INT_EDGE_FALLING, INT_EDGE_RISING, or INT_EDGE_BOTH
	extern int  wiringJetISR(int pin, int mode, void(*function)(void));	
	//  wiring pi api convenience wrapper
	extern int  wiringPiISR(int pin, int mode, void(*function)(void));
	

	
	
#pragma endregion	// PublicInterface
	

//  Node Management (internal workings)
//
#pragma region NodeManagement

	//  Node Struct
	//  This is used to encapsulate extension devices
	//  It is a linked list with the *next node pointing to 
	//    the previously created node, tracked by jetGpioNodes
	struct wiringJetNodeStruct
	{
		int     pinBase;
		int     pinMax;

		int          fd;       	//  file descriptor of the open device
		unsigned int data0;  	
		unsigned int data1;  	
		unsigned int data2;  	
		unsigned int data3;  	

		void(*pinMode)(struct wiringJetNodeStruct *node, int pin, int mode);
		void(*pullUpDnControl)(struct wiringJetNodeStruct *node, int pin, int mode);
		int(*digitalRead)(struct wiringJetNodeStruct *node, int pin);
		void(*digitalWrite)(struct wiringJetNodeStruct *node, int pin, int value);
		void(*pwmWrite)(struct wiringJetNodeStruct *node, int pin, int value);
		void(*pwmSetFrequency)(struct wiringJetNodeStruct *node, float frequency);
		float(*pwmGetFrequency)(struct wiringJetNodeStruct *node);
		int(*pwmGetRange)(struct wiringJetNodeStruct *node, int pin);
		int(*isHardwarePwm)(struct wiringJetNodeStruct *node, int pin);
		int(*analogRead)(struct wiringJetNodeStruct *node, int pin);
		void(*analogWrite)(struct wiringJetNodeStruct *node, int pin, int value);

		struct wiringJetNodeStruct *next ;
	};

	// Pointer to the first node in the linked list of nodes
	extern struct wiringJetNodeStruct *jetGpioNodes;
	
	//  I2C bus speed
	extern int i2cBusSpeed;

	// Manage nodes functions
	extern struct wiringJetNodeStruct *wiringJetFindNode(int pin);
	extern struct wiringJetNodeStruct *wiringJetNewNode(int pinBase, int numPins);
	
	extern int wiringJetGetPinBaseForNode(int pin);
	extern int wiringPiGetPinBaseForNode(int pin);
	
	extern int wiringJetGetFileDescriptorForNode(int pin);
	extern int wiringPiGetFileDescriptorForNode(int pin);
	
#pragma endregion	// NodeManagement
	
	
//  Logging
//
#pragma region Logging

	// Logging Callback
	extern wiringGpioLoggingCallback LogFunction;

	//  Log Level
	extern wiringGpioLogLevel LoggingLevel;

	extern void wiringJetSetLoggingCallback(wiringGpioLoggingCallback);
	extern void wiringPiSetLoggingCallback(wiringGpioLoggingCallback);
	extern void wiringJetSetLoggingLevel(wiringGpioLogLevel level);
	extern void wiringPiSetLoggingLevel(wiringGpioLogLevel level);

	//  Log functions
	void Log(wiringGpioLogLevel level, const char* sender, const char* function, const char* data);
	void LogFormatted(wiringGpioLogLevel level, const char* sender, const char* function, const char* format, ...);

#pragma endregion	// Logging
	
	
#ifdef __cplusplus
}
#endif