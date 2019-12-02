#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "driverlib.h"
#include "magnetometer.h"
#include "Ublox_GPS.h"
#include "GUI_Paint.h"
#include "display.h"

#include <math.h>

#define PROGRAM_MSG_STOP 0xFF
#define PROGRAM_MSP_ACK 0x41 // 'A'

#define REQ_SIV                5
#define AVG_MEAS_CNT           3
#define TARGET_COORD_THRESHOLD 30

#define ADC_SAMPLES            5
#define BATT_VOLT_100P         3.75f
#define BATT_VOLT_75P          3.65f
#define BATT_VOLT_50P          3.55f
#define BATT_VOLT_25P          3.50f

SemaphoreHandle_t xActiveSemaphore; // used to signal when to gather/display data
SemaphoreHandle_t xReceiveSemaphore; // used to signal when to start receiving/storing user's coordinate list
SemaphoreHandle_t xSPISemaphore = NULL;

TaskHandle_t activeHandle;
TaskHandle_t receiveHandle;

// [0] = number of coordinates, [1...256] = data
//int32_t num_coords = 3;
//int32_t coords[256] = {422923200, -837135440, 422923200, -837149320, 422911760, -837159110};
int32_t num_coords = 3;
int32_t coords[256] = {422925750, -837166570, 422916720, -837166310, 422916910, -837150770};
int32_t* target_coord_ptr = coords;
int32_t* end_coord_ptr = coords + 6;

long double distance = 0;
int32_t gps_heading = 0;
int32_t mag_heading = 0;
int32_t dir_heading = 0;
int16_t temp = 0;
display_t* disp;

