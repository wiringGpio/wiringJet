/*
 * wiringJet mcp23017.c, copied from:
 * http://wiringpi.com/
 *
 * mcp23017.c:
 *	Extend wiringPi with the MCP 23017 I2C GPIO expander chip
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 *
 *    wiringJet is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringJet is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringJet.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <pthread.h>

#include "wiringJet.h"
#include "wiringJetI2C.h"

#include "mcp23x0817.h"
#include "mcp23017.h"


/*
 * myPinMode:
 *********************************************************************************
 */

static void myPinMode(struct wiringJetNodeStruct *node, int pin, int mode)
{
	int mask, old, reg;

	pin -= node->pinBase;

	if (pin < 8)		// Bank A
	  reg  = MCP23x17_IODIRA;
	else
	{
		reg  = MCP23x17_IODIRB;
		pin &= 0x07;
	}

	mask = 1 << pin;
	old  = wiringJetI2CReadReg8(node->fd, reg);

	if (mode == OUTPUT)
		old &= (~mask);
	else
		old |=   mask;

	wiringJetI2CWriteReg8(node->fd, reg, old);
}


/*
 * myPullUpDnControl:
 *********************************************************************************
 */

static void myPullUpDnControl(struct wiringJetNodeStruct *node, int pin, int mode)
{
	int mask, old, reg;

	pin -= node->pinBase;

	if (pin < 8)		// Bank A
	  reg  = MCP23x17_GPPUA;
	else
	{
		reg  = MCP23x17_GPPUB;
		pin &= 0x07;
	}

	mask = 1 << pin;
	old  = wiringJetI2CReadReg8(node->fd, reg);

	if (mode == PUD_UP)
		old |=   mask;
	else
		old &= (~mask);

	wiringJetI2CWriteReg8(node->fd, reg, old);
}


/*
 * myDigitalWrite:
 *********************************************************************************
 */

static void myDigitalWrite(struct wiringJetNodeStruct *node, int pin, int value)
{
	int bit, old;

	pin -= node->pinBase;  	// Pin now 0-15

	bit = 1 << (pin & 7);

	if (pin < 8)			// Bank A
		{
			old = node->data2;

			if (value == LOW)
				old &= (~bit);
			else
				old |=   bit;

			wiringJetI2CWriteReg8(node->fd, MCP23x17_GPIOA, old);
			node->data2 = old;
		}
	else				// Bank B
		{
			old = node->data3;

			if (value == LOW)
				old &= (~bit);
			else
				old |=   bit;

			wiringJetI2CWriteReg8(node->fd, MCP23x17_GPIOB, old);
			node->data3 = old;
		}
}


/*
 * myDigitalRead:
 *********************************************************************************
 */

static int myDigitalRead(struct wiringJetNodeStruct *node, int pin)
{
	int mask, value, gpio;

	pin -= node->pinBase;

	if (pin < 8)		// Bank A
	  gpio  = MCP23x17_GPIOA;
	else
	{
		gpio  = MCP23x17_GPIOB;
		pin  &= 0x07;
	}

	mask  = 1 << pin;
	value = wiringJetI2CReadReg8(node->fd, gpio);

	if ((value & mask) == 0)
		return LOW ;
	else 
		return HIGH ;
}


/*
 * mcp23017Setup:
 *	Create a new instance of an MCP23017 I2C GPIO interface. We know it
 *	has 16 pins, so all we need to know here is the I2C address and the
 *	user-defined pin base.
 *********************************************************************************
 */

int mcp23017Setup(const int bus, const int pinBase, const int i2cAddress)
{
	// Create a node with 8 pins 
	  struct wiringJetNodeStruct *node = wiringJetNewNode(pinBase, 16);
	if (!node)
	{
		LogFormatted(LogLevelWarn, "mcp23017.c", "mcp23017Setup", "Invalid pin base and size. Pins %d through %d are not available.", pinBase, pinBase + 7);
		return -1;
	}
	
	int fd = wiringJetI2CSetup(bus, i2cAddress);
	if (fd < 0)
		return fd;
	
	LogFormatted(LogLevelInfo, "mcp23017.c", "mcp23017Setup", "Created MCP23017 on bus %d at address 0x%x. Pin base %d. File Descriptor (fd) %d", bus, i2cAddress, pinBase, fd);

	node->fd              = fd;
	node->pinMode         = myPinMode;
	node->pullUpDnControl = myPullUpDnControl;
	node->digitalRead     = myDigitalRead;
	node->digitalWrite    = myDigitalWrite;
	
	wiringJetI2CWriteReg8(fd, MCP23x17_IOCON, IOCON_INIT);
	node->data2           = wiringJetI2CReadReg8(fd, MCP23x17_OLATA);
	LogFormatted(LogLevelDebug, "mcp23017.c", "mcp23017Setup", "Read register MCP23x17_OLATA, node->data2 = %d", node->data2);
	node->data3           = wiringJetI2CReadReg8(fd, MCP23x17_OLATB);
	LogFormatted(LogLevelDebug, "mcp23017.c", "mcp23017Setup", "Read register MCP23x17_OLATB, node->data3 = %d", node->data2);


	return fd ;
}
