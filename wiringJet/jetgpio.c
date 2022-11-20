/*
 * wiringJet jetgpio.c, copied from
 * https://github.com/Rubberazer/JETGPIO
 * 
 * JETGPIO license
 
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.
In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
For more information, please refer to <http://unlicense.org/>

*/

/* jetgpio version 0.92 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <linux/gpio.h>
#include <pthread.h>

#include "Logging.h"

//  Definitions from jetgpio.h
//
#pragma region JETGPIO_H

/* GPIO base address */

#define base_CNF 0x6000d000
#define base_PINMUX 0x70003000
#define base_CFG 0x70000000

/* PWM Control */

#define base_PWM 0x7000a000             // PWM Controller base address
#define PM3_PWM0 0x00                   // PWM0 pin 32 LCD_BL_PWM 
#define PM3_PWM2 0x20                   // PWM2 pin 33 GPIO_PE6

/* Clock and Reset Controller */

#define CAR 0x60006000                  		    // Clock and Reset Controller (CAR) base address
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 0x10 	// CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0 offset
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0 0x14 	// CLK_RST_CONTROLLER_CLK_OUT_ENB offset
#define CLK_RST_CONTROLLER_RST_DEVICES_H_0 0x8	   	// Reset the spi controllers
#define CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0 0x118 	// CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0 source clock and divider spi2
#define CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0 0x134 	// CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0 source clokc and divider spi1
#define CLK_RST_CONTROLLER_CLK_ENB_L_SET_0 0x320 	// CLK_RST_CONTROLLER_CLK_ENB_L_SET_0 offset

/* Power Management Controller */

#define base_PMC 0x7000e000                  	    // Power Management Controller (PMC) base address
#define APBDEV_PMC_PWR_DET_VAL_0 0xe4               // APBDEV_PMC_PWR_DET_VAL_0
#define APBDEV_PMC_PWR_DET_0 0x48                   // APBDEV_PMC_PWR_DET_0
#define APBDEV_PMC_PWR_DET_LATCH_0 0x4c             // APBDEV_PMC_PWR_DET_LATCH_0

/* GPIO CNF registers */

#define CNF_3 0x204                     // Pin 3 GEN2_I2C_SDA 0x6000d204
#define CNF_5 0x204                     // Pin 5 GEN2_I2C_SCL
#define CNF_7 0x60C                     // Pin 7 AUD_MCLK
#define CNF_8 0x108                     // Pin 8 UART2_TX
#define CNF_10 0x108                    // Pin 10 UART2_RX
#define CNF_11 0x108                    // Pin 11 UART2_RTS
#define CNF_12 0x204                    // Pin 12 DAP4_SCLK
#define CNF_13 0x004                    // Pin 13 SPI2_SCK
#define CNF_15 0x600                    // Pin 15 LCD_TE
#define CNF_16 0x704                    // Pin 16 SPI2_CS1
#define CNF_18 0x004                    // Pin 18 SPI2_CS0
#define CNF_19 0x008                    // Pin 19 SPI1_MOSI
#define CNF_21 0x008                    // Pin 21 SPI1_MISO
#define CNF_22 0x004                    // Pin 22 SPI2_MISO
#define CNF_23 0x008                    // Pin 23 SPI1_SCK
#define CNF_24 0x008                    // Pin 24 SPI1_CS0
#define CNF_26 0x008                    // Pin 26 SPI1_CS1
#define CNF_27 0x204                    // Pin 27 GEN1_I2C_SDA
#define CNF_28 0x204                    // Pin 28 GEN1_I2C_SCL
#define CNF_29 0x408                    // Pin 29 CAM_AF_EN
#define CNF_31 0x604                    // Pin 31 GPIO_PZ0
#define CNF_32 0x504                    // Pin 32 LCD_BL_PWM
#define CNF_33 0x100                    // Pin 33 GPIO_PE6
#define CNF_35 0x204                    // Pin 35 DAP4_FS
#define CNF_36 0x108                    // Pin 36 UART2_CTS
#define CNF_37 0x004                    // Pin 37 SPI2_MOSI
#define CNF_38 0x204                    // Pin 38 DAP4_DIN
#define CNF_40 0x204                    // Pin 40 DAP4_DOUT

/* GPIO Pinmux registers */

#define PINMUX_3 0x0c8                  // Pinmux 3 PINMUX_AUX_GEN2_I2C_SDA_0 0x700030c8
#define PINMUX_5 0x0c4                  // Pinmux 5 PINMUX_AUX_GEN2_I2C_SCL_0
#define PINMUX_7 0x180                  // Pinmux 7 PINMUX_AUX_AUD_MCLK_0
#define PINMUX_8 0x0f4                  // Pinmux 8 PINMUX_AUX_UART2_TX_0
#define PINMUX_10 0x0f8                 // Pinmux 10 PINMUX_AUX_UART2_RX_0
#define PINMUX_11 0x0fc                 // Pinmux 11 PINMUX_AUX_UART2_RTS_0
#define PINMUX_12 0x150                 // Pinmux 12 PINMUX_AUX_DAP4_SCLK_0
#define PINMUX_13 0x06c                 // Pinmux 13 PINMUX_AUX_SPI2_SCK_0
#define PINMUX_15 0x1f8                 // Pinmux 15 PINMUX_AUX_LCD_TE_0
#define PINMUX_16 0x074                 // Pinmux 16 PINMUX_AUX_SPI2_CS1_0
#define PINMUX_18 0x070                 // Pinmux 18 PINMUX_AUX_SPI2_CS0_0
#define PINMUX_19 0x050                 // Pinmux 19 PINMUX_AUX_SPI1_MOSI_0
#define PINMUX_21 0x054                 // Pinmux 21 PINMUX_AUX_SPI1_MISO_0
#define PINMUX_22 0x068                 // Pinmux 22 PINMUX_AUX_SPI2_MISO_0
#define PINMUX_23 0x058                 // Pinmux 23 PINMUX_AUX_SPI1_SCK_0
#define PINMUX_24 0x05c                 // Pinmux 24 PINMUX_AUX_SPI1_CS0_0
#define PINMUX_26 0x060                 // Pinmux 26 PINMUX_AUX_SPI1_CS1_0
#define PINMUX_27 0x0c0                 // Pinmux 27 PINMUX_AUX_GEN1_I2C_SDA_0
#define PINMUX_28 0x0bc                 // Pinmux 28 PINMUX_AUX_GEN1_I2C_SCL_0
#define PINMUX_29 0x1e4                 // Pinmux 29 PINMUX_AUX_CAM_AF_EN_0
#define PINMUX_31 0x27c                 // Pinmux 31 PINMUX_AUX_GPIO_PZ0_0
#define PINMUX_32 0x1fc                 // Pinmux 32 PINMUX_AUX_LCD_BL_PWM_0
#define PINMUX_33 0x248                 // Pinmux 33 PINMUX_AUX_GPIO_PE6_0
#define PINMUX_35 0x144                 // Pinmux 35 PINMUX_AUX_DAP4_FS_0
#define PINMUX_36 0x100                 // Pinmux 36 PINMUX_AUX_UART2_CTS_0
#define PINMUX_37 0x064                 // Pinmux 37 PINMUX_AUX_SPI2_MOSI_0
#define PINMUX_38 0x148                 // Pinmux 38 PINMUX_AUX_DAP4_DIN_0
#define PINMUX_40 0x14c                 // Pinmux 40 PINMUX_AUX_DAP4_DOUT_0

/* GPIO Cfg registers */

#define CFG_3 0x9b4                     // Config 3 GEN2_I2C_SDA_CFG 0x700009b4
#define CFG_5 0x9b0                     // Config 5 GEN2_I2C_SCL_CFG
#define CFG_7 0x8f4                     // Config 7 AUD_MCLK_CFG
#define CFG_8 0xb38                     // Config 8 UART2_TX_CFG
#define CFG_10 0xb34                    // Config 10 UART2_RX_CFG
#define CFG_11 0xb30                    // Config 11 UART2_RTS_CFG
#define CFG_12 0x980                    // Config 12 DAP4_SCLK_CFG
#define CFG_13 0xaf8                    // Config 13 SPI2_SCK_CFG
#define CFG_15 0xa44                    // Config 15 LCD_TE_CFG
#define CFG_16 0xaec                    // Config 16 SPI2_CS1_CFG
#define CFG_18 0xae8                    // Config 18 SPI2_CS0_CFG
#define CFG_19 0xae0                    // Config 19 SPI1_MOSI_CFG
#define CFG_21 0xadc                    // Config 21 SPI1_MISO_CFG
#define CFG_22 0xaf0                    // Config 22 SPI2_MISO_CFG
#define CFG_23 0xae4                    // Config 23 SPI1_SCK_CFG
#define CFG_24 0xad4                    // Config 24 SPI1_CS0_CFG
#define CFG_26 0xad8                    // Config 26 SPI1_CS1_CFG
#define CFG_27 0x9ac                    // Config 27 GEN1_I2C_SDA_CFG
#define CFG_28 0x9a8                    // Config 28 GEN1_I2C_SCL_CFG
#define CFG_29 0x92c                    // Config 29 CAM_AF_EN_CFG
#define CFG_31 0x9fc                    // Config 31 GPIO_PZ0_CFG
#define CFG_32 0xa34                    // Config 32 LCD_BL_PWM_CFG
#define CFG_33 0x9c8                    // Config 33 GPIO_PE6_CFG
#define CFG_35 0x97c                    // Config 35 DAP4_FS_CFG
#define CFG_36 0xb2c                    // Config 36 UART2_CTS_CFG
#define CFG_37 0xaf4                    // Config 37 SPI2_MOSI_CFG
#define CFG_38 0x974                    // Config 38 DAP4_DIN_CFG
#define CFG_40 0x978                    // Config 40 DAP4_DOUT_CFG

/* Typical values Pinmux & Cfg registers */

#define PINMUX_IN 0x00000040            // Typical for pinmux register as input
#define PINMUX_OUT 0x00000400           // Typical for pinmux register as output
#define PINMUX_OUT1 0x0000e200          // Typical for pinmux spi pins register as output 
#define CFG_IN 0x00000000               // Typical for config register as input
#define CFG_OUT 0x01F1F000              // Typical for config register as output
#define CFG_OUT1 0xF0000000             // Typical for config spi pins register as output

/* Define typical input/output */



/* Define the typical interruption trigger */

#define RISING_EDGE 1
#define FALLING_EDGE 2
#define EITHER_EDGE 3

/* i2c definitions */

#define I2C_CLOSED   0
#define I2C_RESERVED 1
#define I2C_OPENED   2

/* SPI definitions */

