#pragma once

/*
 * wiringJet.h
 * 
 *- This file is modified for NVIDIA jetson from a copy paste of:
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


//  Logging
//
#pragma region Logging
	
typedef enum
{
	LogLevelAll,
	LogLevelVerbose,
	LogLevelTrace,
	LogLevelDebug,
	LogLevelInfo,
	LogLevelWarn, 
	LogLevelError,
	LogLevelFatal,
	LogLevelUser,
	LogLevelOff,
} LogLevel;
	
typedef struct 
{
	unsigned long long LogUnixTimeMilliseconds;
	LogLevel Level;
	int Thread;
	const char* Sender;
	const char* Function;
	const char* Data;
		
} LogEvent;
	
typedef void(*LoggingCallback)(LogEvent);
	
#ifdef __cplusplus
extern "C" {
#endif
	extern void wiringJetSetLoggingCallback(LoggingCallback);
	extern void wiringJetSetLoggingLevel(LogLevel level);
#ifdef __cplusplus
}
#endif

// Logging Callback
extern LoggingCallback LogFunction;

//  Log Level
extern LogLevel LoggingLevel;

//  Log functions
void Log(LogLevel level, const char* sender, const char* function, const char* data);
void LogFormatted(LogLevel level, const char* sender, const char* function, const char* format, ...);

#pragma endregion


//  Defines
//
#pragma region Defines

#define	INPUT			(0)
#define	OUTPUT			(1)
#define	PWM_OUTPUT		(2)

#define	LOW				(0)
#define	HIGH			(1)

#define	PUD_OFF			(0)
#define	PUD_DOWN		(1)
#define	PUD_UP			(2)

//  Note jetgpio reversal here from wiringPi
//  These defines match up with 
#define	INT_EDGE_SETUP		(0)
#define	INT_EDGE_FALLING	(2)
#define	INT_EDGE_RISING		(1)
#define	INT_EDGE_BOTH		(3)

#pragma endregion


#pragma region Interface

#ifdef __cplusplus
extern "C" {
#endif
	
	//  Setup Function
	//    You must call the setup function before using the library
	extern int wiringJetSetupPhys();
	extern int wiringPiSetupPhys();
	
	//  Tear Down Function
	//    It is recommended to call terminate function when your program exits
	extern void wiringJetTerminate();

	//  GPIO Pin Control
	
	//  Set Pin Mode
	//		INPUT, OUTPUT, or PWM_OUTPUT
	extern          void pinMode(int pin, int mode);
	
	//  Read the Pin
	//		returns LOW (0) if the pin is low, and HIGH (1) if the pin is high
	extern          int  digitalRead(int pin);
	
	// Write to the pin
	//		Set the pin to be LOW (0) or HIGH (1)
	extern          void digitalWrite(int pin, int value);
	
	//  Set PWM Frequency
	//		Set the PWM frequency for the pin, valid only for pins that support hardware PWM
	extern			int pwmSetFrequency(int pin, float frequency);
	
	//  Write PWM 
	//		Set the duty cycle, relative to the max integer duty cycle for the hardware PWM device
	//		Jetson hardware PWM pins are 0 - 255
	//		PCA9685 hardware is 0 - 4095
	//
	extern          void pwmWrite(int pin, int value);
	
	// Read analog value from the pin
	//		The Jetson does not have built in ADC.
	//		This function is used I2C or SPI devices, such as analog to digital converters
	extern          int  analogRead(int pin);

	//  Pin Edge Detection
	//		Register a function to be called when the pin changes from high to low
	//		Mode is INT_EDGE_FALLING, INT_EDGE_RISING, or INT_EDGE_BOTH
	extern int  wiringJetISR(int pin, int mode, void(*function)(void));	
	//  wiring pi api convenience wrapper
	extern int  wiringPiISR(int pin, int mode, void(*function)(void));
	

	//  Wait for Interrupt
	//		NOT IMPLEMENTED
	extern int  waitForInterrupt(int pin, int mS);
	
	
#ifdef __cplusplus
}
#endif

#pragma endregion