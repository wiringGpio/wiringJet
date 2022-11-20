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

#pragma endregion

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

#ifdef __cplusplus
extern "C" {
#endif
	
	extern int wiringJetSetupPhys();
	extern int wiringPiSetupPhys();
	extern void wiringJetTerminate();

	extern          void pinMode(int pin, int mode);
	extern          int  digitalRead(int pin);
	extern          void digitalWrite(int pin, int value);
	extern			int pwmSetFrequency(int pin, float frequency);
	extern          void pwmWrite(int pin, int value);
	extern          int  analogRead(int pin);


	extern int  waitForInterrupt(int pin, int mS);
	extern int  wiringJetISR(int pin, int mode, void(*function)(void));
	extern int  wiringPiISR(int pin, int mode, void(*function)(void));
	
#ifdef __cplusplus
}
#endif