#ifdef __cplusplus
extern "C"{
#endif

void vApplicationIdleHook( void )
{
    __bis_SR_register( LPM3_bits + GIE );
    __no_operation();
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    signed char *pcTaskName )
{
    configASSERT(false);
}

void vApplicationMallocFailedHook( void )
{
    configASSERT(false);
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

void taskTest(void* pvParameters) {
    vTaskSuspend(NULL);
}

void taskPeriodic(void* pvParameters) {
    vTaskSuspend(NULL);
}

SFE_UBLOX_GPS myGPS;

long latitude, longitude, altitude;
uint8_t siv;

void updateTargetCoord() {
    if (target_coord_ptr == NULL) {
        return;
    }

    // Calculate Euclidean distance between current and target coordinates
    long double degLen = 110.25;
    long double currLat = (long double)latitude * 0.0001;
    long double currLong = (long double)longitude * 0.0001;
    long double targetLat = (long double)target_coord_ptr[0] * 0.0001;
    long double targetLong = (long double)target_coord_ptr[1] * 0.0001;
    long double x = targetLat - currLat;
    long double y = (targetLong - currLong) * cosl(currLat * (M_PI / 180));

    distance = degLen * sqrtl(x*x + y*y);

    if (distance < TARGET_COORD_THRESHOLD) {
        target_coord_ptr += 2;
        if (target_coord_ptr == end_coord_ptr) {
            target_coord_ptr = NULL;
        }
        updateTargetCoord();
    }

    long double heading = atan2l((targetLat - currLat), (targetLong - currLong)) * (180.0 / M_PI);
    gps_heading = 90 - (heading + 0.5);
    if (gps_heading < 0) {
        gps_heading = 360 + gps_heading;
    }
}

void taskActive(void* pvParameters) {
//    while (!myGPS.begin(Serial1)) {
//        vTaskDelay(pdMS_TO_TICKS(100));
//    }

//    while (!myGPS.setUART1Output(COM_TYPE_UBX)) {
//        vTaskDelay(pdMS_TO_TICKS(100));
//    }
//    while (!myGPS.enableAllGNSS()) {
//        vTaskDelay(pdMS_TO_TICKS(100));
//    }
//    while (!myGPS.saveConfiguration()) {
//        vTaskDelay(pdMS_TO_TICKS(100));
//    }

    for(;;) {
        if(xSemaphoreTake( xActiveSemaphore, ( TickType_t ) 10 ) == pdTRUE ) {

            volatile uint8_t cnt = 0;
            volatile long long runningLat = 0, runningLong = 0;
            volatile int8_t month = 0, day = 0, minute = 0, hour = 0;

            char tmpStr[] = "---\r\n";
            printStr(tmpStr);

            mag_heading = mag_getHeading();
            temp = mag_getTemp();

            char strHeading[8];
            itoa((int)mag_heading, strHeading, 10);
            printStr(strHeading);
            char crlf[] = "\r\n";
            printStr(crlf);

            // Wake up GPS by sending a UART message
            // Loop until we get a nonzero value for each measurement
//            siv = myGPS.getSIV();
//            while(siv < REQ_SIV) {
//                vTaskDelay(pdMS_TO_TICKS(10));
//                siv = myGPS.getSIV();
//            }
//            while(!(latitude = myGPS.getLatitude()) || cnt < AVG_MEAS_CNT) {
//                if (latitude) {
//                    runningLat += latitude;
//                    cnt++;
//                }
//                vTaskDelay(pdMS_TO_TICKS(10));
//            }
//            latitude = (long)(runningLat / AVG_MEAS_CNT);
//            cnt = 0;
//            while(!(longitude = myGPS.getLongitude()) || cnt < AVG_MEAS_CNT) {
//                if (longitude) {
//                    runningLong += longitude;
//                    cnt++;
//                }
//                vTaskDelay(pdMS_TO_TICKS(10));
//            }
//            longitude = (long)(runningLong / AVG_MEAS_CNT);

            while(!(month = myGPS.getMonth())) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            while(!(day = myGPS.getDay())) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
//            while(!(hour = myGPS.getHour())) {
//                vTaskDelay(pdMS_TO_TICKS(10));
//            }
//            // Convert hour from UTC to EST
//            if ((hour - 5) < 0) {
//                --day;
//            }
//            hour = (hour + 19) % 24;
//
//
//            while(!(minute = myGPS.getMinute())) {
//                vTaskDelay(pdMS_TO_TICKS(10));
//            }
            latitude = myGPS.getLatitude();
            longitude = myGPS.getLongitude();
            // Put GPS into inactive mode until we communicate with it again
            myGPS.setInactive();

            updateTargetCoord();

            dir_heading = gps_heading - mag_heading;
            if (dir_heading < 0) {
                dir_heading += 360;
            }

            int16_t adc_sum = 0;
            uint8_t adc_cnt = 0;
            while (adc_cnt < ADC_SAMPLES) {
                ADC_startConversion(ADC_BASE,
                                    ADC_SINGLECHANNEL);
                while (ADC_isBusy(ADC_BASE) == ADC_BUSY);
                int16_t adc_result = ADC_getResults(ADC_BASE);
                if (adc_result > 0) {
                    adc_sum += ADC_getResults(ADC_BASE);
                    adc_cnt++;
                }
            }
            // (ADC sum / 5) * 3.3 V * 2 / 2^10
            volatile float batt_volt = ((float)(adc_sum) * 0.00128906f) + 0.55f;
            (void)batt_volt;
            volatile int batt_percent = 0;
            if (batt_volt >= BATT_VOLT_100P) {
                batt_percent = 100;
            } else if (batt_volt >= BATT_VOLT_75P) {
                batt_percent = 75;
            } else if (batt_volt >= BATT_VOLT_50P) {
                batt_percent = 50;
            } else if (batt_volt >= BATT_VOLT_25P) {
                batt_percent = 25;
            } else {
                batt_percent = 0;
            }
//            volatile int batt_percent = trunc((((batt_volt - BATT_MIN_VOLT) / (BATT_MAX_VOLT - BATT_MIN_VOLT)) * 100.f) + 0.5f);
//            batt_percent = (batt_percent < 0) ? 0 : batt_percent;
//            batt_percent = (batt_percent > 100) ? 100 : batt_percent;

            char strBuf[32];
            itoa((long int)gps_heading, strBuf, 10);
            printStr(strBuf);
            printStr(crlf);

            itoa((long int)dir_heading, strBuf, 10);
            printStr(strBuf);
            printStr(crlf);

            itoa((long int)trunc(distance), strBuf, 10);
            printStr(strBuf);
            printStr(crlf);

            itoa((long int)((target_coord_ptr - coords) / 2), strBuf, 10);
            printStr(strBuf);
            printStr(crlf);

            Paint_DrawTime(5, 175, hour, minute, &Font20, WHITE, BLACK);
            Paint_DrawDate(115, 175, month, day, &Font20, WHITE, BLACK);
            Paint_DrawDistance(125, 20+20, (int)trunc(distance));
            Paint_DrawTemp(140, 55+15, temp);
            Paint_DrawBattery(162, 5, batt_percent); //default 90, CHANGE TO ACTUAL VALUE
            Paint_DrawLatLon(10, 125, latitude, longitude);

            Paint_ClearWindows(10, 10, 120, 120, WHITE);
            Paint_DrawCircle(65, 65, 55, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
            Paint_DrawArrowd(360 - dir_heading);
            Paint_DrawNorth(mag_heading);

            while (xSemaphoreTake(xSPISemaphore, (TickType_t)10) == pdFALSE);
            // Configure MOSI as output pin
            GPIO_setAsPeripheralModuleFunctionOutputPin(disp->pins.mosi.port,
                                                        disp->pins.mosi.pin,
                                                        GPIO_PRIMARY_MODULE_FUNCTION);
            __delay_cycles(100);
            display_wakeup(disp);
            display_draw_image(disp);
            display_sleep(disp);

            // Configure MOSI as input pin
            GPIO_setAsInputPin(disp->pins.mosi.port, disp->pins.mosi.pin);
            xSemaphoreGive(xSPISemaphore);

        }
        RTC_start(RTC_BASE, RTC_CLOCKSOURCE_ACLK);
        vTaskSuspend(activeHandle);
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
        vTaskSuspend(receiveHandle);
    }
}

void taskFullRefresh(void* pvParameters) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(100000));
        while (xSemaphoreTake(xSPISemaphore, (TickType_t)10) == pdFALSE);

        // Configure MOSI as output pin
        GPIO_setAsPeripheralModuleFunctionOutputPin(disp->pins.mosi.port,
                                                    disp->pins.mosi.pin,
                                                    GPIO_PRIMARY_MODULE_FUNCTION);
        __delay_cycles(100);

        display_fullrefresh(disp);
        Paint_Clear(WHITE);
        Paint_DrawOutline();
        display_draw_image(disp);

        // put epaper into sleep mode
        display_sleep(disp);

        // Configure MOSI as input pin
        GPIO_setAsInputPin(disp->pins.mosi.port, disp->pins.mosi.pin);

        xSemaphoreGive(xSPISemaphore);
    }
}


