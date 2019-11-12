/*
 * epaper.c
 *
 *  Created on: Oct 10, 2019
 *      Author: rpash
 */

#include "FreeRTOS.h"
#include "GUI_Paint.h"
#include "driverlib.h"
#include "msp430fr2476.h"
#include <common/common.h>
#include <epaper/display.h>
#include <eusci_b_spi.h>
#include <gpio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <task.h>

const uint16_t DATA_LEN = 15;

static const unsigned char EPD_1IN54_lut_full_update[] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69,
    0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00,
    0xF8, 0xB4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00};

static const unsigned char EPD_1IN54_lut_partial_update[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x13, 0x14, 0x44, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static void delay_ms(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

void display_reset(display_t* disp) {
    gpio_write(disp->pins.rst, 1);
    delay_ms(200); // changed to 200 delay
    gpio_write(disp->pins.rst, 0);
    delay_ms(10);
    gpio_write(disp->pins.rst, 1);
    delay_ms(200); // changed to 200 delay
}

static void display_send_cmd(display_t* disp, uint8_t cmd) {
    gpio_write(disp->pins.dc, 0);
    gpio_write(disp->pins.cs, 0);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, cmd);
    gpio_write(disp->pins.cs, 1);
}

static void display_send_data(display_t* disp, uint8_t data) {
    gpio_write(disp->pins.dc, 1);
    gpio_write(disp->pins.cs, 0);
    EUSCI_B_SPI_transmitData(EUSCI_B1_BASE, data);
    gpio_write(disp->pins.cs, 1);
}

static void display_wait_on_busy(display_t* disp) {
    volatile uint8_t busy;
    /*while(1) {
        busy = gpio_read(disp->pins.busy);
        if(busy == 1) {
            return;
        }
    }*/
    // vTaskDelay(pdMS_TO_TICKS(ms))delay_ms(200);
    busy = gpio_read(disp->pins.busy);
    while (busy == 1) { // LOW: idle, HIGH: busy
        busy = gpio_read(disp->pins.busy);
        delay_ms(100);
    }
}

static void display_set_window(display_t* disp, uint16_t Xstart,
                               uint16_t Ystart, uint16_t Xend, uint16_t Yend) {
    display_send_cmd(disp, 0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    display_send_data(disp, ((Xstart >> 3) & 0xFF));
    display_send_data(disp, ((Xend >> 3) & 0xFF));

    display_send_cmd(disp, 0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    display_send_data(disp, (Ystart & 0xFF));
    display_send_data(disp, ((Ystart >> 8) & 0xFF));
    display_send_data(disp, (Yend & 0xFF));
    display_send_data(disp, ((Yend >> 8) & 0xFF));
}

static void display_set_cursor(display_t* disp, uint16_t Xstart,
                               uint16_t Ystart) {
    display_send_cmd(disp, 0x4E); // SET_RAM_X_ADDRESS_COUNTER
    display_send_data(disp, ((Xstart >> 3) & 0xFF));

    display_send_cmd(disp, 0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    display_send_data(disp, (Ystart & 0xFF));
    display_send_data(disp, ((Ystart >> 8) & 0xFF));
}

static void display_turn_on(display_t* disp) {
    display_send_cmd(disp, 0x22); // DISPLAY_UPDATE_CONTROL_2
    display_send_data(disp, 0xC4);
    display_send_cmd(disp, 0x20); // MASTER_ACTIVATION
    display_send_cmd(disp, 0xFF); // TERMINATE_FRAME_READ_WRITE

    display_wait_on_busy(disp);
}

static void display_init_spi(display_t* disp) {
    // Configure MOSI as output pin
    GPIO_setAsPeripheralModuleFunctionOutputPin(disp->pins.mosi.port,
                                                disp->pins.mosi.pin,
                                                GPIO_PRIMARY_MODULE_FUNCTION);

    // Configure CS1 as output pin
    gpio_write(disp->pins.cs, 0);
    GPIO_setAsOutputPin(disp->pins.cs.port, disp->pins.cs.pin);

    // Configure DC as output pin
    gpio_write(disp->pins.dc, 0);
    GPIO_setAsOutputPin(disp->pins.dc.port, disp->pins.dc.pin);

    // Configure RST as output pin
    gpio_write(disp->pins.rst, 1);
    GPIO_setAsOutputPin(disp->pins.rst.port, disp->pins.rst.pin);

    // Configure EPD busy as input pin
    GPIO_setAsInputPin(disp->pins.busy.port, disp->pins.busy.pin);

    // Configure SCLK as output pin
    GPIO_setAsPeripheralModuleFunctionOutputPin(disp->pins.sclk.port,
                                                disp->pins.sclk.pin,
                                                GPIO_PRIMARY_MODULE_FUNCTION);

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();

    // Configure SPI
    EUSCI_B_SPI_initMasterParam config = {0};
    config.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    config.clockSourceFrequency = CS_getSMCLK();
    config.desiredSpiClock = 16000000;
    config.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    config.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    config.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    config.spiMode = EUSCI_B_SPI_3PIN;

    EUSCI_B_SPI_initMaster(EUSCI_B1_BASE, &config);

    EUSCI_B_SPI_enable(EUSCI_B1_BASE);
}

void display_init(display_t* disp, io_pin_t mosi, io_pin_t sclk, io_pin_t cs,
                  io_pin_t dc, io_pin_t rst, io_pin_t busy, uint16_t width,
                  uint16_t height, uint16_t mode) {
    disp->pins.mosi = mosi;
    disp->pins.sclk = sclk;
    disp->pins.cs = cs;
    disp->pins.dc = dc;
    disp->pins.rst = rst;
    disp->pins.busy = busy;

    disp->width = width;
    disp->height = height;
    disp->mode = mode;
    disp->full = (uint8_t *) malloc((width / 8) * height);

    // Init GPIO and SPI
    display_init_spi(disp);

    // Performs full refresh and clears the screen
    disp->mode = EPD_1IN54_FULL;
    display_wakeup(disp);

    // Initializes to user selected mode
    disp->mode = mode;
    display_wakeup(disp);
}

void display_destroy(display_t* disp) {
    if (disp->full) {
        free(disp->full);
        disp->full = NULL;
    }
}

void display_wakeup(display_t* disp) {
    display_reset(disp);

    uint16_t i;

    display_send_cmd(disp, 0x01); // DRIVER_OUTPUT_CONTROL
    display_send_data(disp, ((disp->height - 1) & 0xFF));
    display_send_data(disp, (((disp->height - 1) >> 8) & 0xFF));
    display_send_data(disp, 0x00); // GD = 0; SM = 0; TB = 0;

    display_send_cmd(disp, 0x0C); // BOOSTER_SOFT_START_CONTROL
    display_send_data(disp, 0xD7);
    display_send_data(disp, 0xD6);
    display_send_data(disp, 0x9D);

    display_send_cmd(disp, 0x2C);  // WRITE_VCOM_REGISTER
    display_send_data(disp, 0xA8); // VCOM 7C

    display_send_cmd(disp, 0x3A);  // SET_DUMMY_LINE_PERIOD
    display_send_data(disp, 0x1A); // 4 dummy lines per gate

    display_send_cmd(disp, 0x3B);  // SET_GATE_TIME
    display_send_data(disp, 0x08); // 2us per line

    display_send_cmd(disp, 0x11);
    display_send_data(disp, 0x03);

    // set the look-up table register
    display_send_cmd(disp, 0x32);
    if (disp->mode == EPD_1IN54_FULL) {
        for (i = 0; i < 30; i++) {
            display_send_data(disp, EPD_1IN54_lut_full_update[i]);
        }
        display_clear(disp);
    } else if (disp->mode == EPD_1IN54_PART) {
        for (i = 0; i < 30; i++) {
            display_send_data(disp, EPD_1IN54_lut_partial_update[i]);
        }
    } else {
        return;
    }
}

void display_fullrefresh(display_t* disp) {
    uint8_t mode = disp->mode;
    // Performs full refresh and clears the screen
    disp->mode = EPD_1IN54_FULL;
    display_wakeup(disp);

    // Initializes to user selected mode
    disp->mode = mode;
    display_wakeup(disp);
}

void display_sleep(display_t* disp) {
    display_send_cmd(disp, 0x10); // enter deep sleep
    display_send_data(disp, 0x01);
    // delay_ms(100);
}

void display_clear(display_t* disp) {
    uint16_t width = disp->width & 8 == 0 ? disp->width / 8 : disp->width + 1;
    uint16_t height = disp->height;
    uint16_t i, j;

    display_set_window(disp, 0, 0, disp->width, disp->height);
    for (j = 0; j < height; j++) {
        display_set_cursor(disp, 0, j);
        display_send_cmd(disp, 0x24);
        for (i = 0; i < width; i++) {
            display_send_data(disp, 0XFF);
        }
    }
    display_turn_on(disp);
}

void display_draw_image(display_t* disp) {
    uint16_t width = disp->width / 8;
    uint16_t height = disp->height;
    uint16_t i, j;
    uint32_t Addr = 0;

    display_set_window(disp, 0, 0, disp->width, disp->height);
    for (j = 0; j < height; j++) {
        display_set_cursor(disp, 0, j);
        display_send_cmd(disp, 0x24);
        for (i = 0; i < width; i++) {
            Addr = i + j * width;
            display_send_data(disp, disp->full[Addr]);
        }
    }
    display_turn_on(disp);
}

void display_get_image(display_t* disp, uint8_t** full) { *full = disp->full; }

uint16_t display_get_image_height(display_t* disp) { return disp->height; }

uint16_t display_get_image_width(display_t* disp) { return disp->width; }
