#pragma once

/*
 * wiringJet mcp23017.h, copied from:
 * http://wiringpi.com/ 
 * 
 * 23017.h:
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

#ifdef __cplusplus
extern "C" {
#endif

	extern int mcp23017Setup(const int bus, const int pinBase, const int i2cAddress);

#ifdef __cplusplus
}
#endif