void taskInit(void* pvParameters) {
    // initialize Magnetometer
    mag_init();

    // initialize Real-Time Clock
    RTC_init(RTC_BASE, 16384, RTC_CLOCKPREDIVIDER_10);
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);

    // initialize ADC for battery level
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN0,
            GPIO_TERNARY_MODULE_FUNCTION);

    ADC_init(ADC_BASE,
             ADC_SAMPLEHOLDSOURCE_SC,
             ADC_CLOCKSOURCE_ADCOSC,
             ADC_CLOCKDIVIDER_1);

    ADC_enable(ADC_BASE);

    ADC_setupSamplingTimer(ADC_BASE,
                           ADC_CYCLEHOLD_16_CYCLES,
                           ADC_MULTIPLESAMPLESDISABLE);

    ADC_configureMemory(ADC_BASE,
                        ADC_INPUT_A0,
                        ADC_VREFPOS_AVCC,
                        ADC_VREFNEG_AVSS);

    // initialize Epaper
    disp = (display_t*)malloc(sizeof(display_t));
    uint8_t* Full_Image;
    display_init(disp, (io_pin_t){GPIO_PORT_P3, GPIO_PIN2}, // mosi
                 (io_pin_t){GPIO_PORT_P3, GPIO_PIN5},       // sclk
                 (io_pin_t){GPIO_PORT_P3, GPIO_PIN6},       // cs
                 (io_pin_t){GPIO_PORT_P6, GPIO_PIN1},       // dc
                 (io_pin_t){GPIO_PORT_P6, GPIO_PIN2},       // rst
                 (io_pin_t){GPIO_PORT_P4, GPIO_PIN7},       // busy
                 200,                                       // width
                 200,                                       // height
                 EPD_1IN54_PART); // mode EPD_1IN54_PART

    vTaskDelay(pdMS_TO_TICKS(500));

    display_get_image(disp, &Full_Image);

    Paint_NewImage(Full_Image, EPD_1IN54_WIDTH, EPD_1IN54_HEIGHT, 270, WHITE);
    Paint_Clear(WHITE);
    Paint_DrawOutline();
    display_draw_image(disp);

    // put epaper into sleep mode
    display_sleep(disp);
    // Configure MOSI as input pin
    GPIO_setAsInputPin(disp->pins.mosi.port, disp->pins.mosi.pin);



    // create semaphores for taskActive, taskReceive, and the Epaper
    xActiveSemaphore = xSemaphoreCreateBinary();
    xReceiveSemaphore = xSemaphoreCreateBinary();
    xSPISemaphore = xSemaphoreCreateBinary();
    if(xActiveSemaphore == NULL || xReceiveSemaphore == NULL || xSPISemaphore == NULL) {
        while(1) {
            //spin;
        }
    }
    // Explicitly give display semaphore
    xSemaphoreGive(xSPISemaphore);

    //updateTargetCoord(422925670, -837149970);

    /* FOR BREADBOARD BUTTON  */
