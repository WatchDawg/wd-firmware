#include "magnetometer.h"

void mag_writeReg(uint8_t addr, uint8_t data) {
    GPIO_setOutputLowOnPin(SPI_CS_PORT, SPI_CS_PIN);
    EUSCI_B_SPI_transmitData(SPI_BASE_ADDR, addr);
    while(EUSCI_B_SPI_isBusy(SPI_BASE_ADDR) == EUSCI_B_SPI_BUSY);
    EUSCI_B_SPI_transmitData(SPI_BASE_ADDR, data);
    while(EUSCI_B_SPI_isBusy(SPI_BASE_ADDR) == EUSCI_B_SPI_BUSY);
    GPIO_setOutputHighOnPin(SPI_CS_PORT, SPI_CS_PIN);
}

uint8_t mag_readReg(uint8_t addr) {
	uint8_t data = 0;

    GPIO_setOutputLowOnPin(SPI_CS_PORT, SPI_CS_PIN);
    EUSCI_B_SPI_transmitData(SPI_BASE_ADDR, addr);
    while(EUSCI_B_SPI_isBusy(SPI_BASE_ADDR) == EUSCI_B_SPI_BUSY);
    //while(!(INTERRUPT_FLAG_REG&UCRXIFG0));
    data = EUSCI_B_SPI_receiveData(SPI_BASE_ADDR);
    EUSCI_B_SPI_transmitData(SPI_BASE_ADDR, SPI_READ_MASK);
    while(EUSCI_B_SPI_isBusy(SPI_BASE_ADDR) == EUSCI_B_SPI_BUSY);
    //while(!(INTERRUPT_FLAG_REG&UCRXIFG0));
    data = EUSCI_B_SPI_receiveData(SPI_BASE_ADDR);
    GPIO_setOutputHighOnPin(SPI_CS_PORT, SPI_CS_PIN);
    return data;
}

int16_t mag_readAxis(uint8_t axis) {
    volatile uint8_t requestDataLow = SPI_READ_MASK | 0x28;
    volatile uint8_t requestDataHigh = SPI_READ_MASK | 0x29;
    volatile uint8_t receiveDataLow = 0;
    volatile uint8_t receiveDataHigh = 0;
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
    receiveDataLow = mag_readReg(SPI_READ_MASK | requestDataLow);

    // GET HIGH BITS
    receiveDataHigh = mag_readReg(SPI_READ_MASK | requestDataHigh);

    data = ((int16_t)((uint16_t)(receiveDataHigh << 8) | receiveDataLow));
    return data;
}

void mag_setSingleShotMode() {
	mag_writeReg(0x22, 0x01);
}

void mag_calibrationStep() {
    int16_t x = mag_readAxis('x');
    int16_t y = mag_readAxis('y');
    mag_setSingleShotMode();

    // TODO: find better way of doing this
    volatile unsigned i = 0;
    for(i = 0; i < 8000; ++i) { }

    if(x > calData.maxX) calData.maxX = x;
    if(x < calData.minX) calData.minX = x;

    if(y > calData.maxY) calData.maxY = y;
    if(y < calData.minY) calData.minY = y;
}

void mag_exitCalibration() {
    calData.offsetX = (calData.minX+calData.maxX)/2;
    calData.offsetY = (calData.minY+calData.maxY)/2;

    calData.deltaX = (calData.maxX-calData.minX)/2;
    calData.deltaY = (calData.maxY-calData.minY)/2;
    calData.deltaAvg = (calData.deltaX + calData.deltaY)/2;
    calData.scaleX = (float)(calData.deltaAvg) / (float)(calData.deltaX);
    calData.scaleY = (float)(calData.deltaAvg) / (float)(calData.deltaY);	
}

void mag_calibrate() {
    calData.maxX = -2048;
    calData.minX = 0;
    calData.maxY = -2048;
    calData.minY = 0;
    uint32_t i = 0;
    while(i < 4000) {
        mag_calibrationStep();
        ++i;
    }
    mag_exitCalibration();
}

void mag_initMag() {
	//mag_writeReg(0x20, 0x7C);
    mag_writeReg(0x20, 0x80); // sets temp bit
	mag_writeReg(0x21, 0x00);
	mag_writeReg(0x22, 0x01);
	mag_writeReg(0x23, 0x0C);
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

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

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
	mag_initSPI();
	mag_initMag();
	mag_calibrate();
}

int16_t mag_getHeading() {
    volatile int16_t corrected = 0;
    volatile int16_t x = mag_readAxis('x');
    volatile int16_t y = mag_readAxis('y');
    mag_setSingleShotMode();

//    volatile int16_t x_corrected = (x-calData.offsetX)/calData.scaleX;
//    volatile int16_t y_corrected = (y-calData.offsetY)/calData.scaleY;

    long double x_ldbl = (x-calData.offsetX)/calData.scaleX;
    long double y_ldbl = (y-calData.offsetY)/calData.scaleY;

    volatile long double corrected_ldbl = 180 * (atan2l(y_ldbl, x_ldbl))/M_PI;
//    corrected = 180 * ((_IQ12toF(_IQ12atan2(_IQ12(y_corrected),_IQ12(x_corrected))))/M_PI);
//    if(corrected < 0) {
//        corrected += 360;
//    }
//    return corrected;
    if(corrected_ldbl < 0) {
        corrected_ldbl += 360;
    }
    corrected = corrected_ldbl+0.5;
    return corrected;
}

int16_t mag_getTemp() {
    volatile int16_t temp;
    volatile uint8_t receiveDataLow = 0;
    volatile uint8_t receiveDataHigh = 0;

    // GET LOW BITS
    receiveDataLow = mag_readReg(SPI_READ_MASK | TEMP_OUT_L);

    // GET HIGH BITS
    receiveDataHigh = mag_readReg(SPI_READ_MASK | TEMP_OUT_H);

    temp = ((int16_t)((uint16_t)(receiveDataHigh << 8) | receiveDataLow));
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

/* debug functions that use the USB0 backchannel */
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
    EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, '\r');
    EUSCI_A_UART_transmitData(DEBUG_UART_BASE_ADDR, '\n');	
}