#define SPI_CLOSED   0
#define SPI_RESERVED 1
#define SPI_OPENED   2

typedef struct {
	uint32_t CNF[4];
	uint32_t OE[4];
	uint32_t OUT[4];
	uint32_t IN[4];
	uint32_t INT_STA[4];
	uint32_t INT_ENB[4];
	uint32_t INT_LVL[4];
	uint32_t INT_CLR[4];
} GPIO_CNF;

typedef struct {
	uint32_t pin3;
	uint32_t pin5;
	uint32_t pin7;
	uint32_t pin8;
	uint32_t pin10;
	uint32_t pin11;
	uint32_t pin12;
	uint32_t pin13;
	uint32_t pin15;
	uint32_t pin16;
	uint32_t pin18;
	uint32_t pin19;
	uint32_t pin21;
	uint32_t pin22;
	uint32_t pin23;
	uint32_t pin24;
	uint32_t pin26;
	uint32_t pin27;
	uint32_t pin28;
	uint32_t pin29;
	uint32_t pin31;
	uint32_t pin32;
	uint32_t pin33;
	uint32_t pin35;
	uint32_t pin36;
	uint32_t pin37;
	uint32_t pin38;
	uint32_t pin40;
} GPIO_CNF_Init;

typedef struct {
	uint32_t stat_reg;
	uint32_t gpio;
	uint32_t edge;
	uint32_t gpio_offset;
	uint64_t *timestamp;
	void(*f)();
} ISRFunc;

typedef ISRFunc *PISRFunc;

typedef struct {
	uint32_t PWM_0[4];
	uint32_t PWM_1[4];
	uint32_t PWM_2[4];
	uint32_t PWM_3[4];
} GPIO_PWM;


int gpioPWM(unsigned gpio, unsigned dutycycle);

#pragma endregion


//  Local Variables
//
#pragma region LocalVariables

static int fd_GPIO;

static volatile GPIO_CNF_Init pin_CNF;
static volatile GPIO_CNF_Init pin_OE;
static volatile GPIO_CNF_Init pin_ENB;
static volatile GPIO_CNF_Init pin_LVL;
static volatile GPIO_CNF_Init pin_MUX;
static volatile GPIO_CNF_Init pin_CFG;
PISRFunc ISRFunc_CFG[41];

static volatile GPIO_PWM pinPWM_Init;
static volatile GPIO_PWM *pinPWM;


static volatile GPIO_CNF *pin3;
static volatile GPIO_CNF *pin5;
static volatile GPIO_CNF *pin7;
static volatile GPIO_CNF *pin8;
static volatile GPIO_CNF *pin10;
static volatile GPIO_CNF *pin11;
static volatile GPIO_CNF *pin12;
static volatile GPIO_CNF *pin13;
static volatile GPIO_CNF *pin15;
static volatile GPIO_CNF *pin16;
static volatile GPIO_CNF *pin18;
static volatile GPIO_CNF *pin19;
static volatile GPIO_CNF *pin21;
static volatile GPIO_CNF *pin22;
static volatile GPIO_CNF *pin23;
static volatile GPIO_CNF *pin24;
static volatile GPIO_CNF *pin26;
static volatile GPIO_CNF *pin27;
static volatile GPIO_CNF *pin28;
static volatile GPIO_CNF *pin29;
static volatile GPIO_CNF *pin31;
static volatile GPIO_CNF *pin32;
static volatile GPIO_CNF *pin33;
static volatile GPIO_CNF *pin35;
static volatile GPIO_CNF *pin36;
static volatile GPIO_CNF *pin37;
static volatile GPIO_CNF *pin38;
static volatile GPIO_CNF *pin40;

static volatile uint32_t *pinmux3;
static volatile uint32_t *pinmux5;
static volatile uint32_t *pinmux7;
static volatile uint32_t *pinmux8;
static volatile uint32_t *pinmux10;
static volatile uint32_t *pinmux11;
static volatile uint32_t *pinmux12;
static volatile uint32_t *pinmux13;
static volatile uint32_t *pinmux15;
static volatile uint32_t *pinmux16;
static volatile uint32_t *pinmux18;
static volatile uint32_t *pinmux19;
static volatile uint32_t *pinmux21;
static volatile uint32_t *pinmux22;
static volatile uint32_t *pinmux23;
static volatile uint32_t *pinmux24;
static volatile uint32_t *pinmux26;
static volatile uint32_t *pinmux27;
static volatile uint32_t *pinmux28;
static volatile uint32_t *pinmux29;
static volatile uint32_t *pinmux31;
static volatile uint32_t *pinmux32;
static volatile uint32_t *pinmux33;
static volatile uint32_t *pinmux35;
static volatile uint32_t *pinmux36;
static volatile uint32_t *pinmux37;
static volatile uint32_t *pinmux38;
static volatile uint32_t *pinmux40;

static volatile uint32_t *pincfg3;
static volatile uint32_t *pincfg5;
static volatile uint32_t *pincfg7;
static volatile uint32_t *pincfg8;
static volatile uint32_t *pincfg10;
static volatile uint32_t *pincfg11;
static volatile uint32_t *pincfg12;
static volatile uint32_t *pincfg13;
static volatile uint32_t *pincfg15;
static volatile uint32_t *pincfg16;
static volatile uint32_t *pincfg18;
static volatile uint32_t *pincfg19;
static volatile uint32_t *pincfg21;
static volatile uint32_t *pincfg22;
static volatile uint32_t *pincfg23;
static volatile uint32_t *pincfg24;
static volatile uint32_t *pincfg26;
static volatile uint32_t *pincfg27;
static volatile uint32_t *pincfg28;
static volatile uint32_t *pincfg29;
static volatile uint32_t *pincfg31;
static volatile uint32_t *pincfg32;
static volatile uint32_t *pincfg33;
static volatile uint32_t *pincfg35;
static volatile uint32_t *pincfg36;
static volatile uint32_t *pincfg37;
static volatile uint32_t *pincfg38;
static volatile uint32_t *pincfg40;

static volatile uint32_t *controller_clk_out_enb_l;
static volatile uint32_t *controller_clk_out_enb_h;
static volatile uint32_t *controller_clk_out_enb_l_set;
static volatile uint32_t *controller_rst_devices_h;
static volatile uint32_t *clk_source_spi1;
static volatile uint32_t *clk_source_spi2;

static volatile uint32_t *apbdev_pmc_pwr_det_val;

static void *baseCNF;

static void *basePINMUX;

static void *baseCFG;

static void *basePWM;

static void *baseCAR;

static void *basePMC;

static volatile unsigned global_int;
static pthread_t callThd[28];
static pthread_attr_t attr;
static int pth_err;
static void *status_thread;
static int thread_n = 0;

#pragma endregion //LocalVariables


//  Setup and Tear Down
//
#pragma region SetupTeardown

