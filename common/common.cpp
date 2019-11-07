/*
 * common.c
 *
 *  Created on: Oct 10, 2019
 *      Author: rpash
 */

#include "common.h"
#include <gpio.h>

void gpio_write(io_pin_t pin, uint8_t val) {
    if (val) {
        GPIO_setOutputHighOnPin(pin.port, pin.pin);
    } else {
        GPIO_setOutputLowOnPin(pin.port, pin.pin);
    }
}

uint8_t gpio_read(io_pin_t pin) {
    return GPIO_getInputPinValue(pin.port, pin.pin);
}
