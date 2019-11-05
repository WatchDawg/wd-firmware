#ifndef MAGNETOMETER_h
#define MAGNETOMETER_h

/*

This header initializes the LIS3MDL to use EUSCI B1's SPI interface
The debug function uses EUSCA A0's UART to print over the USB backchannel

*/

/* C headers */
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/* TI MSP430 library headers */
#include "msp430fr2476.h"
#include "driverlib.h"
//#include "IQmathLib.h"
//#include "QmathLib.h"

#define INTERRUPT_FLAG_REG UCB1IFG
#define SPI_CS_PORT GPIO_PORT_P4
#define SPI_CS_PIN GPIO_PIN2
#define SPI_MOSI_PORT GPIO_PORT_P3
#define SPI_MOSI_PIN GPIO_PIN2
#define SPI_SCLK_PORT GPIO_PORT_P3
#define SPI_SCLK_PIN GPIO_PIN5
#define SPI_MISO_PORT GPIO_PORT_P3
#define SPI_MISO_PIN GPIO_PIN6
#define SPI_BASE_ADDR EUSCI_B1_BASE

//#define INTERRUPT_FLAG_REG UCB0IFG
//#define SPI_CS_PORT GPIO_PORT_P4
//#define SPI_CS_PIN GPIO_PIN2
//#define SPI_MOSI_PORT GPIO_PORT_P1
//#define SPI_MOSI_PIN GPIO_PIN2
//#define SPI_SCLK_PORT GPIO_PORT_P1
//#define SPI_SCLK_PIN GPIO_PIN1
//#define SPI_MISO_PORT GPIO_PORT_P1
//#define SPI_MISO_PIN GPIO_PIN3
//#define SPI_BASE_ADDR EUSCI_B0_BASE

#define SPI_READ_MASK 0x80
#define SPI_CLOCK_HZ 500000

#define DEBUG_UART_TX_PORT GPIO_PORT_P1
#define DEBUG_UART_TX_PIN GPIO_PIN4
#define DEBUG_UART_RX_PORT GPIO_PORT_P1
#define DEBUG_UART_RX_PIN GPIO_PIN5
#define DEBUG_UART_BASE_ADDR EUSCI_A0_BASE

typedef struct {
	int16_t maxX, maxY, minX, minY;
	int16_t offsetX, offsetY;
	int16_t deltaX, deltaY, deltaAvg;
	float scaleX, scaleY;
} magCalibrationData;

// register addresses
typedef enum {
  WHO_AM_I    = 0x0F,
  
  CTRL_REG1   = 0x20,
  CTRL_REG2   = 0x21,
  CTRL_REG3   = 0x22,
  CTRL_REG4   = 0x23,
  CTRL_REG5   = 0x24,

  STATUS_REG  = 0x27,
  
  OUT_X_L     = 0x28,
  OUT_X_H     = 0x29,
  OUT_Y_L     = 0x2A,
  OUT_Y_H     = 0x2B,
  OUT_Z_L     = 0x2C,
  OUT_Z_H     = 0x2D,
  
  TEMP_OUT_L  = 0x2E,
  TEMP_OUT_H  = 0x2F,

  INT_CFG     = 0x30,
  INT_SRC     = 0x31,
  INT_THS_L   = 0x32,
  INT_THS_H   = 0x33,
} regAddr;

magCalibrationData calData;

void mag_writeReg(uint8_t addr, uint8_t data);
uint8_t mag_readReg(uint8_t addr);
int16_t mag_readAxis(uint8_t axis);
void mag_setSingleShotMode();
void mag_calibrationStep();
void mag_exitCalibration();
void mag_calibrate();
void mag_initMag();
void mag_initSPI();
void mag_init();
int16_t mag_getHeading();
int16_t mag_getTemp();

/* apparently itoa is not a standard C function
and thus there is no port for the MSP430 */
// implementation was taken from an E2E post
void itoa(long int value, char* result, int base);

/* debug functions that use the USB0 backchannel */
uint8_t DEBUG_MAG_INIT();
void DEBUG_MAG_PRINT_STRING(char* str);
void DEBUG_MAG_PRINT_NEWLINE(); // prints windows newline <CRLF>

#endif