int gpioInitialise(void)
{
	int status = 1;
	//  Getting the page size
	int pagesize = sysconf(_SC_PAGESIZE);     //getpagesize();	
	LogFormatted(LogLevelInfo, "jetgpio.c", "gpioInitialize", "Initializing GPIO with page size %d", pagesize);
	
	//  read physical memory (needs root)
	fd_GPIO = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd_GPIO < 0) 
	{
		perror("/dev/mem");
		Log(LogLevelFatal, "jetgpio.c", "gpioInitialize", "Please run this program as root.");
		return -1;
	}
	//  Mapping GPIO_CNF
	baseCNF = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CNF);
	if (baseCNF == MAP_FAILED) 
	{
		return -2;
	}
    
	//  Mapping GPIO_PINMUX
	basePINMUX = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PINMUX);
	if (basePINMUX == MAP_FAILED) 
	{
		return -3;
	}
    
	//  Mapping GPIO_CFG
	baseCFG = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_CFG);
	if (baseCFG == MAP_FAILED) 
	{
		return -4;
	}
    
	//  Mapping GPIO_PWM
	basePWM = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PWM);
	if (basePWM == MAP_FAILED) 
	{
		return -5;
	}
    
	//  Mapping CAR
	baseCAR = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, CAR);
	if (baseCAR == MAP_FAILED) 
	{
		return -6;
	}

	//  Mapping PMC
	basePMC = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_GPIO, base_PMC);
	if (basePMC == MAP_FAILED) 
	{
		return -7;
	}  
    
	// Pointer to CNF3
	pin3 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_3);
	pin_CNF.pin3 = pin3->CNF[0];
	pin_OE.pin3 =  pin3->OE[0];
	pin_ENB.pin3 =  pin3->INT_ENB[0];
	pin_LVL.pin3 =  pin3->INT_LVL[0];
    
	// Pointer to PINMUX3
	pinmux3 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_3);
	pin_MUX.pin3 = *pinmux3;
    
	// Pointer to PINCFG3
	pincfg3 = (uint32_t volatile *)((char *)baseCFG + CFG_3);
	pin_CFG.pin3 = *pincfg3;
    
	// Pointer to CNF5
	pin5 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_5);
	pin_CNF.pin5 = pin5->CNF[0];
	pin_OE.pin5 =  pin5->OE[0];
	pin_ENB.pin5 =  pin5->INT_ENB[0];
	pin_LVL.pin5 =  pin5->INT_LVL[0];
    
	// Pointer to PINMUX5
	pinmux5 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_5);
	pin_MUX.pin5 = *pinmux5;
    
	// Pointer to PINCFG5
	pincfg5 = (uint32_t volatile *)((char *)baseCFG + CFG_5);
	pin_CFG.pin5 = *pincfg5;
    
	// Pointer to CNF7
	pin7 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_7);
	pin_CNF.pin7 = pin7->CNF[0];
	pin_OE.pin7 =  pin7->OE[0];
	pin_ENB.pin7 =  pin7->INT_ENB[0];
	pin_LVL.pin7 =  pin7->INT_LVL[0];
    
	// Pointer to PINMUX7
	pinmux7 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_7);
	pin_MUX.pin7 = *pinmux7;
    
	// Pointer to PINCFG7
	pincfg7 = (uint32_t volatile *)((char *)baseCFG + CFG_7);
	pin_CFG.pin7 = *pincfg7;
    
	// Pointer to CNF8
	pin8 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_8);
	pin_CNF.pin8 = pin8->CNF[0];
	pin_OE.pin8 =  pin8->OE[0];
	pin_ENB.pin8 =  pin8->INT_ENB[0];
	pin_LVL.pin8 =  pin8->INT_LVL[0];
    
	// Pointer to PINMUX8
	pinmux8 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_8);
	pin_MUX.pin8 = *pinmux8;
    
	// Pointer to PINCFG8
	pincfg8 = (uint32_t volatile *)((char *)baseCFG + CFG_8);
	pin_CFG.pin8 = *pincfg8;
    
	// Pointer to CNF10
	pin10 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_10);
	pin_CNF.pin10 = pin10->CNF[0];
	pin_OE.pin10 =  pin10->OE[0];
	pin_ENB.pin10 =  pin10->INT_ENB[0];
	pin_LVL.pin10 =  pin10->INT_LVL[0];
    
	// Pointer to PINMUX10
	pinmux10 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_10);
	pin_MUX.pin10 = *pinmux10;
    
	// Pointer to PINCFG10
	pincfg10 = (uint32_t volatile *)((char *)baseCFG + CFG_10);
	pin_CFG.pin10 = *pincfg10;
    
	// Pointer to CNF11
	pin11 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_11);
	pin_CNF.pin11 = pin11->CNF[0];
	pin_OE.pin11 =  pin11->OE[0];
	pin_ENB.pin11 =  pin11->INT_ENB[0];
	pin_LVL.pin11 =  pin11->INT_LVL[0];
   
	// Pointer to PINMUX11
	pinmux11 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_11);
	pin_MUX.pin11 = *pinmux11;
    
	// Pointer to PINCFG11
	pincfg11 = (uint32_t volatile *)((char *)baseCFG + CFG_11);
	pin_CFG.pin11 = *pincfg11;
    
	// Pointer to CNF12
	pin12 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_12);
	pin_CNF.pin12 = pin12->CNF[0];
	pin_OE.pin12 =  pin12->OE[0];
	pin_ENB.pin12 =  pin12->INT_ENB[0];
	pin_LVL.pin12 =  pin12->INT_LVL[0];
    
	// Pointer to PINMUX12
	pinmux12 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_12);
	pin_MUX.pin12 = *pinmux12;
    
	// Pointer to PINCFG12
	pincfg12 = (uint32_t volatile *)((char *)baseCFG + CFG_12);
	pin_CFG.pin12 = *pincfg12;
    
	// Pointer to CNF13
	pin13 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_13);
	pin_CNF.pin13 = pin13->CNF[0];
	pin_OE.pin13 =  pin13->OE[0];
	pin_ENB.pin13 =  pin13->INT_ENB[0];
	pin_LVL.pin13 =  pin13->INT_LVL[0];
    
	// Pointer to PINMUX13
	pinmux13 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_13);
	pin_MUX.pin13 = *pinmux13;
    
	// Pointer to PINCFG13
	pincfg13 = (uint32_t volatile *)((char *)baseCFG + CFG_13);
	pin_CFG.pin13 = *pincfg13;
    
	// Pointer to CNF15
	pin15 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_15);
	pin_CNF.pin15 = pin15->CNF[0];
	pin_OE.pin15 =  pin15->OE[0];
	pin_ENB.pin15 =  pin15->INT_ENB[0];
	pin_LVL.pin15 =  pin15->INT_LVL[0];
    
	// Pointer to PINMUX15
	pinmux15 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_15);
	pin_MUX.pin15 = *pinmux15;
    
	// Pointer to PINCFG15
	pincfg15 = (uint32_t volatile *)((char *)baseCFG + CFG_15);
	pin_CFG.pin15 = *pincfg15;
    
	// Pointer to CNF16
	pin16 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_16);
	pin_CNF.pin16 = pin16->CNF[0];
	pin_OE.pin16 =  pin16->OE[0];
	pin_ENB.pin16 =  pin16->INT_ENB[0];
	pin_LVL.pin16 =  pin16->INT_LVL[0];
    
	// Pointer to PINMUX16
	pinmux16 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_16);
	pin_MUX.pin16 = *pinmux16;
   
	// Pointer to PINCFG16
	pincfg16 = (uint32_t volatile *)((char *)baseCFG + CFG_16);
	pin_CFG.pin16 = *pincfg16;
    
	// Pointer to CNF18
	pin18 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_18);
	pin_CNF.pin18 = pin18->CNF[0];
	pin_OE.pin18 =  pin18->OE[0];
	pin_ENB.pin18 =  pin18->INT_ENB[0];
	pin_LVL.pin18 =  pin18->INT_LVL[0];
    
	// Pointer to PINMUX18
	pinmux18 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_18);
	pin_MUX.pin18 = *pinmux18;
    
	// Pointer to PINCFG18
	pincfg18 = (uint32_t volatile *)((char *)baseCFG + CFG_18);
	pin_CFG.pin18 = *pincfg18;
    
	// Pointer to CNF19
	pin19 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_19);
	pin_CNF.pin19 = pin19->CNF[0];
	pin_OE.pin19 =  pin19->OE[0];
	pin_ENB.pin19 =  pin19->INT_ENB[0];
	pin_LVL.pin19 =  pin19->INT_LVL[0];
    
	// Pointer to PINMUX19
	pinmux19 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_19);
	pin_MUX.pin19 = *pinmux19;
    
	// Pointer to PINCFG19
	pincfg19 = (uint32_t volatile *)((char *)baseCFG + CFG_19);
	pin_CFG.pin19 = *pincfg19;
    
	// Pointer to CNF21
	pin21 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_21);
	pin_CNF.pin21 = pin21->CNF[0];
	pin_OE.pin21 =  pin21->OE[0];
	pin_ENB.pin21 =  pin21->INT_ENB[0];
	pin_LVL.pin21 =  pin21->INT_LVL[0];
    
	// Pointer to PINMUX21
	pinmux21 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_21);
	pin_MUX.pin21 = *pinmux21;
    
	// Pointer to PINCFG21
	pincfg21 = (uint32_t volatile *)((char *)baseCFG + CFG_21);
	pin_CFG.pin21 = *pincfg21;
    
	// Pointer to CNF22
	pin22 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_22);
	pin_CNF.pin22 = pin22->CNF[0];
	pin_OE.pin22 =  pin22->OE[0];
	pin_ENB.pin22 =  pin22->INT_ENB[0];
	pin_LVL.pin22 =  pin22->INT_LVL[0];
    
	// Pointer to PINMUX22
	pinmux22 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_22);
	pin_MUX.pin22 = *pinmux22;
    
	// Pointer to PINCFG22
	pincfg22 = (uint32_t volatile *)((char *)baseCFG + CFG_22);
	pin_CFG.pin22 = *pincfg22;
    
	// Pointer to CNF23
	pin23 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_23);
	pin_CNF.pin23 = pin23->CNF[0];
	pin_OE.pin23 =  pin23->OE[0];
	pin_ENB.pin23 =  pin23->INT_ENB[0];
	pin_LVL.pin23 =  pin23->INT_LVL[0];
    
	// Pointer to PINMUX23
	pinmux23 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_23);
	pin_MUX.pin23 = *pinmux23;
    
	// Pointer to PINCFG23
	pincfg23 = (uint32_t volatile *)((char *)baseCFG + CFG_23);
	pin_CFG.pin23 = *pincfg23;
    
	// Pointer to CNF24
	pin24 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_24);
	pin_CNF.pin24 = pin24->CNF[0];
	pin_OE.pin24 =  pin24->OE[0];
	pin_ENB.pin24 =  pin24->INT_ENB[0];
	pin_LVL.pin24 =  pin24->INT_LVL[0];
    
	// Pointer to PINMUX24
	pinmux24 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_24);
	pin_MUX.pin24 = *pinmux24;
    
	// Pointer to PINCFG24
	pincfg24 = (uint32_t volatile *)((char *)baseCFG + CFG_24);
	pin_CFG.pin24 = *pincfg24;
    
	// Pointer to CNF26
	pin26 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_26);
	pin_CNF.pin26 = pin26->CNF[0];
	pin_OE.pin26 =  pin26->OE[0];
	pin_ENB.pin26 =  pin26->INT_ENB[0];
	pin_LVL.pin26 =  pin26->INT_LVL[0];
    
	// Pointer to PINMUX26
	pinmux26 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_26);
	pin_MUX.pin26 = *pinmux26;
    
	// Pointer to PINCFG26
	pincfg26 = (uint32_t volatile *)((char *)baseCFG + CFG_26);
	pin_CFG.pin26 = *pincfg26;
    
	// Pointer to CNF27
	pin27 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_27);
	pin_CNF.pin27 = pin27->CNF[0];
	pin_OE.pin27 =  pin27->OE[0];
	pin_ENB.pin27 =  pin27->INT_ENB[0];
	pin_LVL.pin27 =  pin27->INT_LVL[0];
    
	// Pointer to PINMUX27
	pinmux27 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_27);
	pin_MUX.pin27 = *pinmux27;
    
	// Pointer to PINCFG27
	pincfg27 = (uint32_t volatile *)((char *)baseCFG + CFG_27);
	pin_CFG.pin27 = *pincfg27;
    
	// Pointer to CNF28
	pin28 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_28);
	pin_CNF.pin28 = pin28->CNF[0];
	pin_OE.pin28 =  pin28->OE[0];
	pin_ENB.pin28 =  pin28->INT_ENB[0];
	pin_LVL.pin28 =  pin28->INT_LVL[0];
    
	// Pointer to PINMUX28
	pinmux28 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_28);
	pin_MUX.pin28 = *pinmux28;
    
	// Pointer to PINCFG28
	pincfg28 = (uint32_t volatile *)((char *)baseCFG + CFG_28);
	pin_CFG.pin28 = *pincfg28;
    
	// Pointer to CNF29
	pin29 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_29);
	pin_CNF.pin29 = pin29->CNF[0];
	pin_OE.pin29 =  pin29->OE[0];
	pin_ENB.pin29 =  pin29->INT_ENB[0];
	pin_LVL.pin29 =  pin29->INT_LVL[0];
    
	// Pointer to PINMUX29
	pinmux29 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_29);
	pin_MUX.pin29 = *pinmux29;
    
	// Pointer to PINCFG29
	pincfg29 = (uint32_t volatile *)((char *)baseCFG + CFG_29);
	pin_CFG.pin29 = *pincfg29;
    
	// Pointer to CNF31
	pin31 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_31);
	pin_CNF.pin31 = pin31->CNF[0];
	pin_OE.pin31 =  pin31->OE[0];
	pin_ENB.pin31 =  pin31->INT_ENB[0];
	pin_LVL.pin31 =  pin31->INT_LVL[0];
    
	// Pointer to PINMUX31
	pinmux31 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_31);
	pin_MUX.pin31 = *pinmux31;
    
	// Pointer to PINCFG31
	pincfg31 = (uint32_t volatile *)((char *)baseCFG + CFG_31);
	pin_CFG.pin31 = *pincfg31;
    
	// Pointer to CNF32
	pin32 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_32);
	pin_CNF.pin32 = pin32->CNF[0];
	pin_OE.pin32 =  pin32->OE[0];
	pin_ENB.pin32 =  pin32->INT_ENB[0];
	pin_LVL.pin32 =  pin32->INT_LVL[0];
    
	// Pointer to PINMUX32
	pinmux32 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_32);
	pin_MUX.pin32 = *pinmux32;
   
	// Pointer to PINCFG32
	pincfg32 = (uint32_t volatile *)((char *)baseCFG + CFG_32);
	pin_CFG.pin32 = *pincfg32;
    
	// Pointer to CNF33
	pin33 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_33);
	pin_CNF.pin33 = pin33->CNF[0];
	pin_OE.pin33 =  pin33->OE[0];
	pin_ENB.pin33 =  pin33->INT_ENB[0];
	pin_LVL.pin33 =  pin33->INT_LVL[0];
    
	// Pointer to PINMUX33
	pinmux33 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_33);
	pin_MUX.pin33 = *pinmux33;
    
	// Pointer to PINCFG33
	pincfg33 = (uint32_t volatile *)((char *)baseCFG + CFG_33);
	pin_CFG.pin33 = *pincfg33;
    
	// Pointer to CNF35
	pin35 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_35);
	pin_CNF.pin35 = pin35->CNF[0];
	pin_OE.pin35 =  pin35->OE[0];
	pin_ENB.pin35 =  pin35->INT_ENB[0];
	pin_LVL.pin35 =  pin35->INT_LVL[0];
    
	// Pointer to PINMUX35
	pinmux35 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_35);
	pin_MUX.pin35 = *pinmux35;
    
	// Pointer to PINCFG35
	pincfg35 = (uint32_t volatile *)((char *)baseCFG + CFG_35);
	pin_CFG.pin35 = *pincfg35;
    
	// Pointer to CNF36
	pin36 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_36);
	pin_CNF.pin36 = pin36->CNF[0];
	pin_OE.pin36 =  pin36->OE[0];
	pin_ENB.pin36 =  pin36->INT_ENB[0];
	pin_LVL.pin36 =  pin36->INT_LVL[0];
    
	// Pointer to PINMUX36
	pinmux36 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_36);
	pin_MUX.pin36 = *pinmux36;
    
	// Pointer to PINCFG36
	pincfg36 = (uint32_t volatile *)((char *)baseCFG + CFG_36);
	pin_CFG.pin36 = *pincfg36;
    
	// Pointer to CNF37
	pin37 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_37);
	pin_CNF.pin37 = pin37->CNF[0];
	pin_OE.pin37 =  pin37->OE[0];
	pin_ENB.pin37 =  pin37->INT_ENB[0];
	pin_LVL.pin37 =  pin37->INT_LVL[0];
    
	// Pointer to PINMUX37
	pinmux37 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_37);
	pin_MUX.pin37 = *pinmux37;
    
	// Pointer to PINCFG37
	pincfg37 = (uint32_t volatile *)((char *)baseCFG + CFG_37);
	pin_CFG.pin37 = *pincfg37;
    
	// Pointer to CNF38
	pin38 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_38);
	pin_CNF.pin38 = pin38->CNF[0];
	pin_OE.pin38 =  pin38->OE[0];
	pin_ENB.pin38 =  pin38->INT_ENB[0];
	pin_LVL.pin38 =  pin38->INT_LVL[0];
    
	// Pointer to PINMUX38
	pinmux38 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_38);
	pin_MUX.pin38 = *pinmux38;
    
	// Pointer to PINCFG38
	pincfg38 = (uint32_t volatile *)((char *)baseCFG + CFG_38);
	pin_CFG.pin38 = *pincfg38;
   
	// Pointer to CNF40
	pin40 = (GPIO_CNF volatile *)((char *)baseCNF + CNF_40);
	pin_CNF.pin40 = pin40->CNF[0];
	pin_OE.pin40 =  pin40->OE[0];
	pin_ENB.pin40 =  pin40->INT_ENB[0];
	pin_LVL.pin40 =  pin40->INT_LVL[0];
    
	// Pointer to PINMUX40
	pinmux40 = (uint32_t volatile *)((char *)basePINMUX + PINMUX_40);
	pin_MUX.pin40 = *pinmux40;
    
	// Pointer to PINCFG40
	pincfg40 = (uint32_t volatile *)((char *)baseCFG + CFG_40);
	pin_CFG.pin40 = *pincfg40;
    
	// Pointer to PM3_PWMx
	pinPWM = (GPIO_PWM volatile *)((char *)basePWM + PM3_PWM0);
	pinPWM_Init = *pinPWM;

	// Pointer to APBDEV_PMC_PWR_DET_VAL_0
	apbdev_pmc_pwr_det_val = (uint32_t volatile *)((char *)basePMC + APBDEV_PMC_PWR_DET_VAL_0 + 0x400);

	// Pointer to controller_clk_out_enb_l
	controller_clk_out_enb_l = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0);
    
	// Pointer to controller_clk_out_enb_h
	controller_clk_out_enb_h = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_OUT_ENB_H_0);

	// Pointer to controller_rst_devices_h
	controller_rst_devices_h = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_RST_DEVICES_H_0);

	// Pointer to clksourcespi1
	clk_source_spi1 = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_SOURCE_SPI1_0);

	// Pointer to clksourcespi2
	clk_source_spi2 = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_SOURCE_SPI2_0);

	// Pointer to controller_clk_out_enb_h
	controller_clk_out_enb_l_set = (uint32_t volatile *)((char *)baseCAR + CLK_RST_CONTROLLER_CLK_ENB_L_SET_0);

	// Global interrupt variable
	global_int = 1;

	// Power Controller it is enabled on boot
	//*controller_clk_out_enb_l |= 0x00000100;
	//*controller_clk_out_enb_l_set |= 0x00000100;

	// Allocating memory for the struct
	for(int j = 0 ; j < 41 ; j++) 
	{
		ISRFunc_CFG[j] = malloc(sizeof(ISRFunc));
		memset(ISRFunc_CFG[j], 0x00, sizeof(ISRFunc));
	}
	return status;
}


