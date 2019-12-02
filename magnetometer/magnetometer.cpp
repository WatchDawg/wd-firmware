#include "magnetometer.h"

int16_t min (int16_t a, int16_t b) {
    return a < b ? a : b;
}

int16_t max (int16_t a, int16_t b) {
    return a > b ? a : b;
}

void mag_writeReg(uint8_t addr, uint8_t data) {
	UCB0CTLW1 = UCASTP_1;
	UCB0TBCNT = 0x0001;
	UCB0CTL1  &= ~UCSWRST;
	UCB0CTL1 |= UCTXSTT + UCTR;		// Start i2c write operation
	//write the address
	while(!(UCB0IFG & UCTXIFG0));
	UCB0TXBUF = addr;

	//write the data
	while(!(UCB0IFG & UCTXIFG0));
	//UCB0TXBUF = TxData[0];
	UCB0TXBUF = data;
	while(!(UCB0IFG & UCTXIFG0));

	UCB0CTL1 |= UCTXSTP;
	while (!(UCB0IFG & UCSTPIFG)); 	// Ensure stop condition got sent
	UCB0CTL1  |= UCSWRST;

    __delay_cycles(1000);
}

uint8_t mag_readReg(uint8_t addr) {
    uint8_t data;

	UCB0CTLW1 = UCASTP_1;
	UCB0TBCNT = 0x0001;
	UCB0CTL1  &= ~UCSWRST;
	UCB0CTL1 |= UCTXSTT + UCTR;		// Start i2c write operation

	while(!(UCB0IFG & UCTXIFG0));
	UCB0TXBUF = addr;

	while(!(UCB0IFG & UCBCNTIFG));
	UCB0CTL1 &= ~UCTR; 				// I2C read operation
	UCB0CTL1 |= UCTXSTT; 			// Repeated start

	while(!(UCB0IFG & UCRXIFG0));
	data = UCB0RXBUF;

	UCB0CTLW0 |= UCTXSTP; 			// Send stop after next RX
	while(!(UCB0IFG & UCRXIFG0));	// Receive BIP8
	while(!(UCB0IFG & UCSTPIFG));   // Ensure stop condition got sent
	UCB0CTL1  |= UCSWRST;

    UCB0TXBUF = 0;
    UCB0RXBUF = 0;
    UCB0IFG &= ~(UCSTPIFG | UCRXIFG0 | UCTXIFG0);

    __delay_cycles(1000);

	return data;
}

int16_t mag_readAxis(uint8_t axis) {
    volatile uint8_t requestDataLow = 0x28;
    volatile uint8_t requestDataHigh = 0x29;
    volatile uint16_t receiveDataLow = 0;
    volatile uint16_t receiveDataHigh = 0;
    volatile int16_t data = 0x0000;

    switch(axis) {
        case 'x':
            requestDataLow = 0x28;
            requestDataHigh = 0x29;
            break;
        case 'y':
            requestDataLow = 0x2A;
            requestDataHigh = 0x2B;
            break;
        case 'z':
            requestDataLow = 0x2C;
            requestDataHigh = 0x2D;
            break;
        default:
            break;
    }
    // GET LOW BITS
    receiveDataLow = mag_readReg(requestDataLow);

    // GET HIGH BITS
    receiveDataHigh = mag_readReg(requestDataHigh) << 8;

    data = (int16_t)(receiveDataHigh | receiveDataLow);

    return data;
}

void mag_setSingleShotMode() {
	mag_writeReg(0x22, 0x01);
    __delay_cycles(80000);
}

void mag_calibrationStep() {
    mag_setSingleShotMode();
    int16_t x = mag_readAxis('x');
    int16_t y = mag_readAxis('y');


    calData.minX = min(calData.minX, x);
    calData.minY = min(calData.minY, y);

    calData.maxX = max(calData.maxX, x);
    calData.maxY = max(calData.maxY, y);
}

void mag_calibrate() {
    calData.minX = INT16_MAX;
    calData.minY = INT16_MAX;

    calData.maxX = INT16_MIN;
    calData.maxY = INT16_MIN;
    uint32_t i = 0;
    while(i < 1500) {
        mag_calibrationStep();
        ++i;
    }
}

void mag_initMag() {
	//mag_writeReg(0x20, 0x7C);
    mag_writeReg(0x20, 0x80); // sets temp bit
	mag_writeReg(0x21, 0x00);
	mag_writeReg(0x22, 0x01);
	mag_writeReg(0x23, 0x0C);
}

