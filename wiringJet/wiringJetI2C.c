/*
 * wiringJetI2C.c
 *- Copied and modified for the NVIDIA Jetson from:
 * http://wiringpi.com/
 *
 * wiringPiI2C.c:
 *	Simplified I2C access routines
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

/*
 * Gordon's Notes:
 *	The Linux I2C code is actually the same (almost) as the SMBus code.
 *	SMBus is System Management Bus - and in essentially I2C with some
 *	additional functionality added, and stricter controls on the electrical
 *	specifications, etc. however I2C does work well with it and the
 *	protocols work over both.
 *
 *	I'm directly including the SMBus functions here as some Linux distros
 *	lack the correct header files, and also some header files are GPLv2
 *	rather than the LGPL that wiringPi is released under - presumably because
 *	originally no-one expected I2C/SMBus to be used outside the kernel -
 *	however enter the Raspberry Pi with people now taking directly to I2C
 *	devices without going via the kernel...
 *
 *	This may ultimately reduce the flexibility of this code, but it won't be
 *	hard to maintain it and keep it current, should things change.
 *
 *	Information here gained from: kernel/Documentation/i2c/dev-interface
 *	as well as other online resources.
 *********************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

#include "wiringJet.h"
#include "wiringJetI2C.h"

// I2C definitions

#define I2C_SLAVE	0x0703
#define I2C_SMBUS	0x0720	/* SMBus-level access */

#define I2C_SMBUS_READ	1
#define I2C_SMBUS_WRITE	0

// SMBus transaction types

#define I2C_SMBUS_QUICK		    0
#define I2C_SMBUS_BYTE		    1
#define I2C_SMBUS_BYTE_DATA	    2 
#define I2C_SMBUS_WORD_DATA	    3
#define I2C_SMBUS_PROC_CALL	    4
#define I2C_SMBUS_BLOCK_DATA	    5
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
#define I2C_SMBUS_BLOCK_PROC_CALL   7		/* SMBus 2.0 */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

// SMBus messages

#define I2C_SMBUS_BLOCK_MAX	32	/* As specified in SMBus standard */	
#define I2C_SMBUS_I2C_BLOCK_MAX	32	/* Not specified but we use same structure */

// Structures used in the ioctl() calls

union i2c_smbus_data
{
	uint8_t  byte;
	uint16_t word;
	uint8_t  block[I2C_SMBUS_BLOCK_MAX + 2];   	// block [0] is used for length + one more for PEC
}
;

struct i2c_smbus_ioctl_data
{
	char read_write;
	uint8_t command;
	int size;
	union i2c_smbus_data *data ;
};


static inline int i2c_smbus_access(int fd, char rw, uint8_t command, int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args ;

	args.read_write = rw;
	args.command    = command;
	args.size       = size;
	args.data       = data;
	return ioctl(fd, I2C_SMBUS, &args) ;
}


int wiringJetI2CRead(int fd)
{
	union i2c_smbus_data data;

	int ret = -1;
	if ((ret = i2c_smbus_access(fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CRead", "Error reading fd %d. Error code %d.", fd, ret);
		return ret;
	}
	else
	{
		return data.byte & 0xFF ;
	}
}
//
int wiringPiI2CRead(int fd)
{
	return wiringJetI2CRead(fd);
}


int wiringJetI2CReadReg8(int fd, int reg)
{
	union i2c_smbus_data data;

	int ret = -1;
	if ((ret = i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CReadReg8", "Error reading fd %d register 0x%x.  Error code %d.", fd, reg, ret);
		return ret;
	}
	else
		return data.byte & 0xFF ;
}
//
int wiringPiI2CReadReg8(int fd, int reg)
{
	return wiringJetI2CReadReg8(fd, reg);
}


int wiringJetI2CReadReg16(int fd, int reg)
{
	union i2c_smbus_data data;

	int ret = -1;
	if ((ret = i2c_smbus_access(fd, I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CReadReg16", "Error reading fd %d register 0x%x. Error code %d.", fd, reg, ret);
		return ret;
	}
	else
		return data.word & 0xFFFF ;
}
//
int wiringPiI2CReadReg16(int fd, int reg)
{
	return wiringJetI2CReadReg16(fd, reg);
}


int wiringJetI2CWrite(int fd, int data)
{
	int ret = -1;
	if ((ret = i2c_smbus_access(fd, I2C_SMBUS_WRITE, data, I2C_SMBUS_BYTE, NULL)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CWrite", "Error writing fd %d data %d.  Error code %d.", fd, data, ret);
	}
	return ret;
}
//
int wiringPiI2CWrite(int fd, int data)
{
	return wiringJetI2CWrite(fd, data);
}


int wiringJetI2CWriteReg8(int fd, int reg, int value)
{
	union i2c_smbus_data data ;

	data.byte = value;
	int ret = -1;
	if ((ret = i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CWriteReg8", "Error writing fd %d register 0x%x data %d.  Error code %d.", fd, reg, data.byte, ret);
	}
	return ret;
}
//
int wiringPiI2CWriteReg8(int fd, int reg, int value)
{
	return wiringJetI2CWriteReg8(fd, reg, value);
}


int wiringJetI2CWriteReg16(int fd, int reg, int value)
{
	union i2c_smbus_data data ;

	data.word = value;
	int ret = -1;
	if ((ret =  i2c_smbus_access(fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CWriteReg16", "Error writing fd %d register 0x%x data %d. Error code %d.", fd, reg, data.byte, ret);
	}
	return ret;
}
//
int wiringPiI2CWriteReg16(int fd, int reg, int value)
{
	return wiringJetI2CWriteReg16(fd, reg, value);
}


int wiringJetI2CSetupInterface(const char *device, int devId)
{
	int fd;

	if ((fd = open(device, O_RDWR)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CSetupInterface", "Unable to open I2C device: %s id 0x%x. Error code %d: %s.", device, devId, fd, strerror(errno));
		return -1;
	}

	int ret = -1;
	if ((ret = ioctl(fd, I2C_SLAVE, devId)) < 0)
	{
		LogFormatted(LogLevelError, "wiringJetI2C.c", "wiringJetI2CSetupInterface", "Unable to select I2C device:%s id 0x%x. Error code %d: %s.", device, devId, ret, strerror(errno));
		return -1;
	}

	LogFormatted(LogLevelInfo, "wiringJetI2C.c", "wiringJetI2CSetupInterface", "Setup I2C interface %s 0x%x. File Descriptor (fd) %d.", device, devId, fd);
	return fd ;
}



int wiringJetI2CSetup(const int bus, const int devId)
{
	int rev;
	const char *device;

	if (bus == 0)
		device = "/dev/i2c-0";
	else
		device = "/dev/i2c-1";
	
	return wiringJetI2CSetupInterface(device, devId) ;
}
//
int wiringPiI2CSetup(const int bus, const int devId)
{
	return wiringJetI2CSetup(bus, devId);
}
