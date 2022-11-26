/*
 * wiringJet mcp23008.c, copied from
 * http://wiringpi.com/
 *  
 * mcp23008.c:
 *	Extend wiringPi with the MCP 23008 I2C GPIO expander chip
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
#include "mcp23008.h"


static void myPinMode(struct wiringJetNodeStruct *node, int pin, int mode)
{
	int mask, old, reg;

	reg  = MCP23x08_IODIR;
	mask = 1 << (pin - node->pinBase);
	old  = wiringJetI2CReadReg8(node->fd, reg);

	if (mode == OUTPUT)
		old &= (~mask);
	else
		old |=   mask;

	wiringJetI2CWriteReg8(node->fd, reg, old);
}


static void myPullUpDnControl(struct wiringJetNodeStruct *node, int pin, int mode)
{
	int mask, old, reg;

	reg  = MCP23x08_GPPU;
	mask = 1 << (pin - node->pinBase);

	old  = wiringJetI2CReadReg8(node->fd, reg);

	if (mode == PUD_UP)
		old |=   mask;
	else
		old &= (~mask);

	wiringJetI2CWriteReg8(node->fd, reg, old);
}


static void myDigitalWrite(struct wiringJetNodeStruct *node, int pin, int value)
{
	int bit, old;

	bit  = 1 << ((pin - node->pinBase) & 7);

	old = node->data2;
	if (value == LOW)
		old &= (~bit);
	else
		old |=   bit;

	wiringJetI2CWriteReg8(node->fd, MCP23x08_GPIO, old);
	node->data2 = old;
}


static int myDigitalRead(struct wiringJetNodeStruct *node, int pin)
{
	int mask, value;

	mask  = 1 << ((pin - node->pinBase) & 7);
	value = wiringJetI2CReadReg8(node->fd, MCP23x08_GPIO);

	if ((value & mask) == 0)
		return LOW ;
	else 
		return HIGH ;
}


int mcp23008Setup(const int bus, const int pinBase, const int i2cAddress)
{
	// Create a node with 8 pins 
	struct wiringJetNodeStruct *node = wiringJetNewNode(pinBase, 8);
	if (!node)
	{
		LogFormatted(LogLevelWarn, "mcp23008.c", "mcp23008Setup", "Invalid pin base and size. Pins %d through %d are not available.", pinBase, pinBase + 7);
		return -1;
	}
	
	int fd = wiringJetI2CSetup(bus, i2cAddress);
	if (fd < 0)
		return fd;
	
	LogFormatted(LogLevelInfo, "mcp23008.c", "mcp23008Setup", "Created MCP23008 on bus %d at address 0x%x. Pin base %d. File Descriptor (fd) %d", bus, i2cAddress, pinBase, fd);

	node->fd              = fd;
	node->pinMode         = myPinMode;
	node->pullUpDnControl = myPullUpDnControl;
	node->digitalRead     = myDigitalRead;
	node->digitalWrite    = myDigitalWrite;
		
	wiringJetI2CWriteReg8(fd, MCP23x08_IOCON, IOCON_INIT);
	node->data2           = wiringJetI2CReadReg8(fd, MCP23x08_OLAT);
	LogFormatted(LogLevelDebug, "mcp23008.c", "mcp23008Setup", "Read register MCP23x08_OLAT, node->data2 = %d", node->data2);
	
	return fd ;
}