void gpioTerminate(void)
{
	// Stopping threads
	global_int = 0;

	// Cancelling threads to avoid blocking on read()
	for(int i = 0 ; i < thread_n ; i++) 
	{
		pthread_cancel(callThd[i]);
		LogFormatted(LogLevelDebug, "jetgpio.c", "gpioTerminate", "Thread number: %d cancelled.", i);
	}
    
	//Joining threads
	 for(int j = 0 ; j < thread_n ; j++) 
	{
		pthread_join(callThd[j], &status_thread);
		LogFormatted(LogLevelInfo, "jetgpio.c", "gpioTerminate", "Thread number: %d joined", j);
	}

	// Free allocated memory 
	for(int k = 0 ; k < 41 ; k++) 
	{
		free(ISRFunc_CFG[k]);
	}
    
	int pagesize = sysconf(_SC_PAGESIZE);
	// Restoring registers to their previous state
	*pinPWM = pinPWM_Init;
	pin3->CNF[0] = pin_CNF.pin3;
	pin3->OE[0] = pin_OE.pin3;
	pin3->INT_ENB[0] = pin_ENB.pin3;
	pin3->INT_LVL[0] = pin_LVL.pin3;
	*pinmux3 = pin_MUX.pin3;
	*pincfg3 = pin_CFG.pin3;
	pin5->CNF[0] = pin_CNF.pin5;
	pin5->OE[0] = pin_OE.pin5;
	pin5->INT_ENB[0] = pin_ENB.pin5;
	pin5->INT_LVL[0] = pin_LVL.pin5;
	*pinmux5 = pin_MUX.pin5;
	*pincfg5 = pin_CFG.pin5;
	pin7->CNF[0] = pin_CNF.pin7;
	pin7->OE[0] = pin_OE.pin7;
	pin7->INT_ENB[0] = pin_ENB.pin7;
	pin7->INT_LVL[0] = pin_LVL.pin7;
	*pinmux7 = pin_MUX.pin7;
	*pincfg7 = pin_CFG.pin7;
	pin8->CNF[0] = pin_CNF.pin8;
	pin8->OE[0] = pin_OE.pin8;
	pin8->INT_ENB[0] = pin_ENB.pin8;
	pin8->INT_LVL[0] = pin_LVL.pin8;
	*pinmux8 = pin_MUX.pin8;
	*pincfg8 = pin_CFG.pin8;
	pin10->CNF[0] = pin_CNF.pin10;
	pin10->OE[0] = pin_OE.pin10;
	pin10->INT_ENB[0] = pin_ENB.pin10;
	pin10->INT_LVL[0] = pin_LVL.pin10;
	*pinmux10 = pin_MUX.pin10;
	*pincfg10 = pin_CFG.pin10;
	pin11->CNF[0] = pin_CNF.pin11;
	pin11->OE[0] = pin_OE.pin11;
	pin11->INT_ENB[0] = pin_ENB.pin11;
	pin11->INT_LVL[0] = pin_LVL.pin11;
	*pinmux11 = pin_MUX.pin11;
	*pincfg11 = pin_CFG.pin11;
	pin12->CNF[0] = pin_CNF.pin12;
	pin12->OE[0] = pin_OE.pin12;
	pin12->INT_ENB[0] = pin_ENB.pin12;
	pin12->INT_LVL[0] = pin_LVL.pin12;
	*pinmux12 = pin_MUX.pin12;
	*pincfg12 = pin_CFG.pin12;
	pin13->CNF[0] = pin_CNF.pin13;
	pin13->OE[0] = pin_OE.pin13;
	pin13->INT_ENB[0] = pin_ENB.pin13;
	pin13->INT_LVL[0] = pin_LVL.pin13;
	*pinmux13 = pin_MUX.pin13;
	*pincfg13 = pin_CFG.pin13;
	pin15->CNF[0] = pin_CNF.pin15;
	pin15->OE[0] = pin_OE.pin15;
	pin15->INT_ENB[0] = pin_ENB.pin15;
	pin15->INT_LVL[0] = pin_LVL.pin15;
	*pinmux15 = pin_MUX.pin15;
	*pincfg15 = pin_CFG.pin15;
	pin16->CNF[0] = pin_CNF.pin16;
	pin16->OE[0] = pin_OE.pin16;
	pin16->INT_ENB[0] = pin_ENB.pin16;
	pin16->INT_LVL[0] = pin_LVL.pin16;
	*pinmux16 = pin_MUX.pin16;
	*pincfg16 = pin_CFG.pin16;
	pin18->CNF[0] = pin_CNF.pin18;
	pin18->OE[0] = pin_OE.pin18;
	pin18->INT_ENB[0] = pin_ENB.pin18;
	pin18->INT_LVL[0] = pin_LVL.pin18;
	*pinmux18 = pin_MUX.pin18;
	*pincfg18 = pin_CFG.pin18;
	pin19->CNF[0] = pin_CNF.pin19;
	pin19->OE[0] = pin_OE.pin19;
	pin19->INT_ENB[0] = pin_ENB.pin19;
	pin19->INT_LVL[0] = pin_LVL.pin19;
	*pinmux19 = pin_MUX.pin19;
	*pincfg19 = pin_CFG.pin19;
	pin21->CNF[0] = pin_CNF.pin21;
	pin21->OE[0] = pin_OE.pin21;
	pin21->INT_ENB[0] = pin_ENB.pin21;
	pin21->INT_LVL[0] = pin_LVL.pin21;
	*pinmux21 = pin_MUX.pin21;
	*pincfg21 = pin_CFG.pin21;
	pin22->CNF[0] = pin_CNF.pin22;
	pin22->OE[0] = pin_OE.pin22;
	pin22->INT_ENB[0] = pin_ENB.pin22;
	pin22->INT_LVL[0] = pin_LVL.pin22;
	*pinmux22 = pin_MUX.pin22;
	*pincfg22 = pin_CFG.pin22;
	pin23->CNF[0] = pin_CNF.pin23;
	pin23->OE[0] = pin_OE.pin23;
	pin23->INT_ENB[0] = pin_ENB.pin23;
	pin23->INT_LVL[0] = pin_LVL.pin23;
	*pinmux23 = pin_MUX.pin23;
	*pincfg23 = pin_CFG.pin23;
	pin24->CNF[0] = pin_CNF.pin24;
	pin24->OE[0] = pin_OE.pin24;
	pin24->INT_ENB[0] = pin_ENB.pin24;
	pin24->INT_LVL[0] = pin_LVL.pin24;
	*pinmux24 = pin_MUX.pin24;
	*pincfg24 = pin_CFG.pin24;
	pin26->CNF[0] = pin_CNF.pin26;
	pin26->OE[0] = pin_OE.pin26;
	pin26->INT_ENB[0] = pin_ENB.pin26;
	pin26->INT_LVL[0] = pin_LVL.pin26;
	*pinmux26 = pin_MUX.pin26;
	*pincfg26 = pin_CFG.pin26;
	pin27->CNF[0] = pin_CNF.pin27;
	pin27->OE[0] = pin_OE.pin27;
	pin27->INT_ENB[0] = pin_ENB.pin27;
	pin27->INT_LVL[0] = pin_LVL.pin27;
	*pinmux27 = pin_MUX.pin27;
	*pincfg27 = pin_CFG.pin27;
	pin28->CNF[0] = pin_CNF.pin28;
	pin28->OE[0] = pin_OE.pin28;
	pin28->INT_ENB[0] = pin_ENB.pin28;
	pin28->INT_LVL[0] = pin_LVL.pin28;
	*pinmux28 = pin_MUX.pin28;
	*pincfg28 = pin_CFG.pin28;
	pin29->CNF[0] = pin_CNF.pin29;
	pin29->OE[0] = pin_OE.pin29;
	pin29->INT_ENB[0] = pin_ENB.pin29;
	pin29->INT_LVL[0] = pin_LVL.pin29;
	*pinmux29 = pin_MUX.pin29;
	*pincfg29 = pin_CFG.pin29;
	pin31->CNF[0] = pin_CNF.pin31;
	pin31->OE[0] = pin_OE.pin31;
	pin31->INT_ENB[0] = pin_ENB.pin31;
	pin31->INT_LVL[0] = pin_LVL.pin31;
	*pinmux31 = pin_MUX.pin31;
	*pincfg31 = pin_CFG.pin31;
	pin32->CNF[0] = pin_CNF.pin32;
	pin32->OE[0] = pin_OE.pin32;
	pin32->INT_ENB[0] = pin_ENB.pin32;
	pin32->INT_LVL[0] = pin_LVL.pin32;
	*pinmux32 = pin_MUX.pin32;
	*pincfg32 = pin_CFG.pin32;
	pin33->CNF[0] = pin_CNF.pin33;
	pin33->OE[0] = pin_OE.pin33;
	pin33->INT_ENB[0] = pin_ENB.pin33;
	pin33->INT_LVL[0] = pin_LVL.pin33;
	*pinmux33 = pin_MUX.pin33;
	*pincfg33 = pin_CFG.pin33;
	pin35->CNF[0] = pin_CNF.pin35;
	pin35->OE[0] = pin_OE.pin35;
	pin35->INT_ENB[0] = pin_ENB.pin35;
	pin35->INT_LVL[0] = pin_LVL.pin35;
	*pinmux35 = pin_MUX.pin35;
	*pincfg35 = pin_CFG.pin35;
	pin36->CNF[0] = pin_CNF.pin36;
	pin36->OE[0] = pin_OE.pin36;
	pin36->INT_ENB[0] = pin_ENB.pin36;
	pin36->INT_LVL[0] = pin_LVL.pin36;
	*pinmux36 = pin_MUX.pin36;
	*pincfg36 = pin_CFG.pin36;
	pin37->CNF[0] = pin_CNF.pin37;
	pin37->OE[0] = pin_OE.pin37;
	pin37->INT_ENB[0] = pin_ENB.pin37;
	pin37->INT_LVL[0] = pin_LVL.pin37;
	*pinmux37 = pin_MUX.pin37;
	*pincfg37 = pin_CFG.pin37;
	pin38->CNF[0] = pin_CNF.pin38;
	pin38->OE[0] = pin_OE.pin38;
	pin38->INT_ENB[0] = pin_ENB.pin38;
	pin38->INT_LVL[0] = pin_LVL.pin38;
	*pinmux38 = pin_MUX.pin38;
	*pincfg38 = pin_CFG.pin38;
	pin40->CNF[0] = pin_CNF.pin40;
	pin40->OE[0] = pin_OE.pin40;
	pin40->INT_ENB[0] = pin_ENB.pin40;
	pin40->INT_LVL[0] = pin_LVL.pin40;
	*pinmux40 = pin_MUX.pin40;
	*pincfg40 = pin_CFG.pin40;
	
	// Ummapping CNF registers
	munmap(baseCNF, pagesize);
    
	// Ummapping PINMUX registers
	munmap(basePINMUX, pagesize);
	
	// Ummapping CFG registers 
	munmap(baseCFG, pagesize);
	
	// Ummapping PWM registers 
	munmap(basePWM, pagesize);
	
	// Ummapping CAR registers 
	munmap(baseCAR, pagesize);

	// Ummapping PMC registers 
	munmap(basePMC, pagesize);
  
	// close /dev/mem 
	close(fd_GPIO);
}

