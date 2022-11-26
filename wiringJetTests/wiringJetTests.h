#pragma once

// Logging Callback
extern wiringGpioLoggingCallback LogFunction;

//  Log Level
extern wiringGpioLogLevel LoggingLevel;

//  Add a log to the program logging
void Log(wiringGpioLogLevel level, const char* sender, const char* function, const char* data);
void LogFormatted(wiringGpioLogLevel level, const char* sender, const char* function, const char* format, ...);


int testPinOutput(int argc, char *argv[]);

int testPinInput(int argc, char *argv[]);

int testGpioPwmOutput(int argc, char *argv[]);

int testISR(int argc, char *argv[]);

int testMcp23008(int argc, char *argv[]);

int testMcp23017(int argc, char *argv[]);

int testAds1115(int argc, char *argv[]);

int testPca9685(int argc, char *argv[]);

int testMcp3008(int argc, char *argv[]);

