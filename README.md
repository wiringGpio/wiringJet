# wiringJet library

A C library to manage the NVIDIA Jetson Nano GPIO that implements the wiringPi interface.

This code is an amalgamation of the wiringPi library interface and device extensions, with the JETGPIO library. 
* wiringPi: [http://wiringpi.com/](http://wiringpi.com/)
* JETGPIO: [https://github.com/Rubberazer/JETGPIO](https://github.com/Rubberazer/JETGPIO)

<h2 align="left">FEATURES</h2>
The public interface for wiringJet is equivalent to wiringPi, so you can compile your existing Pi code to run on the Jetson Nano with minimal changes.

<h3>wiringPi Feature Support</h3>
Based on the wiringPi code, you can control the I2C and SPI pins of the 40 pin header. 

- I2C serial communication over pins: 27 (GEN1_I2C_SDA) & 28 (GEN1_I2C_SCL)  and 3 (GEN2_I2C_SDA) & 5 (GEN2_I2C_SCL)
- SPI serial communication over pins: 19 (SPI1_MOSI), 21 (SPI1_MISO), 23 (SPI1_SCK) & 24 (SPI1_CS0) and 37 (SPI2_MOSI), 22 (SPI2_MISO), 13 (SPI2_SCK) & 18 (SPI2_CS0)

Some of the most popular I2C and SPI device extensions are implemented, including:

- MCP 23007 and 23017 pin expanders
- PCA 9685 PWM controller
- ADS1115 analog to digital converter
- MCP3004/8 analog to digital converter
-  ... We are working on porting additional devices when we get time and hardware to test.

Notes:

Most of the wiringPi functionality  is implemented, but not all. Notable omissions include: 
- Things I am not yet sure work on the Jetson, such as pull up/down pin mode for the 40 header pins and PWM tone write.
- Serial port and DRC.
- Software PWM and and the other thread management convenience functions.

<h3>Jetson 40 Pin Header Support</h3>

Based on the JETGPIO code, you can control the GPIO pins on the 40 pin header:

- Jetson Nano (TX1) family only, so far it does not support other models such as Xavier or Orin
- GPIO control of the 40 pin header pinout as input or output. Low latency is expected writing directly to the CPU registers.
- Use input pins with event callback on rising edge, falling edge, or both. 
- PWM (hardware) control on pins 32 & 33 
- Works out of the box with the official NVIDIA Jetson Nano dev carrier board.  Also tested using the Seeed Studio A203 v2 carrier board.
  

<h2 align="left">INSTALLATION</h2>
 
Clone or download the repository into a folder on the Jetson, cd to that folder and type:
  
    cd wiringJet
    make CONFIG=Release  
    sudo make CONFIG=Release install                                             
  
To use wiringJet in your program, `#include <wiringJet.h>` and add  `-lwiringJet -lpthread` to your linker settings.

To uninstall the library, from the wiringJet/wiringJet folder:

    sudo make uninstall
  
<h2 align="left">HOW TO</h2> 
 
Consult any one one of the many available wiringPi samples for how to use the library. 

The library can be tested using the wiringJetTests project. See the comments in the different test files for details about hardware setup for teach test.
    
<h2 align="left">JETSON NANO PINOUT</h2>

The wiringJet library supports physical pin numbers only with the typical 40 pin header numbering: pin 1 is 3v3 and pin 2 is 5V.
 
You can review the NVIDIA pinmux configuration here:

https://jetsonhacks.com/nvidia-jetson-nano-j41-header-pinout/







