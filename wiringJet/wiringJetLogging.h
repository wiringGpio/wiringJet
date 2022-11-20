#pragma once

/*
 * wiringJet Logging.h
 * Simple logging framework for the wiringJet library
 * 
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


// Logging Callback
extern LoggingCallback LogFunction;

//  Log Level
extern LogLevel LoggingLevel;

//  Add a log to the program logging
void Log(LogLevel level, const char* sender, const char* function, const char* data);
void LogFormatted(LogLevel level, const char* sender, const char* function, const char* format, ...);