void mag_initI2C() {
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        I2C_SCL_PORT,
        I2C_SCL_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    GPIO_setAsPeripheralModuleFunctionOutputPin(
        I2C_SDA_PORT,
        I2C_SDA_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    GPIO_setAsOutputPin( I2C_CS_PORT, I2C_CS_PIN );
    GPIO_setOutputHighOnPin( I2C_CS_PORT, I2C_CS_PIN );

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = CS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    param.byteCounterThreshold = 1;
    param.autoSTOPGeneration = EUSCI_B_I2C_SET_BYTECOUNT_THRESHOLD_FLAG;

    EUSCI_B_I2C_initMaster(I2C_BASE_ADDR, &param);

    EUSCI_B_I2C_setSlaveAddress(I2C_BASE_ADDR, I2C_MAG_ADDR);
}

void mag_initSPI() {
    //Configure MOSI as output pin
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        SPI_MOSI_PORT,
        SPI_MOSI_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    //Configure CS1 as output pin
    GPIO_setOutputHighOnPin(SPI_CS_PORT, SPI_CS_PIN);
    GPIO_setAsOutputPin(SPI_CS_PORT, SPI_CS_PIN);

    //Configure SCLK as output pin
    GPIO_setAsPeripheralModuleFunctionOutputPin(
        SPI_SCLK_PORT,
        SPI_SCLK_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    //Configure MISO as input pin
    GPIO_setAsPeripheralModuleFunctionInputPin(
        SPI_MISO_PORT,
        SPI_MISO_PIN,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    // Configure SPI
    EUSCI_B_SPI_initMasterParam config = {0};
    config.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    config.clockSourceFrequency = CS_getSMCLK();
    config.desiredSpiClock = 5000000;
    config.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    config.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    config.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    config.spiMode = EUSCI_B_SPI_3PIN;

    EUSCI_B_SPI_initMaster(SPI_BASE_ADDR, &config);

    EUSCI_B_SPI_enable(SPI_BASE_ADDR);
}

void mag_init() {
	mag_initI2C();
	mag_initMag();
	mag_calibrate();
}


int16_t mag_getHeading() {
    mag_setSingleShotMode();
    volatile int16_t x = mag_readAxis('x');
    volatile int16_t y = mag_readAxis('y');
    
    // set to single shot (but without delay)
    mag_writeReg(0x22, 0x01);


    calData.minX = min(calData.minX, x);
    calData.minY = min(calData.minY, y);

    calData.maxX = max(calData.maxX, x);
    calData.maxY = max(calData.maxY, y);

    volatile int16_t x_corrected = x - ((calData.minX + calData.maxX) / 2);
    volatile int16_t y_corrected = y - ((calData.minY + calData.maxY) / 2);

    volatile int16_t result = 180 * (atan2(y_corrected, x_corrected) / M_PI);
    result += 90;
    if(result < 0) {
        result += 360;
    }

    return result;
}


int16_t mag_getTemp() {
    volatile int16_t temp;
    volatile uint16_t receiveDataLow = 0;
    volatile uint16_t receiveDataHigh = 0;

    // GET LOW BITS
    receiveDataLow = mag_readReg(TEMP_OUT_L);

    // GET HIGH BITS
    receiveDataHigh = mag_readReg(TEMP_OUT_H) << 8;


    temp = (int16_t)(receiveDataHigh | receiveDataLow);
    return 25 + (temp/8); // spec says 8 LSB/ deg C; 25 is zero point
}

/* apparently itoa is not a standard C function
and thus there is no port for the MSP430 */
// implementation was taken from an E2E post
void itoa(long int value, char* result, int base) {
	    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0';}

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr--= *ptr1;
    *ptr1++ = tmp_char;
    }
}

///* debug functions that use the USB0 backchannel */
uint8_t DEBUG_MAG_INIT() {
	//Configure UART pins
	GPIO_setAsPeripheralModuleFunctionInputPin(
	    DEBUG_UART_TX_PORT,
	    DEBUG_UART_TX_PIN,
	    GPIO_PRIMARY_MODULE_FUNCTION
	);
	GPIO_setAsPeripheralModuleFunctionInputPin(
	    DEBUG_UART_RX_PORT,
	    DEBUG_UART_RX_PIN,
	    GPIO_PRIMARY_MODULE_FUNCTION
	);

	PMM_unlockLPM5();

	//Configure UART
    //SMCLK = 1MHz, Baudrate = 115200
    //UCBRx = 8, UCBRFx = 0, UCBRSx = 0xD6, UCOS16 = 0 (EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION)
    //SMCLK = 1MHz, Baudrate = 9600
    //UCBRx = 6, UCBRFx = 8, UCBRSx = 0x20, UCOS16 = 1 (EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION)
    //http://www.ti.com.cn/cn/lit/ug/slau445i/slau445i.pdf
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_ACLK;
    param.clockPrescalar = 3;
    param.firstModReg = 0;
    param.secondModReg = 0x92;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    if (STATUS_FAIL == EUSCI_A_UART_init(DEBUG_UART_BASE_ADDR, &param)) {
        return 1;
    }

    EUSCI_A_UART_enable(DEBUG_UART_BASE_ADDR);
    return 0;
}

void DEBUG_MAG_PRINT_STRING(char* str) {
    uint8_t i = 0;
    while(i < strlen(str)) {
        EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, str[i]);
        ++i;
    }

    EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, ' ');
    EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, ' ');
    EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, ' ');
    EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, ' ');
}

void DEBUG_MAG_PRINT_NEWLINE() {
    // EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, '\r');
    EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, '\n');
}