#pragma endregion //SetupTeardown


//  Read and Write Functions
//
#pragma region GpioReadWrite

int gpioRead(unsigned gpio)
{
	int level = 0;
	switch (gpio) 
	{
		
	case 3:
		level = (pin3->IN[0]) >> 3 & 1;
		break;
	case 5:
		level = (pin5->IN[0]) >> 2 & 1;
		break;
	case 7:
		level = pin7->IN[0] & 1;
		break;
	case 8:
		level = pin8->IN[0] & 1;
		break;
	case 10:
		level = (pin10->IN[0]) >> 1 & 1;
		break;
	case 11:
		level = (pin11->IN[0]) >> 2 & 1;
		break;
	case 12:
		level = (pin12->IN[0]) >> 7 & 1;
		break;
	case 13:
		level = (pin13->IN[0]) >> 6 & 1;
		break;
	case 15:
		level = (pin15->IN[0]) >> 2 & 1;
		break;
	case 16:
		level = pin16->IN[0] & 1;
		break;
	case 18:
		level = (pin18->IN[0]) >> 7 & 1;
		break;
	case 19:
		level = pin19->IN[0] & 1;
		break;
	case 21:
		level = (pin21->IN[0]) >> 1 & 1;
		break;
	case 22:
		level = (pin22->IN[0]) >> 5 & 1;
		break;
	case 23:
		level = (pin23->IN[0]) >> 2 & 1;
		break;
	case 24:
		level = (pin24->IN[0]) >> 3 & 1;
		break;
	case 26:
		level = (pin26->IN[0]) >> 4 & 1;
		break;
	case 27:
		level = pin27->IN[0] & 1;
		break;
	case 28:
		level = (pin28->IN[0]) >> 1 & 1;
		break;
	case 29:
		level = (pin29->IN[0]) >> 5 & 1;
		break;
	case 31:
		level = pin31->IN[0] & 1;
		break;
	case 32:
		level = pin32->IN[0] & 1;
		break;
	case 33:
		level = (pin33->IN[0]) >> 6 & 1;
		break;
	case 35:
		level = (pin35->IN[0]) >> 4 & 1;
		break;
	case 36:
		level = (pin36->IN[0]) >> 3 & 1;
		break;
	case 37:
		level = (pin37->IN[0]) >> 4 & 1;
		break;
	case 38:
		level = (pin38->IN[0]) >> 5 & 1;
		break;
	case 40:
		level = (pin40->IN[0]) >> 6 & 1;
		break;
	default:
		level = -1;
		Log(LogLevelWarn, "jetgpio.c", "gpioRead", "Only gpio numbers from 3 to 40 are accepted.");
	}
	return level;
}


