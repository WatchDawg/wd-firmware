#include "FreeRTOS.h"
#include "task.h"

#include "driverlib.h"

#include "Ublox_GPS.h"

#include "magnetometer.h"
#include "magnetometer.c"

#ifdef __cplusplus
extern "C"{
#endif

void vApplicationIdleHook( void )
{
    __bis_SR_register( LPM4_bits + GIE );
    __no_operation();
}

void vApplicationSetupTimerInterrupt( void ) {
    const unsigned short usACLK_Frequency_Hz = 32768;

    /* Ensure the timer is stopped. */
    TA0CTL = 0;

    /* Run the timer from the ACLK. */
    TA0CTL = TASSEL_1;

    /* Clear everything to start with. */
    TA0CTL |= TACLR;

    /* Set the compare match value according to the tick rate we want. */
    TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

    /* Enable the interrupts. */
    TA0CCTL0 = CCIE;

    /* Start up clean. */
    TA0CTL |= TACLR;

    /* Up mode. */
    TA0CTL |= MC_1;
}

#ifdef __cplusplus
}
#endif

static void prvSetupHardware( void ) {
    /* Stop Watchdog timer. */
    WDT_A_hold( __MSP430_BASEADDRESS_WDT_A__ );

    /* Set all GPIO pins to output and low. */
//    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setOutputLowOnPin( GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setOutputLowOnPin( GPIO_PORT_PJ, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 | GPIO_PIN8 | GPIO_PIN9 | GPIO_PIN10 | GPIO_PIN11 | GPIO_PIN12 | GPIO_PIN13 | GPIO_PIN14 | GPIO_PIN15 );
//    GPIO_setAsOutputPin( GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setAsOutputPin( GPIO_PORT_P3, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );
//    GPIO_setAsOutputPin( GPIO_PORT_PJ, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 | GPIO_PIN8 | GPIO_PIN9 | GPIO_PIN10 | GPIO_PIN11 | GPIO_PIN12 | GPIO_PIN13 | GPIO_PIN14 | GPIO_PIN15 );

    // Change DCO clock freq to 16MHz
    CS_initFLLSettle(16000, 487);

    //Set ACLK = REFOCLK with clock divider of 1
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);
    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);

    /* Disable the GPIO power-on default high-impedance mode. */
    PMM_unlockLPM5();
}

SFE_UBLOX_GPS myGPS;

long latitude;
long longitude;
long altitude;
uint8_t SIV;

uint16_t protocolVersion;
volatile uint8_t pVH = 0, pVL = 0;


void printStr(char* str) {
    while (*str) {
        Serial.write(*str++);
    }
}

void taskGPSInterface(void* pvParameters) {
    Serial.begin(9600);
    Serial1.begin(9600);
    while (!myGPS.begin(Serial1)) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    //configASSERT(myGPS.setUART1Output(COM_TYPE_UBX)); //Set the UART port to output UBX only
    //configASSERT(myGPS.saveConfiguration()); //Save the current settings to flash and BBR
    pVL = myGPS.getProtocolVersionLow();
    vTaskDelay(pdMS_TO_TICKS(100));
    pVH = myGPS.getProtocolVersionHigh();
    vTaskDelay(pdMS_TO_TICKS(100));
    protocolVersion = (pVL & 0x00FF) | (pVH & 0xFF) << 8;
    for (;;) {
        latitude = myGPS.getLatitude();
        longitude = myGPS.getLongitude();
        altitude = myGPS.getAltitude();
        SIV = myGPS.getSIV();
    }
}

void taskMag(void* pvParameters) {
    Serial.begin(9600);
    volatile int16_t corrected = 0;
    for(;;) {
        corrected = mag_getHeading();
        char strCorrected[8];
        itoa((int)corrected, strCorrected, 10);
        printStr(strCorrected);
        char tmp[] = "\r";
        printStr(tmp);
        char tmp2[] = "\n";
        printStr(tmp2);
    }
}

void main(void) {
    // Setup hardware
    prvSetupHardware();

    mag_init();

    //Create light-blinking task
    //xTaskCreate(taskGPSInterface, "taskGPSInterface", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(taskMag, "taskMag", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();
}
