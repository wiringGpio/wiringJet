#pragma once

// Logging Callback
extern LoggingCallback LogFunction;

//  Log Level
extern LogLevel LoggingLevel;

//  Add a log to the program logging
void Log(LogLevel level, const char* sender, const char* function, const char* data);
void LogFormatted(LogLevel level, const char* sender, const char* function, const char* format, ...);


int testPinOutput(int argc, char *argv[]);

int testPinInput(int argc, char *argv[]);

int testGpioPwmOutput(int argc, char *argv[]);

int testISR(int argc, char *argv[]);

int testMcp23008(int argc, char *argv[]);

int testAds1115(int argc, char *argv[]);

int testPca9685(int argc, char *argv[]);

int testMcp3008(int argc, char *argv[]);

