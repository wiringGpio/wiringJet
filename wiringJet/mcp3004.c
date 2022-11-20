/*
 * wiringJet mcp3004.c, copied from:
 * http://wiringpi.com/
 * 
 * mcp3004.c:
 *	Extend wiringPi with the MCP3004 SPI Analog to Digital convertor
 *	Copyright (c) 2012-2013 Gordon Henderson
 *
 *	Thanks also to "ShorTie" on IRC for some remote debugging help!
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

#include "wiringJet.h"
#include "wiringJetImplementation.h"
#include "wiringJetSPI.h"
#include "Logging.h"

#include "mcp3004.h"

/*
 * myAnalogRead:
 *	Return the analog value of the given pin
 *********************************************************************************
 */
static int myAnalogRead (struct wiringJetNodeStruct *node, int pin)
{
  unsigned char spiData [3] ;
  unsigned char chanBits ;
  int chan = pin - node->pinBase ;

  chanBits = 0b10000000 | (chan << 4) ;

  spiData [0] = 1 ;		// Start bit
  spiData [1] = chanBits ;
  spiData [2] = 0 ;

  wiringJetSPIDataRW (node->fd, spiData, 3) ;

  return ((spiData [1] << 8) | spiData [2]) & 0x3FF ;
}


/*
 * mcp3004Setup:
 *	Create a new wiringPi device node for an mcp3004 on the Nano's
 *	SPI interface.
 *********************************************************************************
 */
int mcp3004Setup(const int pinBase, int spiChannel)
{
	// Create a node with 8 pins 
	struct wiringJetNodeStruct *node = wiringJetNewNode(pinBase, 4);
	if (!node)
	{
		LogFormatted(LogLevelWarn, "mcp3004.c", "mcp3004Setup", "Invalid pin base and size. Pins %d through %d are not available.", pinBase, pinBase + 3);
		return -1;
	}

	int fd = wiringJetSPISetup(spiChannel, 1000000);
	if (fd < 0)
		return -1 ;

	LogFormatted(LogLevelInfo, "mcp3004.c", "mcp3004Setup", "Created MCP3004 on channel %d. Pin base %d. File Descriptor (fd) %d", spiChannel, pinBase, fd);

	node->fd         = spiChannel;
	node->analogRead = myAnalogRead;
	
	return fd ;
}


/*
 * mcp3008Setup:
 *	Create a new wiringPi device node for an mcp3008 on the Nano's
 *	SPI interface.
 *********************************************************************************
 */
int mcp3008Setup(const int pinBase, int spiChannel)
{
	// Create a node with 8 pins 
	struct wiringJetNodeStruct *node = wiringJetNewNode(pinBase, 8);
	if (!node)
	{
		LogFormatted(LogLevelWarn, "mcp3004.c", "mcp3008Setup", "Invalid pin base and size. Pins %d through %d are not available.", pinBase, pinBase + 3);
		return -1;
	}

	int fd = wiringJetSPISetup(spiChannel, 1000000);
	if (fd < 0)
		return fd ;

	LogFormatted(LogLevelInfo, "mcp3004.c", "mcp3008Setup", "Created MCP3008 on channel %d. Pin base %d. File Descriptor (fd) %d", spiChannel, pinBase, fd);

	node->fd         = spiChannel;
	node->analogRead = myAnalogRead;
	return fd ;
}