int gpioWrite(unsigned gpio, unsigned level)
{
	int status = 1;
	if (level == 0) 
	{
		switch (gpio) 
		{
		
		case 3:
			pin3->OUT[0] &= ~(0x00000008);
			break;
		case 5:
			pin5->OUT[0] &= ~(0x00000004);
			break;
		case 7:
			pin7->OUT[0] &= ~(0x00000001);
			break;
		case 8:
			pin8->OUT[0] &= ~(0x00000001);
			break;
		case 10:
			pin10->OUT[0] &= ~(0x00000002);
			break;
		case 11:
			pin11->OUT[0] &= ~(0x00000004);
			break;
		case 12:
			pin12->OUT[0] &= ~(0x00000080);
			break;
		case 13:
			pin13->OUT[0] &= ~(0x00000040);
			break;
		case 15:
			pin15->OUT[0] &= ~(0x00000004);
			break;
		case 16:
			pin16->OUT[0] &= ~(0x00000001);
			break;
		case 18:
			pin18->OUT[0] &= ~(0x00000080);
			break;
		case 19:
			pin19->OUT[0] &= ~(0x00000001);
			break;
		case 21:
			pin21->OUT[0] &= ~(0x00000002);
			break;
		case 22:
			pin22->OUT[0] &= ~(0x00000020);
			break;
		case 23:
			pin23->OUT[0] &= ~(0x00000004);
			break;
		case 24:
			pin24->OUT[0] &= ~(0x00000008);
			break;
		case 26:
			pin26->OUT[0] &= ~(0x00000010);
			break;
		case 27:
			pin27->OUT[0] &= ~(0x00000001);
			break;
		case 28:
			pin28->OUT[0] &= ~(0x00000002);
			break;
		case 29:
			pin29->OUT[0] &= ~(0x00000020);
			break;
		case 31:
			pin31->OUT[0] &= ~(0x00000001);
			break;
		case 32:
			pin32->OUT[0] &= ~(0x00000001);
			break;
		case 33:
			pin33->OUT[0] &= ~(0x00000040);
			break;
		case 35:
			pin35->OUT[0] &= ~(0x00000010);
			break;
		case 36:
			pin36->OUT[0] &= ~(0x00000008);
			break;
		case 37:
			pin37->OUT[0] &= ~(0x00000010);
			break;
		case 38:
			pin38->OUT[0] &= ~(0x00000020);
			break;
		case 40:
			pin40->OUT[0] &= ~(0x00000040);
			break;
		default:
			status = -1;
			Log(LogLevelWarn, "jetgpio.c", "gpioWrite", "Only gpio numbers from 3 to 40 are accepted.");
		}
	}
	else if (level == 1) 
	{
		switch (gpio)
		{
		
		case 3:
			pin3->OUT[0] |= level << 3;
			break;
		case 5:
			pin5->OUT[0] |= level << 2;
			break;
		case 7:
			pin7->OUT[0] |= level;
			break;
		case 8:
			pin8->OUT[0] |= level;
			break;
		case 10:
			pin10->OUT[0] |= level << 1;
			break;
		case 11:
			pin11->OUT[0] |= level << 2;
			break;
		case 12:
			pin12->OUT[0] |= level << 7;
			break;
		case 13:
			pin13->OUT[0] |= level << 6;
			break;
		case 15:
			pin15->OUT[0] |= level << 2;
			break;
		case 16:
			pin16->OUT[0] |= level;
			break;
		case 18:
			pin18->OUT[0] |= level << 7;
			break;
		case 19:
			pin19->OUT[0] |= level;
			break;
		case 21:
			pin21->OUT[0] |= level << 1;
			break;
		case 22:
			pin22->OUT[0] |= level << 5;
			break;
		case 23:
			pin23->OUT[0] |= level << 2;
			break;
		case 24:
			pin24->OUT[0] |= level << 3;
			break;
		case 26:
			pin26->OUT[0] |= level << 4;
			break;
		case 27:
			pin27->OUT[0] |= level;
			break;
		case 28:
			pin28->OUT[0] |= level << 1;
			break;
		case 29:
			pin29->OUT[0] |= level << 5;
			break;
		case 31:
			pin31->OUT[0] |= level;
			break;
		case 32:
			pin32->OUT[0] |= level;
			break;
		case 33:
			pin33->OUT[0] |= level << 6;
			break;
		case 35:
			pin35->OUT[0] |= level << 4;
			break;
		case 36:
			pin36->OUT[0] |= level << 3;
			break;
		case 37:
			pin37->OUT[0] |= level << 4;
			break;
		case 38:
			pin38->OUT[0] |= level << 5;
			break;
		case 40:
			pin40->OUT[0] |= level << 6;
			break;
		default:
			status = -2;
			Log(LogLevelWarn, "jetgpio.c", "gpioWrite", "Only gpio numbers from 3 to 40 are accepted.");
		}
	}
	else 
	{
		Log(LogLevelWarn, "jetgpio.c", "gpioWrite", "Only levels 0 or 1 are allowed.");
		status = -3;
	}
	return status;
}


int gpioSetPWMfrequency(unsigned gpio, float frequency)
{
	int status = 0;
	int PFM = 0;
	unsigned freq = (unsigned)frequency;
	if ((freq >= 25) && (freq <= 200000))
	{
		PFM = (204800 / freq) - 1;
		switch (gpio) {
		case 32:
			pinPWM->PWM_0[0] = 0x0;
			pinPWM->PWM_0[0] = PFM;
			break;
		case 33:
			pinPWM->PWM_2[0] = 0x0;
			pinPWM->PWM_2[0] = PFM;
			break;
		default:
			status = -1;
			Log(LogLevelWarn, "jetgpio.c", "gpioSetPWMFrequency", "Only gpio numbers 32 and 33 are accepted.");
		}
		
	}
	else 
	{
		Log(LogLevelWarn, "jetgpio.c", "gpioSetPWMFrequency", "Only frequencies from 25 to 200000 Hz are allowed.");
		status = -2;
	}
	return status;
}


int gpioPWM(unsigned gpio, unsigned dutycycle)
{
	int status = 1;
	if ((dutycycle >= 0) && (dutycycle <= 256))
	{
		switch (gpio)
		{
		case 32:
			*pinmux32 = 0x00000001;
			*pincfg32 = CFG_OUT;
			pin32->CNF[0] &= ~(0x00000001);
			pinPWM->PWM_0[0] &= ~(0xFFFF0000);
			pinPWM->PWM_0[0] |= dutycycle << 16;
			pinPWM->PWM_0[0] |= 0x80000000;
			break;
		case 33:
			*pinmux33 = 0x00000002;
			*pincfg33 = CFG_OUT;
			pin33->CNF[0] &= ~(0x00000040);
			pinPWM->PWM_2[0] &= ~(0xFFFF0000);
			pinPWM->PWM_2[0] |= dutycycle << 16;
			pinPWM->PWM_2[0] |= 0x80000000;
			break;
		default:
			status = -1;
			Log(LogLevelWarn, "jetgpio.c", "gpioPWM", "Only gpio numbers 32 and 33 are accepted.");
		}
	}
	else 
	{
		Log(LogLevelWarn, "jetgpio.c", "gpioPWM", "Only a dutycycle from 0 to 256 is allowed.");
		status = -2;
	}
	return status;
}


