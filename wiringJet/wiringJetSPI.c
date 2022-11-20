/*
 * wiringJetSPI.c:, copied from
 * http://wiringpi.com/
 *
 * wiringPiSPI.c
 *	Simplified SPI access routines
 *	Copyright (c) 2012-2015 Gordon Henderson
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
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>

#include "wiringJet.h"
#include "wiringJetImplementation.h"
#include "wiringJetI2C.h"
#include "Logging.h"


// The SPI bus parameters
static const uint8_t     spiBPW   = 8;
static const uint16_t    spiDelay = 0;

static uint32_t    spiSpeeds[2];
static int         spiFds[2];


/*
 * wiringJetSPIGetFd:
 *	Return the file-descriptor for the given channel
 *********************************************************************************
 */

int wiringJetSPIGetFd(int channel)
{
	return spiFds[channel & 1] ;
}


/*
 * wiringJetSPIDataRW:
 *	Write and Read a block of data over the SPI bus.
 *	Note the data ia being read into the transmit buffer, so will
 *	overwrite it!
 *	This is also a full-duplex operation.
 *********************************************************************************
 */

int wiringJetSPIDataRW(int channel, unsigned char *data, int len)
{
	struct spi_ioc_transfer spi ;

	channel &= 1;

	// Mentioned in spidev.h but not used in the original kernel documentation
	//	test program )-:

	  memset(&spi, 0, sizeof(spi));

	spi.tx_buf        = (unsigned long)data;
	spi.rx_buf        = (unsigned long)data;
	spi.len           = len;
	spi.delay_usecs   = spiDelay;
	spi.speed_hz      = spiSpeeds[channel];
	spi.bits_per_word = spiBPW;

	return ioctl(spiFds[channel], SPI_IOC_MESSAGE(1), &spi);
}


/*
 * wiringJetSPISetupMode:
 *	Open the SPI device, and set it up, with the mode, etc.
 *********************************************************************************
 */

int wiringJetSPISetupMode(int channel, int speed, int mode)
{
	int fd;
	char spiDev[32];

	// Mode is 0, 1, 2 or 3
	mode    &= 3;   	

	// Channel can be anything - lets hope for the best
	//  channel &= 1 ;	// Channel is 0 or 1
	snprintf(spiDev, 31, "/dev/spidev0.%d", channel);
	if ((fd = open(spiDev, O_RDWR)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetSPI.c", "wiringJetSPISetupMode", "Unable to open SPI device: %s", strerror(errno));
		return -1;
	}

	spiSpeeds[channel] = speed;
	spiFds[channel] = fd;

	// Set SPI parameters.

	  if(ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetSPI.c", "wiringJetSPISetupMode", "SPI Mode Change failure: %s\n", strerror(errno));
		return -1;
	}
	
    
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetSPI.c", "wiringJetSPISetupMode", "SPI BPW Change failure: %s\n", strerror(errno));
		return -1;
	}
    

	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)   < 0)
	{
		LogFormatted(LogLevelError, "wiringJetSPI.c", "wiringJetSPISetupMode", "SPI Speed Change failure: %s\n", strerror(errno));
		return -1;
	}
	  
	return fd;
}


/*
 * wiringJetSPISetup:
 *	Open the SPI device, and set it up, etc. in the default MODE 0
 *********************************************************************************
 */

int wiringJetSPISetup(int channel, int speed)
{
	return wiringJetSPISetupMode(channel, speed, 0);
}