//    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN0);
//    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN0, GPIO_LOW_TO_HIGH_TRANSITION);

    // enable GPS and Backchannel UARTs
    Serial.begin(9600);
    Serial1.begin(9600);
    while (!myGPS.begin(Serial1)) {
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
    while (!myGPS.setUART1Output(COM_TYPE_UBX)) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    while (!myGPS.enableAllGNSS()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    while (!myGPS.saveConfiguration()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    myGPS.setInactive();

    xTaskCreate(taskReceiveData, "taskReceiveData", configMINIMAL_STACK_SIZE, NULL, 2, &receiveHandle);
    xTaskCreate(taskPeriodic, "taskPeriodic", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(taskTest, "taskTest", configMINIMAL_STACK_SIZE * 4, NULL, 1, NULL);
    xTaskCreate(taskActive, "taskActive", configMINIMAL_STACK_SIZE * 4, NULL, 1, &activeHandle);
    xTaskCreate(taskFullRefresh, "FullRefresh", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskSuspend(receiveHandle);
    vTaskSuspend(activeHandle);


    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_ACLK);
    __bis_SR_register( LPM3_bits + GIE);
    __no_operation();

    vTaskSuspend(NULL);
}


void main(void) {

    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);

    // Setup hardware
    prvSetupHardware();
    
    xTaskCreate(taskInit, "taskInit", 2 * configMINIMAL_STACK_SIZE, NULL, 3, NULL);

    // enable global interrupts
    __enable_interrupt();

    // Start FreeRTOS scheduler
    vTaskStartScheduler();
}

//******************************************************************************
//
// RTC and PORT1_VECTOR interrupt vector service routine
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_VECTOR
#pragma vector=PORT1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(RTC_VECTOR)))
__attribute__((interrupt(PORT1_VECTOR)))
#endif
void RTC_ISR (void) {
    RTC_stop(RTC_BASE);
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
    //GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN0);

    if (xActiveSemaphore) {
        xSemaphoreGiveFromISR(xActiveSemaphore, NULL);
        vTaskResume(activeHandle);

        __bic_SR_register( LPM3_bits); // bit clear LPM3 bits
        __bis_SR_register( GIE ); // bit set interrupt enable
        __low_power_mode_off_on_exit();
    }
}