int gpioSetMode(unsigned gpio, unsigned mode)
{
	int status = 0;
	if (mode == 0) 
	{
		switch (gpio)
		{
		case 3:
			*pinmux3 = PINMUX_IN;
			*pincfg3 = CFG_IN;
			pin3->CNF[0] |= 0x00000008;
			pin3->OE[0] &= ~(0x00000008);
			break;
		case 5:
			*pinmux5 = PINMUX_IN;
			*pincfg5 = CFG_IN;
			pin5->CNF[0] |= 0x00000004;
			pin5->OE[0] &= ~(0x00000004);
			break;
		case 7:
			*pinmux7 = PINMUX_IN;
			*pincfg7 = CFG_IN;
			pin7->CNF[0] |= 0x00000001;
			pin7->OE[0] &= ~(0x00000001);
			break;
		case 8:
			*pinmux8 = PINMUX_IN;
			*pincfg8 = CFG_IN;
			pin8->CNF[0] |= 0x00000001;
			pin8->OE[0] &= ~(0x00000001);
			break;
		case 10:
			*pinmux10 = PINMUX_IN;
			*pincfg10 = CFG_IN;
			pin10->CNF[0] |= 0x00000002;
			pin10->OE[0] &= ~(0x00000002);
			break;
		case 11:
			*pinmux11 = PINMUX_IN;
			*pincfg11 = CFG_IN;
			pin11->CNF[0] |= 0x00000004;
			pin11->OE[0] &= ~(0x00000004);
			break;
		case 12:
			*pinmux12 = PINMUX_IN;
			*pincfg12 = CFG_IN;
			pin12->CNF[0] |= 0x00000080;
			pin12->OE[0] &= ~(0x00000080);
			break;
		case 13:
			*pinmux13 = PINMUX_IN;
			*pincfg13 = CFG_IN;
			pin13->CNF[0] |= 0x00000040;
			pin13->OE[0] &= ~(0x00000040);
			break;
		case 15:
			*pinmux15 = PINMUX_IN;
			*pincfg15 = CFG_IN;
			pin15->CNF[0] |= 0x00000004;
			pin15->OE[0] &= ~(0x00000004);
			break;
		case 16:
			*pinmux16 = PINMUX_IN;
			*pincfg16 = CFG_IN;
			pin16->CNF[0] |= 0x00000001;
			pin16->OE[0] &= ~(0x00000001);
			break;
		case 18:
			*pinmux18 = PINMUX_IN;
			*pincfg18 = CFG_IN;
			pin18->CNF[0] |= 0x00000080;
			pin18->OE[0] &= ~(0x00000080);
			break;
		case 19:
			*pinmux19 = PINMUX_IN;
			*pincfg19 = CFG_IN;
			pin19->CNF[0] |= 0x00000001;
			pin19->OE[0] &= ~(0x00000001);
			break;
		case 21:
			*pinmux21 = PINMUX_IN;
			*pincfg21 = CFG_IN;
			pin21->CNF[0] |= 0x00000002;
			pin21->OE[0] &= ~(0x00000002);
			break;
		case 22:
			*pinmux22 = PINMUX_IN;
			*pincfg22 = CFG_IN;
			pin22->CNF[0] |= 0x00000020;
			pin22->OE[0] &= ~(0x00000020);
			break;
		case 23:
			*pinmux23 = PINMUX_IN;
			*pincfg23 = CFG_IN;
			pin23->CNF[0] |= 0x00000004;
			pin23->OE[0] &= ~(0x00000004);
			break;
		case 24:
			*pinmux24 = PINMUX_IN;
			*pincfg24 = CFG_IN;
			pin24->CNF[0] |= 0x00000008;
			pin24->OE[0] &= ~(0x00000008);
			break;
		case 26:
			*pinmux26 = PINMUX_IN;
			*pincfg26 = CFG_IN;
			pin26->CNF[0] |= 0x00000010;
			pin26->OE[0] &= ~(0x00000010);
			break;
		case 27:
			*pinmux27 = PINMUX_IN;
			*pincfg27 = CFG_IN;
			pin27->CNF[0] |= 0x00000001;
			pin27->OE[0] &= ~(0x00000001);
			break;
		case 28:
			*pinmux28 = PINMUX_IN;
			*pincfg28 = CFG_IN;
			pin28->CNF[0] |= 0x00000002;
			pin28->OE[0] &= ~(0x00000002);
			break;
		case 29:
			*pinmux29 = PINMUX_IN;
			*pincfg29 = CFG_IN;
			pin29->CNF[0] |= 0x00000020;
			pin29->OE[0] &= ~(0x00000020);
			break;
		case 31:
			*pinmux31 = PINMUX_IN;
			*pincfg31 = CFG_IN;
			pin31->CNF[0] |= 0x00000001;
			pin31->OE[0] &= ~(0x00000001);
			break;
		case 32:
			*pinmux32 = PINMUX_IN;
			*pincfg32 = CFG_IN;
			pin32->CNF[0] |= 0x00000001;
			pin32->OE[0] &= ~(0x00000001);
			break;
		case 33:
			*pinmux33 = PINMUX_IN;
			*pincfg33 = CFG_IN;
			pin33->CNF[0] |= 0x00000040;
			pin33->OE[0] &= ~(0x00000040);
			break;
		case 35:
			*pinmux35 = PINMUX_IN;
			*pincfg35 = CFG_IN;
			pin35->CNF[0] |= 0x00000010;
			pin35->OE[0] &= ~(0x00000010);
			break;
		case 36:
			*pinmux36 = PINMUX_IN;
			*pincfg36 = CFG_IN;
			pin36->CNF[0] |= 0x00000008;
			pin36->OE[0] &= ~(0x00000008);
			break;
		case 37:
			*pinmux37 = PINMUX_IN;
			*pincfg37 = CFG_IN;
			pin37->CNF[0] |= 0x00000010;
			pin37->OE[0] &= ~(0x00000010);
			break;
		case 38:
			*pinmux38 = PINMUX_IN;
			*pincfg38 = CFG_IN;
			pin38->CNF[0] |= 0x00000020;
			pin38->OE[0] &= ~(0x00000020);
			break;
		case 40:
			*pinmux40 = PINMUX_IN;
			*pincfg40 = CFG_IN;
			pin40->CNF[0] |= 0x00000040;
			pin40->OE[0] &= ~(0x00000040);
			break;
		default:
			status = -1;
			Log(LogLevelWarn, "jetgpio.c", "gpioSetMode", "Only gpio numbers from 3 to 40 are accepted.");
		}
	}
	else if (mode == 1) 
	{
		switch (gpio)
		{	
		case 3:
			*pinmux3 = PINMUX_OUT;
			*pincfg3 = CFG_OUT;
			pin3->CNF[0] |= 0x00000008;
			pin3->OE[0] |= 0x00000008;
			break;
		case 5:
			*pinmux5 = PINMUX_OUT;
			*pincfg5 = CFG_OUT;
			pin5->CNF[0] |= 0x00000004;
			pin5->OE[0] |= 0x00000004;
			break;
		case 7:
			*pinmux7 = PINMUX_OUT;
			*pincfg7 = CFG_OUT;
			pin7->CNF[0] |= 0x00000001;
			pin7->OE[0] |= 0x00000001;
			break;
		case 8:
			*pinmux8 = PINMUX_OUT;
			*pincfg8 = CFG_OUT;
			pin8->CNF[0] |= 0x00000001;
			pin8->OE[0] |= 0x00000001;
			break;
		case 10:
			*pinmux10 = PINMUX_OUT;
			*pincfg10 = CFG_OUT;
			pin10->CNF[0] |= 0x00000002;
			pin10->OE[0] |= 0x00000002;
			break;
		case 11:
			*pinmux11 = PINMUX_OUT;
			*pincfg11 = CFG_OUT;
			pin11->CNF[0] |= 0x00000004;
			pin11->OE[0] |= 0x00000004;
			break;
		case 12:
			*pinmux12 = PINMUX_OUT;
			*pincfg12 = CFG_OUT;
			pin12->CNF[0] |= 0x00000080;
			pin12->OE[0] |= 0x00000080;
			break;
		case 13:
			*pinmux13 = PINMUX_OUT1;
			*pincfg13 = CFG_OUT1;
			pin13->CNF[0] |= 0x00000040;
			pin13->OE[0] |= 0x00000040;
			break;
		case 15:
			*pinmux15 = PINMUX_OUT;
			*pincfg15 = CFG_OUT;
			pin15->CNF[0] |= 0x00000004;
			pin15->OE[0] |= 0x00000004;
			break;
		case 16:
			*pinmux16 = PINMUX_OUT1;
			*pincfg16 = CFG_OUT1;
			pin16->CNF[0] |= 0x00000001;
			pin16->OE[0] |= 0x00000001;
			break;
		case 18:
			*pinmux18 = PINMUX_OUT1;
			*pincfg18 = CFG_OUT1;
			pin18->CNF[0] |= 0x00000080;
			pin18->OE[0] |= 0x00000080;
			break;
		case 19:
			*pinmux19 = PINMUX_OUT1;
			*pincfg19 = CFG_OUT1;
			pin19->CNF[0] |= 0x00000001;
			pin19->OE[0] |= 0x00000001;
			break;
		case 21:
			*pinmux21 = PINMUX_OUT1;
			*pincfg21 = CFG_OUT1;
			pin21->CNF[0] |= 0x00000002;
			pin21->OE[0] |= 0x00000002;
			break;
		case 22:
			*pinmux22 = PINMUX_OUT1;
			*pincfg22 = CFG_OUT1;
			pin22->CNF[0] |= 0x00000020;
			pin22->OE[0] |= 0x00000020;
			break;
		case 23:
			*pinmux23 = PINMUX_OUT1;
			*pincfg23 = CFG_OUT1;
			pin23->CNF[0] |= 0x00000004;
			pin23->OE[0] |= 0x00000004;
			break;
		case 24:
			*pinmux24 = PINMUX_OUT1;
			*pincfg24 = CFG_OUT1;
			pin24->CNF[0] |= 0x00000008;
			pin24->OE[0] |= 0x00000008;
			break;
		case 26:
			*pinmux26 = PINMUX_OUT1;
			*pincfg26 = CFG_OUT1;
			pin26->CNF[0] |= 0x00000010;
			pin26->OE[0] |= 0x00000010;
			break;
		case 27:
			*pinmux27 = PINMUX_OUT;
			*pincfg27 = CFG_OUT;
			pin27->CNF[0] |= 0x00000001;
			pin27->OE[0] |= 0x00000001;
			break;
		case 28:
			*pinmux28 = PINMUX_OUT;
			*pincfg28 = CFG_OUT;
			pin28->CNF[0] |= 0x00000002;
			pin28->OE[0] |= 0x00000002;
			break;
		case 29:
			*pinmux29 = PINMUX_OUT;
			*pincfg29 = CFG_OUT;
			pin29->CNF[0] |= 0x00000020;
			pin29->OE[0] |= 0x00000020;
			break;
		case 31:
			*pinmux31 = PINMUX_OUT;
			*pincfg31 = CFG_OUT;
			pin31->CNF[0] |= 0x00000001;
			pin31->OE[0] |= 0x00000001;
			break;
		case 32:
			*pinmux32 = PINMUX_OUT;
			*pincfg32 = CFG_OUT;
			pin32->CNF[0] |= 0x00000001;
			pin32->OE[0] |= 0x00000001;
			break;
		case 33:
			*pinmux33 = PINMUX_OUT;
			*pincfg33 = CFG_OUT;
			pin33->CNF[0] |= 0x00000040;
			pin33->OE[0] |= 0x00000040;
			break;
		case 35:
			*pinmux35 = PINMUX_OUT;
			*pincfg35 = CFG_OUT;
			pin35->CNF[0] |= 0x00000010;
			pin35->OE[0] |= 0x00000010;
			break;
		case 36:
			*pinmux36 = PINMUX_OUT;
			*pincfg36 = CFG_OUT;
			pin36->CNF[0] |= 0x00000008;
			pin36->OE[0] |= 0x00000008;
			break;
		case 37:
			*pinmux37 = PINMUX_OUT1;
			*pincfg37 = CFG_OUT1;
			pin37->CNF[0] |= 0x00000010;
			pin37->OE[0] |= 0x00000010;
			break;
		case 38:
			*pinmux38 = PINMUX_OUT;
			*pincfg38 = CFG_OUT;
			pin38->CNF[0] |= 0x00000020;
			pin38->OE[0] |= 0x00000020;
			break;
		case 40:
			*pinmux40 = PINMUX_OUT;
			*pincfg40 = CFG_OUT;
			pin40->CNF[0] |= 0x00000040;
			pin40->OE[0] |= 0x00000040;
			break;
		default:
			status = -2;
			Log(LogLevelWarn, "jetgpio.c", "gpioSetMode", "Only gpio numbers from 3 to 40 are accepted.");
		}
		
	}
	else if (mode == 1) 
	{
		return gpioPWM(gpio, 0);
	}
	else
	{
		Log(LogLevelWarn, "jetgpio.c", "gpioSetMode", "Only modes allowed are INPUT, OUTPUT, and PWM_OUTPUT.");
		status = -3;
	}

	return status;	
}

#pragma endregion //GpioReadWrite


// Interrupt Callback
//
#pragma region ISR

void *callback(void *arg)
{
	ISRFunc *int_struct = (ISRFunc *) arg;
	int edge = int_struct->edge;
	unsigned gpio_offset = int_struct->gpio_offset;
	uint64_t *timestamp =  int_struct->timestamp;
	int fd;
	int ret;
	struct gpioevent_request req;
	struct gpioevent_data event;
    
	fd = open("/dev/gpiochip0", 0);
	if (fd < 0) {
		printf("bad handle (%d)\n", fd);
		pthread_exit(NULL);	
	}

	req.lineoffset = gpio_offset;
	req.handleflags = GPIOHANDLE_REQUEST_INPUT;
	req.eventflags = edge;
	  
	ret = ioctl(fd, GPIO_GET_LINEEVENT_IOCTL, &req);
	if (ret == -1) {
		ret = -errno;
		LogFormatted(LogLevelError, "jetgpio.c", "callback", "Failed to issue GET EVENT ""IOCTL (%d)", ret);
		close(fd);
		pthread_exit(NULL);
	}
	close(fd);
	while (global_int) {
		ret = read(req.fd, &event, sizeof(event));
		if ((ret == -1) || (ret != sizeof(event))) {
			ret = -errno;
			LogFormatted(LogLevelError, "jetgpio.c", "callback", "Failed to read event (%d)", ret);
			break;
		}
		*timestamp = event.timestamp;     
		switch (event.id) {
		case GPIOEVENT_EVENT_RISING_EDGE:
			int_struct->f();
			break;
		case GPIOEVENT_EVENT_FALLING_EDGE:
			int_struct->f();
			break;
		default:
			//Do nothing, this shouldn't be happening
			pthread_exit(NULL);
		}
	}
	pthread_exit(NULL);
}


