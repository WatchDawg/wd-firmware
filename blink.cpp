#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "driverlib.h"

#include "Ublox_GPS.h"

#include "magnetometer.h"
#include "magnetometer.c"

#include <math.h>

#define PROGRAM_MSG_STOP 0xFF
#define PROGRAM_MSP_ACK 0x41 // 'A'

#define TARGET_COORD_THRESHOLD 10

SemaphoreHandle_t xActiveSemaphore; // used to signal when to gather/display data
SemaphoreHandle_t xReceiveSemaphore; // used to signal when to start receiving/storing user's coordinate list

// [0] = number of coordinates, [1...256] = data
//int32_t num_coords = 3;
//int32_t coords[256] = {422923200, -837135440, 422923200, -837149320, 422911760, -837159110};
int32_t num_coords = 1;
int32_t coords[256] = {422925750, -837166570};
int32_t* target_coord_ptr = coords;
int32_t* end_coord_ptr = coords + 2;

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

void printStr(char* str) {
    while (*str) {
        Serial.write(*str++);
    }
}

void taskPeriodic(void* pvParameters) {
    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        xSemaphoreGive(xActiveSemaphore);
    }
}

SFE_UBLOX_GPS myGPS;

long latitude, longitude, altitude;
//uint8_t SIV;
//uint16_t protocolVersion;
//volatile uint8_t pVH = 0, pVL = 0;

long double factorial(unsigned int n) {
    unsigned int ret = n--;
    for (; n > 1; n--) {
        ret *= n;
    }
    long double ldRet = ret;
    return ldRet;
}

// Taylor series approximation of cosine
long double taylorCos(unsigned int n, long double x) {
    long double ret = 1;
    bool neg = true;
    for (unsigned int i = 2; i <= 2*n; i += 2) {
        long double p = 1;
        for (unsigned int j = 0; j < i; ++j) {
            p *= x;
        }
        if (neg) {
            ret -= p / factorial(i);
        } else {
            ret += p / factorial(i);
        }
        neg = !neg;
    }
    return ret;
}

void updateTargetCoord(int32_t latitude, int32_t longitude) {
    if (target_coord_ptr == NULL) {
        return;
    }

    // Calculate Euclidean distance between current and target coordinates
    long double degLen = 110.25;
    long double currLat = (long double)latitude * 0.0000001;
    long double currLong = (long double)longitude * 0.0000001;
    long double targetLat = (long double)target_coord_ptr[0] * 0.0000001;
    long double targetLong = (long double)target_coord_ptr[1] * 0.0000001;
    long double x = targetLat - currLat;
    long double y = targetLong - currLong * cosl(currLat * (M_PI / 180));
    //long double y = (targetLong - currLong) * taylorCos(2, currLat * (M_PI / 180));
    long double dist = degLen * sqrtl(x*x + y*y);

    if (dist < TARGET_COORD_THRESHOLD) {
        target_coord_ptr += 2;
        if (target_coord_ptr == end_coord_ptr) {
            target_coord_ptr = NULL;
        }
    }

    volatile long double test = atan2l((targetLat - currLat), (targetLong - currLong)) * (180 / M_PI);
    (void)test;
}

void taskActive(void* pvParameters) {
    while (!myGPS.begin(Serial1)) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    for(;;) {
        if(xSemaphoreTake( xActiveSemaphore, ( TickType_t ) 10 ) == pdTRUE ) {

            char tmpStr[] = "RUNNING TASK\r\n";
            printStr(tmpStr);

            volatile int16_t corrected = 0;
            corrected = mag_getHeading();

            volatile int16_t temp = mag_getTemp();

            char strCorrected[8];
            itoa((int)corrected, strCorrected, 10);
            printStr(strCorrected);
            char crlf[] = "\r\n";
            printStr(crlf);

            // Wake up GPS by sending a UART message
            // Loop until we get a nonzero value for each measurement
            while(!(latitude = myGPS.getLatitude()));
            while(!(longitude = myGPS.getLongitude()));
            while(!(altitude = myGPS.getAltitudeMSL()));
            // Put GPS into inactive mode until we communicate with it again
            myGPS.setInactive();

            char strLat[32];
            itoa((long int)latitude, strLat, 10);
            printStr(strLat);
            printStr(crlf);
        }

    }
}

void taskReceiveData(void* pvParameters) {
    for(;;) {
        if(xSemaphoreTake( xReceiveSemaphore, ( TickType_t ) 10 ) == pdTRUE) {
            // By giving to the semaphore,
            //   we can down it at the end of processing to ensure that
            //   no extra count will carry over from reception of coordinates
            xSemaphoreGive(xReceiveSemaphore);

            while(Serial.available() != 0) {
                Serial.read();
            }

            // receive buffer is now empty

            // Reset number of coordinates
            num_coords = 0;
            // Write pointer for coordinates array
            int32_t* coord_ptr = coords;

            volatile int32_t receivedLatitude = 0;
            volatile int32_t receivedLongitude = 0;
            while(1) {
                Serial.write(PROGRAM_MSP_ACK);
                // Receive 1 byte (could be stop byte)
                while(Serial.available() == 0);
                receivedLatitude = Serial.read();
                if (receivedLatitude == PROGRAM_MSG_STOP) {
                    // Stop byte received
                    break;
                }
                receivedLatitude <<= 8;
                // If stop byte was not received, receive other 3 bytes of latitude
                while(Serial.available() < 3);
                receivedLatitude |= Serial.read();
                receivedLatitude <<= 8;
                receivedLatitude |= Serial.read();
                receivedLatitude <<= 8;
                receivedLatitude |= Serial.read();
                *(coord_ptr++) = receivedLatitude;

                Serial.write(PROGRAM_MSP_ACK);
                while(Serial.available() < 4);
                receivedLongitude = Serial.read();
                receivedLongitude <<= 8;
                receivedLongitude |= Serial.read();
                receivedLongitude <<= 8;
                receivedLongitude |= Serial.read();
                receivedLongitude <<= 8;
                receivedLongitude |= Serial.read();
                *(coord_ptr++) = receivedLongitude;

                num_coords++;
            }

            end_coord_ptr = coords + (num_coords * 2);

            while(!(xSemaphoreTake( xReceiveSemaphore, ( TickType_t ) 10 ) == pdTRUE));
        }
    }
}

void taskInit(void* pvParameters) {
    //mag_init();

    Serial.begin(9600);
    //Serial1.begin(9600);

    xActiveSemaphore = xSemaphoreCreateBinary();
    xReceiveSemaphore = xSemaphoreCreateBinary();
    if(xActiveSemaphore == NULL || xReceiveSemaphore == NULL) {
        while(1) {
            //spin;
        }
    }

    updateTargetCoord(422925670, -837149970);

    for(;;);

    xTaskCreate(taskReceiveData, "taskReceiveData", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

//
//    /* FOR BREADBOARD BUTTON TEST */
//    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN0, GPIO_LOW_TO_HIGH_TRANSITION);
//
//    xTaskCreate(taskPeriodic, "taskPeriodic", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
//    xTaskCreate(taskActive, "taskActive", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    vTaskSuspend(NULL);
}


void main(void) {
    // Setup hardware
    prvSetupHardware();

    xTaskCreate(taskInit, "taskInit", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);

    // enable global interrupts
    __enable_interrupt();

    // Start FreeRTOS scheduler
    vTaskStartScheduler();
}

//******************************************************************************
//
//This is the PORT1_VECTOR interrupt vector service routine
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT1_VECTOR)))
#endif
void P1_ISR (void) {
    xSemaphoreGiveFromISR(xActiveSemaphore, NULL);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN0);
}
