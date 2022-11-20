#pragma once

/*
 * wiringJetI2C.h, copied from
 * http://wiringpi.com/
 * 
 * wiringPiI2C.h:
 *	Simplified I2C access routines
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 *
 *    wiringJet is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringJet.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

	//  I2C functions
	extern int wiringJetI2CRead(int fd);
	extern int wiringJetI2CReadReg8(int fd, int reg);
	extern int wiringJetI2CReadReg16(int fd, int reg);	
	extern int wiringJetI2CWrite(int fd, int data);
	extern int wiringJetI2CWriteReg8(int fd, int reg, int data);
	extern int wiringJetI2CWriteReg16(int fd, int reg, int data);
	extern int wiringJetI2CSetup(const int bus, const int devId);
	
	//  wiring pi api convenience wrapper
	extern int wiringPiI2CRead(int fd); 
	extern int wiringPiI2CReadReg8(int fd, int reg);
	extern int wiringPiI2CReadReg16(int fd, int reg);
	extern int wiringPiI2CWrite(int fd, int data);
	extern int wiringPiI2CWriteReg8(int fd, int reg, int data);
	extern int wiringPiI2CWriteReg16(int fd, int reg, int data);
	extern int wiringPiI2CSetup(const int bus, const int devId);


#ifdef __cplusplus
}
#endif