int gpioSetISRFunc(unsigned gpio, unsigned edge, unsigned long *timestamp, void(*f)())
{
	int status = 1;
	unsigned x = 0;
	unsigned gpio_offset = 0;

	if (edge == RISING_EDGE || edge == FALLING_EDGE || edge == EITHER_EDGE)
	{
		switch (gpio) 
		{
		case 3:
			x = 0x00000008;
			pin3->INT_LVL[0] = (edge == 1 ? (pin3->INT_LVL[0] | (x + x * 0x100)) : (edge == 2 ? ((pin3->INT_LVL[0] | x * 0x100) & ~x) : pin3->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin3->INT_ENB[0] |= x;
			pin3->INT_CLR[0] |= x;
			gpio_offset = 75;
			break;
		case 5:
			x = 0x00000004;
			pin5->INT_LVL[0] = (edge == 1 ? (pin5->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin5->INT_LVL[0] | x * 0x100) & ~x) : pin5->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin5->INT_ENB[0] |= x;
			pin5->INT_CLR[0] |= x;
			gpio_offset = 74;
			break;
		case 7:
			x =  0x00000001;
			pin7->INT_LVL[0] = (edge == 1 ? (pin7->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin7->INT_LVL[0] | x * 0x100) & ~x) : pin7->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin7->INT_ENB[0] |= x;
			pin7->INT_CLR[0] |= x;
			gpio_offset = 216;
			break;
		case 8:
			x =  0x00000001;
			pin8->INT_LVL[0] = (edge == 1 ? (pin8->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin8->INT_LVL[0] | x * 0x100) & ~x) : pin8->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin8->INT_ENB[0] |= x;
			pin8->INT_CLR[0] |= x;
			gpio_offset = 48;
			break;
		case 10:
			x = 0x00000002;
			pin10->INT_LVL[0] = (edge == 1 ? (pin10->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin10->INT_LVL[0] | x * 0x100) & ~x) : pin10->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin10->INT_ENB[0] |= x;
			pin10->INT_CLR[0] |= x;
			gpio_offset = 49;
			break;
		case 11:
			x = 0x00000004;
			pin11->INT_LVL[0] = (edge == 1 ? (pin11->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin11->INT_LVL[0] | x * 0x100) & ~x) : pin11->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin11->INT_ENB[0] |= x;
			pin11->INT_CLR[0] |= x;
			gpio_offset = 50;
			break;
		case 12:
			x = 0x00000080;
			pin12->INT_LVL[0] = (edge == 1 ? (pin12->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin12->INT_LVL[0] | x * 0x100) & ~x) : pin12->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin12->INT_ENB[0] |= x;
			pin12->INT_CLR[0] |= x;
			gpio_offset = 79;
			break;
		case 13:
			x = 0x00000040;
			pin13->INT_LVL[0] = (edge == 1 ? (pin13->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin13->INT_LVL[0] | x * 0x100) & ~x) : pin13->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin13->INT_ENB[0] |= x;
			pin13->INT_CLR[0] |= x;
			gpio_offset = 14;
			break;
		case 15:
			x = 0x00000004;
			pin15->INT_LVL[0] = (edge == 1 ? (pin15->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin15->INT_LVL[0] | x * 0x100) & ~x) : pin15->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin15->INT_ENB[0] |= x;
			pin15->INT_CLR[0] |= x;
			gpio_offset = 194;
			break;
		case 16:
			x = 0x00000001;
			pin16->INT_LVL[0] = (edge == 1 ? (pin16->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin16->INT_LVL[0] | x * 0x100) & ~x) : pin16->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin16->INT_ENB[0] |= x;
			pin16->INT_CLR[0] |= x;
			gpio_offset = 232;
			break;
		case 18:
			x = 0x00000080;
			pin18->INT_LVL[0] = (edge == 1 ? (pin18->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin18->INT_LVL[0] | x * 0x100) & ~x) : pin18->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin18->INT_ENB[0] |= x;
			pin18->INT_CLR[0] |= x;
			gpio_offset = 15;
			break;
		case 19:
			x = 0x00000001;
			pin19->INT_LVL[0] = (edge == 1 ? (pin19->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin19->INT_LVL[0] | x * 0x100) & ~x) : pin19->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin19->INT_ENB[0] |= x;
			pin19->INT_CLR[0] |= x;
			gpio_offset = 16;
			break;
		case 21:
			x = 0x00000002;
			pin21->INT_LVL[0] = (edge == 1 ? (pin21->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin21->INT_LVL[0] | x * 0x100) & ~x) : pin21->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin21->INT_ENB[0] |= x;
			pin21->INT_CLR[0] |= x;
			gpio_offset = 17;
			break;
		case 22:
			x = 0x00000020;
			pin22->INT_LVL[0] = (edge == 1 ? (pin22->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin22->INT_LVL[0] | x * 0x100) & ~x) : pin22->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin22->INT_ENB[0] |= x;
			pin22->INT_CLR[0] |= x;
			gpio_offset = 13;
			break;
		case 23:
			x = 0x00000004;
			pin23->INT_LVL[0] = (edge == 1 ? (pin23->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin23->INT_LVL[0] | x * 0x100) & ~x) : pin23->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin23->INT_ENB[0] |= x;
			pin23->INT_CLR[0] |= x;
			gpio_offset = 18;
			break;
		case 24:
			x = 0x00000008;
			pin24->INT_LVL[0] = (edge == 1 ? (pin24->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin24->INT_LVL[0] | x * 0x100) & ~x) : pin24->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin24->INT_ENB[0] |= x;
			pin24->INT_CLR[0] |= x;
			gpio_offset = 19;
			break;
		case 26:
			x = 0x00000010;
			pin26->INT_LVL[0] = (edge == 1 ? (pin26->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin26->INT_LVL[0] | x * 0x100) & ~x) : pin26->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin26->INT_ENB[0] |= x;
			pin26->INT_CLR[0] |= x;
			gpio_offset = 20;
			break;
		case 27:
			x = 0x00000001;
			pin27->INT_LVL[0] = (edge == 1 ? (pin27->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin27->INT_LVL[0] | x * 0x100) & ~x) : pin27->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin27->INT_ENB[0] |= x;
			pin27->INT_CLR[0] |= x;
			gpio_offset = 72;
			break;
		case 28:
			x = 0x00000002;
			pin28->INT_LVL[0] = (edge == 1 ? (pin28->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin28->INT_LVL[0] | x * 0x100) & ~x) : pin28->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin28->INT_ENB[0] |= x;
			pin28->INT_CLR[0] |= x;
			gpio_offset = 73;
			break;
		case 29:
			x =  0x00000020;
			pin29->INT_LVL[0] = (edge == 1 ? (pin29->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin29->INT_LVL[0] | x * 0x100) & ~x) : pin29->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin29->INT_ENB[0] |= x;
			pin29->INT_CLR[0] |= x;
			gpio_offset = 149;
			break;
		case 31:
			x =  0x00000001;
			pin31->INT_LVL[0] = (edge == 1 ? (pin31->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin31->INT_LVL[0] | x * 0x100) & ~x) : pin31->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin31->INT_ENB[0] |= x;
			pin31->INT_CLR[0] |= x;
			gpio_offset = 200;
			break;
		case 32:
			x =  0x00000001;
			pin32->INT_LVL[0] = (edge == 1 ? (pin32->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin32->INT_LVL[0] | x * 0x100) & ~x) : pin32->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin32->INT_ENB[0] |= x;
			pin32->INT_CLR[0] |= x;
			gpio_offset = 168;
			break;
		case 33:
			x = 0x00000040;
			pin33->INT_LVL[0] = (edge == 1 ? (pin33->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin33->INT_LVL[0] | x * 0x100) & ~x) : pin33->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin33->INT_ENB[0] |= x;
			pin33->INT_CLR[0] |= x;
			gpio_offset = 38;
			break;
		case 35:
			x = 0x00000010;
			pin35->INT_LVL[0] = (edge == 1 ? (pin35->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin35->INT_LVL[0] | x * 0x100) & ~x) : pin35->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin35->INT_ENB[0] |= x;
			pin35->INT_CLR[0] |= x;
			gpio_offset = 76;
			break;
		case 36:
			x = 0x00000008;
			pin36->INT_LVL[0] = (edge == 1 ? (pin36->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin36->INT_LVL[0] | x * 0x100) & ~x) : pin36->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin36->INT_ENB[0] |= x;
			pin36->INT_CLR[0] |= x;
			gpio_offset = 51;
			break;
		case 37:
			x = 0x00000010;
			pin37->INT_LVL[0] = (edge == 1 ? (pin37->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin37->INT_LVL[0] | x * 0x100) & ~x) : pin37->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin37->INT_ENB[0] |= x;
			pin37->INT_CLR[0] |= x;
			gpio_offset = 12;
			break;
		case 38:
			x =  0x00000020;
			pin38->INT_LVL[0] = (edge == 1 ? (pin38->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin38->INT_LVL[0] | x * 0x100) & ~x) : pin38->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin38->INT_ENB[0] |= x;
			pin38->INT_CLR[0] |= x;
			gpio_offset = 77;
			break;
		case 40:
			x = 0x00000040;
			pin40->INT_LVL[0] = (edge == 1 ? (pin40->INT_LVL[0] | (x + x * 0x100)) : (edge == 0 ? ((pin40->INT_LVL[0] | x * 0x100) & ~x) : pin40->INT_LVL[0] | (x * 0x100 + x * 0x10000)));
			pin40->INT_ENB[0] |= x;
			pin40->INT_CLR[0] |= x;
			gpio_offset = 78;
			break;
		default:
			status = -1;
			Log(LogLevelWarn, "jetgpio.c", "gpioSetISRFunc", "Only gpio numbers from 3 to 40 are accepted.");
		}
	}
  
	else 
	{
		Log(LogLevelWarn, "jetgpio.c", "gpioSetISRFunc", "Edge should be: RISING_EDGE,FALLING_EDGE or EITHER_EDGE");
		status = -3;
	}
	if (ISRFunc_CFG[gpio]->gpio != 0) 
	{
		LogFormatted(LogLevelWarn, "jetgpio.c", "gpioSetISRFunc", "Input pin %d is already being monitored for interruptions\n", gpio);
		status = -4;
	}
	else {
		ISRFunc_CFG[gpio]->gpio = gpio;
		ISRFunc_CFG[gpio]->f = f;
		ISRFunc_CFG[gpio]->edge = edge;
		ISRFunc_CFG[gpio]->gpio_offset = gpio_offset;
		ISRFunc_CFG[gpio]->stat_reg = x;
		ISRFunc_CFG[gpio]->timestamp = timestamp;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pth_err = pthread_create(&callThd[thread_n], &attr, callback, (void *)ISRFunc_CFG[gpio]); 
		if (pth_err != 0) 
		{
			LogFormatted(LogLevelError, "jetgpio.c", "gpioSetISRFunc", "Thread not created, exiting the function with error: %d\n", pth_err);
			return (-5);
		}
		thread_n++;
	}
	return status;
}

#pragma endregion