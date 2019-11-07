/*
 * common.h
 *
 *  Created on: Oct 10, 2019
 *      Author: rpash
 */

#ifndef COMMON_COMMON_H_
#define COMMON_COMMON_H_

#include <stdint.h>

typedef struct {
    uint8_t port;
    uint16_t pin;
} io_pin_t;

typedef struct {
    uint16_t x;
    uint16_t y;
} point_t;

void gpio_write(io_pin_t pin, uint8_t val);

uint8_t gpio_read(io_pin_t pin);


#endif /* COMMON_COMMON_H_ */
