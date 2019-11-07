/*
 * epaper.h
 *
 *  Created on: Oct 10, 2019
 *      Author: rpash
 */

#ifndef WATCHDAWG_EPAPER_H_
#define WATCHDAWG_EPAPER_H_

#include <stdint.h>

// Display resolution
#define EPD_1IN54_WIDTH 200
#define EPD_1IN54_HEIGHT 200

#define EPD_1IN54_FULL 0
#define EPD_1IN54_PART 1

// Forward declaration of opaque display_t
typedef struct {
    struct {
        io_pin_t mosi, sclk, cs, dc, rst, busy;
    } pins;

    uint8_t* full;
    uint16_t height, width, mode;
} display_t;

typedef enum { FONT_LARGE, FONT_MED, FONT_SMALL } font_t;

/**
 * @brief Initialize display, allocate pins, and wakes up display
 *
 * @details Pins based on waveshare E-Ink 1.54in display
 * https://www.waveshare.com/product/modules/oleds-lcds/e-paper/1.54inch-e-paper-module-b.htm
 *
 * @param disp Pointer to display type
 * @param mosi SPI MOSI pin
 * @param sclk SPI CLK pin
 * @param cs SPI CS pin
 * @param dc Data/command selection pin
 * @param rst Reset pin
 * @param busy Busy pin
 */
void display_init(display_t* disp, io_pin_t mosi, io_pin_t sclk, io_pin_t cs,
                  io_pin_t dc, io_pin_t rst, io_pin_t busy, uint16_t width,
                  uint16_t height, uint16_t mode);

/**
 * @brief Destroy display struct, deallocating all memory
 *
 * @param disp Pointer to the display type
 */
void display_destroy(display_t* disp);

/**
 * @brief Wake up display from sleep mode
 *
 * @param disp Pointer to the display type
 */
void display_wakeup(display_t* disp);

/**
 * @brief Put the display in low power sleep mode
 *
 * @param disp Pointer to the display type
 */
void display_sleep(display_t* disp);

/**
 * @brief Display software reset
 *
 * @param disp Pointer to the display type
 */
void display_reset(display_t* disp);

/**
 * @brief Clear display, all pixels turn white
 *
 * @param disp Pointer to the display
 */
void display_clear(display_t* disp);

/**
 * @brief Performs full refresh on display, all pixels turn white.
 *
 * Function is pure. Display is reverted to partial display mode after full
 * refresh.
 *
 * @param disp Pointer to the display
 */
void display_fullrefresh(display_t* disp);

/**
 * @brief Draw the given image on the display
 *
 * @details The image drawn is based on the internal img buffer in RAM of the
 * display. Change those buffers in order to draw something new.
 *
 * @param disp Pointer to display struct
 */
void display_draw_image(display_t* disp);

/**
 * @brief Retreive the currently displayed image.
 *
 * @details Immutable pointer to current image is given.
 *
 * @param disp Pointer to the display
 * @param img_in Pointer will be pointing to image data for buffer
 */
void display_get_image(display_t* disp, uint8_t** full);

/**
 * @brief Get the current image height in pixels
 *
 * @param disp Pointer to display
 * @return uint16_t The image height
 */
uint16_t display_get_image_height(display_t* disp);

/**
 * @brief Get the current image width in pixels
 *
 * @param disp Pointer to display
 * @return uint16_t The image width
 */
uint16_t display_get_image_width(display_t* disp);
/**
 * @brief Draw a line from p1 to p2 on the display
 *
 * @param disp Pointer to the display
 * @param p1 Line point 1
 * @param p2 Line point 2
 */
// void display_draw_line(display_t* disp, point_t p1, point_t p2);

/**
 * @brief Drow a circle on the display
 *
 * @param disp Pointer to the display
 * @param center The center of the circle, in pixels
 * @param radius The radius of the circle, in pixels
 */
// void display_draw_circle(display_t* disp, uint16_t center, uint16_t radius);

/**
 * @brief Draw a rectangle. The points are in counter-clockwise order starting
 * on the top left
 *
 * @param disp Pointer to the dispplay
 * @param p0 Top left point
 * @param p1 Bottom left point
 * @param p2 Bottom right point
 * @param p3 Top right point
 */
// void display_draw_rect(display_t* disp, point_t p0, point_t p1, point_t p2,
// point_t p3);

/**
 * @brief Draw an arc
 *
 * @details This function draws a portion of a circle (a semi-circle) to
 * represent and arc. The center and radius describe the center and radius of
 * the circle. The angle desribes how much of that circle to keep. An angle of
 * 180 will draw a half circle, whereas an angle of 360 will draw a full circle.
 * Start describes where the beginning of the arc is on the circle. Angles are
 * measured on a polar plane where 0 is on the right (the positive x axis) and
 * angles increase going counterclockwise. All angles are in degrees, no
 * fractions are allowed.
 *
 * @param disp A pointer to the current display
 * @param center Center of the circle used for arc (semi-circle)
 * @param radius Radius of the circle used for arc (semi-circle)
 * @param angle How much of the semi circle to keep (angle, degrees)
 * @param start  Beginning angle of semi-circle (degrees)
 */
// void display_draw_arc(display_t* disp,
//                      uint16_t   center,
//                      uint16_t   radius,
//                      uint16_t   angle,
//                      uint16_t   start);

/**
 * @brief Draw text on the screen
 *
 * @param disp A pointer to the display
 * @param msg The message two write
 * @param font The font to use, either large, medium, or small
 * @param start Bottom left point of text
 */
// void display_draw_text(display_t* disp, const char* msg, font_t font, point_t
// start);

#endif /* EPAPER_EPAPER_H_ */
