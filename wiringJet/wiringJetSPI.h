#pragma once

/*
 * wiringJetSPI.h:, copied from
 * http://wiringpi.com/
 *
 * wiringPiSPI.h
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

#ifdef __cplusplus
extern "C" {
#endif

	int wiringJetSPIGetFd(int channel);
	int wiringJetSPIDataRW(int channel, unsigned char *data, int len);
	int wiringJetSPISetupMode(int channel, int speed, int mode);
	int wiringJetSPISetup(int channel, int speed);

#ifdef __cplusplus
}
#endif
