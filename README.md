# wiringJet library

A C library to manage the NVIDIA Jetson Nano GPIO.  This code is an amalgamation of the wiringPi library interface and device extensions with the JETGPIO library. 
* wiringPi: [http://wiringpi.com/](http://wiringpi.com/)
* JETGPIO: [https://github.com/Rubberazer/JETGPIO](https://github.com/Rubberazer/JETGPIO)

<h2 align="left">FEATURES</h2>
The public interface for wiringJet is equivalent to wiringPi, so you can compile your existing Pi code to run on the Jetson Nano with minimal changes.

Based on the wiringPi code, you can control the I2C and SPI pins of the 40 pin header. 
- I2C serial communication over pins: 27 (GEN1_I2C_SDA) & 28 (GEN1_I2C_SCL)  and 3 (GEN2_I2C_SDA) & 5 (GEN2_I2C_SCL)
- SPI serial communication over pins: 19 (SPI1_MOSI), 21 (SPI1_MISO), 23 (SPI1_SCK) & 24 (SPI1_CS0) and 37 (SPI2_MOSI), 22 (SPI2_MISO), 13 (SPI2_SCK) & 18 (SPI2_CS0)

Some of the most popular I2C and SPI device extensions are implemented, including:
- MCP 23007 and 23018 pin expanders
- PCA 9658 PWM controller
- ADS1115 analog to digital converter
- ...
- We are working on porting additional devices when we get time and hardware to test.

Based on the JETGPIO code, you can control the GPIO pins on the 40 pin header:
- JETSON NANO (TX1) family only, so far it does not support other models such as Xavier or Orin
- GPIO control of all the header pinout as input or output. Low latency is expected (it does not go through the kernel) writing directly to the CPU registers
- Catching rising or falling edges in any header pin working as input. Timestamp of the event in nanoseconds in epoch format is provided 
- PWM (hardware) control on header pins 32 & 33 
- No need of previous kernel configuration, no need to change the device tree, no need to reconfigure via /opt/nvidia/jetson-io/jetson-io.py or the like  
  

<h2 align="left">INSTALLATION</h2>
 
Clone/download the content into any folder in your JETSON, cd to that folder and type:
  
    make   
    sudo make install                                             
  
To use wiringJet in your program, `#include <wiringJet.h>` and add  `-lwiringJet -lpthread` to your linker settings.

To uninstall the library:

    sudo make uninstall
  
<h2 align="left">HOW TO</h2> 
 
Consult any one one of the many available wiringPi samples for how to use the library. 

You can also peruse the the project's test files in wiringJetTests
    
<h2 align="left">JETSON NANO PINOUT</h2>

The wiringJet library supports physical pin numbers only with the typical 40 pin header numbering; pin 1 is 3v3 and pin 2 is 5V.
 
You can check the official NVIDIA pinmux configuration for reference or if not available you can use this one below:

https://jetsonhacks.com/nvidia-jetson-nano-j41-header-pinout/

The library has been tested on a Jetson Nano using both the dev kit carrier board, and a Seeed Studio A203 v2 carrier board.








