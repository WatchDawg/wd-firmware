/*
 * Energia_lib.h
 *
 *  Created on: Oct 9, 2019
 *      Author: evan
 */

#ifndef UBLOX_GPS_ENERGIA_LIB_H_
#define UBLOX_GPS_ENERGIA_LIB_H_


#include <stdint.h>
#include <msp430fr2476.h>

#ifdef __cplusplus
extern "C"{
#endif

#define BV(x) (1 << (x))

#define F_CPU 16000000L

#define NOT_A_PORT 0
#define NOT_A_PIN 0
#define NOT_ON_TIMER 0
#define NOT_ON_ADC 0xFF

#define HIGH 0x1
#define LOW  0x0

#define LSBFIRST 0
#define MSBFIRST 1

#define RISING 0
#define FALLING 1
#define CHANGE 2

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define INPUT_PULLDOWN 0x4
#define PORT_SELECTION0 0x10
#define PORT_SELECTION1 0x20


#define true 0x1
#define false 0x0

static const uint8_t DEBUG_UARTRXD = 42;  /* Receive  Data (RXD) at P1.5 */
static const uint8_t DEBUG_UARTTXD = 41;  /* Transmit Data (TXD) at P1.4 */
static const uint8_t AUX_UARTRXD = 3;     /* Receive  Data (RXD) at P2.5 */
static const uint8_t AUX_UARTTXD = 4;     /* Transmit Data (TXD) at P2.6 */
#define DEBUG_UARTRXD_SET_MODE (PORT_SELECTION0 | INPUT)
#define DEBUG_UARTTXD_SET_MODE (PORT_SELECTION0 | OUTPUT)
#define AUX_UARTRXD_SET_MODE (PORT_SELECTION0 | INPUT)
#define AUX_UARTTXD_SET_MODE (PORT_SELECTION0 | OUTPUT)
#define DEBUG_UART_MODULE_OFFSET 0x00
#define AUX_UART_MODULE_OFFSET 0x20
#define SERIAL1_AVAILABLE 1

#define digitalPinToPort(P)    ( digital_pin_to_port[P] )
#define digitalPinToBitMask(P) ( digital_pin_to_bit_mask[P] )
#define digitalPinToTimer(P)   ( digital_pin_to_timer[P] )
#define digitalPinToADCIn(P)      ( digital_pin_to_analog_in[P] )
#define portDirRegister(P)     ( (volatile uint8_t *)( port_to_dir[P]) )
/*
 * We either of the compination   PxSEL and PxSEL2   or   PxSEL0 and PxSEL1
 * So we can remap  PxSEL and PxSEL2   to   PxSEL0 and PxSEL1
*/
#define portSelRegister(P)     ( (volatile uint8_t *)( port_to_sel0[P]) )
#define portSel2Register(P)    ( (volatile uint8_t *)( port_to_sel2[P]) )

#define portSel0Register(P)    ( (volatile uint8_t *)( port_to_sel0[P]) )
#define portSel1Register(P)    ( (volatile uint8_t *)( port_to_sel1[P]) )
#define portRenRegister(P)     ( (volatile uint8_t *)( port_to_ren[P]) )
#define portOutputRegister(P)  ( (volatile uint8_t *)( port_to_output[P]) )
#define portInputRegister(P)   ( (volatile uint8_t *)( port_to_input[P]) )
#define portPMReg(P)           ( (volatile uint8_t *)( port_to_pmap[P]) )
#define digitalPinToTimer(P)   ( digital_pin_to_timer[P] )

enum{
  P1 = 1,
  P2,
#ifdef __MSP430_HAS_PORT3_R__
  P3,
#endif
#ifdef __MSP430_HAS_PORT4_R__
  P4,
#endif
#ifdef __MSP430_HAS_PORT5_R__
  P5,
#endif
#ifdef __MSP430_HAS_PORT6_R__
  P6,
#endif
#ifdef __MSP430_HAS_PORT7_R__
  P7,
#endif
#ifdef __MSP430_HAS_PORT8_R__
  P8,
#endif
#ifdef __MSP430_HAS_PORT9_R__
  P9,
#endif
#ifdef __MSP430_HAS_PORT10_R__
  P10,
#endif
#ifdef __MSP430_HAS_PORT11_R__
  P11,
#endif
#ifdef __MSP430_HAS_PORT12_R__
  P12,
#endif
#ifdef __MSP430_HAS_PORTJ_R__
  PJ,
#endif
  };

enum{
  T0A0,     // here starts the enum for Timer pins
  T0A1,     // mapped with PxSEL0
  T0A2,
  T0A3,
  T0A4,
  T1A0,
  T1A1,
  T1A2,
  T1A3,
  T1A4,
  T1A5,
  T2A0,
  T2A1,
  T2A2,
  T3A0,
  T3A1,
  T3A2,
  T4A0,
  T4A1,
  T4A2,
  T0B0,
  T0B1,
  T0B2,
  T0B3,
  T0B4,
  T0B5,
  T0B6,
  T1B0,
  T1B1,
  T1B2,
  T2B0,
  T2B1,
  T2B2,
  T3B0,
  T3B1,
  T3B2,
  T3B3,
  T3B4,
  T3B5,
  T3B6,
  T0A0_SEL1,  // here starts the enum for Timer pins
  T0A1_SEL1,  // mapped with PxSEL1
  T0A2_SEL1,
  T0A3_SEL1,
  T0A4_SEL1,
  T1A0_SEL1,
  T1A1_SEL1,
  T1A2_SEL1,
  T1A3_SEL1,
  T1A4_SEL1,
  T1A5_SEL1,
  T2A0_SEL1,
  T2A1_SEL1,
  T2A2_SEL1,
  T3A0_SEL1,
  T3A1_SEL1,
  T3A2_SEL1,
  T4A0_SEL1,
  T4A1_SEL1,
  T4A2_SEL1,
  T0B0_SEL1,
  T0B1_SEL1,
  T0B2_SEL1,
  T0B3_SEL1,
  T0B4_SEL1,
  T0B5_SEL1,
  T0B6_SEL1,
  T1B0_SEL1,
  T1B1_SEL1,
  T1B2_SEL1,
  T2B0_SEL1,
  T2B1_SEL1,
  T2B2_SEL1,
  T0A0_SEL01,  // here starts the enum for Timer pins
  T0A1_SEL01,  // mapped with PxSEL0 and PxSEL1
  T0A2_SEL01,
  T0A3_SEL01,
  T0A4_SEL01,
  T1A0_SEL01,
  T1A1_SEL01,
  T1A2_SEL01,
  T1A3_SEL01,
  T1A4_SEL01,
  T1A5_SEL01,
  T2A0_SEL01,
  T2A1_SEL01,
  T2A2_SEL01,
  T3A0_SEL01,
  T3A1_SEL01,
  T3A2_SEL01,
  T4A0_SEL01,
  T4A1_SEL01,
  T4A2_SEL01,
  T0B0_SEL01,
  T0B1_SEL01,
  T0B2_SEL01,
  T0B3_SEL01,
  T0B4_SEL01,
  T0B5_SEL01,
  T0B6_SEL01,
  T1B0_SEL01,
  T1B1_SEL01,
  T1B2_SEL01,
  T2B0_SEL01,
  T2B1_SEL01,
  T2B2_SEL01
  };

const uint16_t port_to_input[] = {
   NOT_A_PORT,
   (uint16_t) (P1_BASE+OFS_P1IN),
   (uint16_t) (P2_BASE+OFS_P2IN),
#ifdef __MSP430_HAS_PORT3_R__
   (uint16_t) (P3_BASE+OFS_P3IN),
#endif
#ifdef __MSP430_HAS_PORT4_R__
   (uint16_t) (P4_BASE+OFS_P4IN),
#endif
#ifdef __MSP430_HAS_PORT5_R__
   (uint16_t) (P5_BASE+OFS_P5IN),
#endif
#ifdef __MSP430_HAS_PORT6_R__
   (uint16_t) (P6_BASE+OFS_P6IN),
#endif
#ifdef __MSP430_HAS_PORT7_R__
   (uint16_t) (P7_BASE+OFS_P7IN),
#endif
#ifdef __MSP430_HAS_PORT8_R__
   (uint16_t) (P8_BASE+OFS_P8IN),
#endif
#ifdef __MSP430_HAS_PORT9_R__
   (uint16_t) (P9_BASE+OFS_P9IN),
#endif
#ifdef __MSP430_HAS_PORT10_R__
   (uint16_t) (P10_BASE+OFS_P10IN),
#endif
#ifdef __MSP430_HAS_PORTJ_R__
   (uint16_t) (PJ_BASE+OFS_PJIN),
#endif
};

const uint16_t port_to_output[] = {
   NOT_A_PORT,
   (uint16_t) (P1_BASE+OFS_P1OUT),
   (uint16_t) (P2_BASE+OFS_P2OUT),
#ifdef __MSP430_HAS_PORT3_R__
   (uint16_t) (P3_BASE+OFS_P3OUT),
#endif
#ifdef __MSP430_HAS_PORT4_R__
   (uint16_t) (P4_BASE+OFS_P4OUT),
#endif
#ifdef __MSP430_HAS_PORT5_R__
   (uint16_t) (P5_BASE+OFS_P5OUT),
#endif
#ifdef __MSP430_HAS_PORT6_R__
   (uint16_t) (P6_BASE+OFS_P6OUT),
#endif
#ifdef __MSP430_HAS_PORT7_R__
   (uint16_t) (P7_BASE+OFS_P7OUT),
#endif
#ifdef __MSP430_HAS_PORT8_R__
   (uint16_t) (P8_BASE+OFS_P8OUT),
#endif
#ifdef __MSP430_HAS_PORT9_R__
   (uint16_t) (P9_BASE+OFS_P9OUT),
#endif
#ifdef __MSP430_HAS_PORT10_R__
   (uint16_t) (P10_BASE+OFS_P10OUT),
#endif
#ifdef __MSP430_HAS_PORTJ_R__
   (uint16_t) (PJ_BASE+OFS_PJOUT),
#endif
};

const uint16_t port_to_dir[] = {
   NOT_A_PORT,
   (uint16_t) (P1_BASE+OFS_P1DIR),
   (uint16_t) (P2_BASE+OFS_P2DIR),
#ifdef __MSP430_HAS_PORT3_R__
   (uint16_t) (P3_BASE+OFS_P3DIR),
#endif
#ifdef __MSP430_HAS_PORT4_R__
   (uint16_t) (P4_BASE+OFS_P4DIR),
#endif
#ifdef __MSP430_HAS_PORT5_R__
   (uint16_t) (P5_BASE+OFS_P5DIR),
#endif
#ifdef __MSP430_HAS_PORT6_R__
   (uint16_t) (P6_BASE+OFS_P6DIR),
#endif
#ifdef __MSP430_HAS_PORT7_R__
   (uint16_t) (P7_BASE+OFS_P7DIR),
#endif
#ifdef __MSP430_HAS_PORT8_R__
   (uint16_t) (P8_BASE+OFS_P8DIR),
#endif
#ifdef __MSP430_HAS_PORT9_R__
   (uint16_t) (P9_BASE+OFS_P9DIR),
#endif
#ifdef __MSP430_HAS_PORT10_R__
   (uint16_t) (P10_BASE+OFS_P10DIR),
#endif
#ifdef __MSP430_HAS_PORTJ_R__
   (uint16_t) (PJ_BASE+OFS_PJDIR),
#endif
};

const uint16_t port_to_ren[] = {
      NOT_A_PORT,
      (uint16_t) (P1_BASE+OFS_P1REN),
      (uint16_t) (P2_BASE+OFS_P2REN),
   #ifdef __MSP430_HAS_PORT3_R__
      (uint16_t) (P3_BASE+OFS_P3REN),
   #endif
   #ifdef __MSP430_HAS_PORT4_R__
      (uint16_t) (P4_BASE+OFS_P4REN),
   #endif
   #ifdef __MSP430_HAS_PORT5_R__
      (uint16_t) (P5_BASE+OFS_P5REN),
   #endif
   #ifdef __MSP430_HAS_PORT6_R__
      (uint16_t) (P6_BASE+OFS_P6REN),
   #endif
   #ifdef __MSP430_HAS_PORT7_R__
      (uint16_t) (P7_BASE+OFS_P7REN),
   #endif
   #ifdef __MSP430_HAS_PORT8_R__
      (uint16_t) (P8_BASE+OFS_P8REN),
   #endif
   #ifdef __MSP430_HAS_PORT9_R__
      (uint16_t) (P9_BASE+OFS_P9REN),
   #endif
   #ifdef __MSP430_HAS_PORT10_R__
      (uint16_t) (P10_BASE+OFS_P10REN),
   #endif
   #ifdef __MSP430_HAS_PORTJ_R__
      (uint16_t) (PJ_BASE+OFS_PJREN),
   #endif
};

const uint16_t port_to_sel0[] = {  /* put this PxSEL register under the group of PxSEL0 */
      NOT_A_PORT,
      (uint16_t) (P1_BASE+OFS_P1SEL0),
      (uint16_t) (P2_BASE+OFS_P2SEL0),
   #ifdef __MSP430_HAS_PORT3_R__
      (uint16_t) (P3_BASE+OFS_P3SEL0),
   #endif
   #ifdef __MSP430_HAS_PORT4_R__
      (uint16_t) (P4_BASE+OFS_P4SEL0),
   #endif
   #ifdef __MSP430_HAS_PORT5_R__
      (uint16_t) (P5_BASE+OFS_P5SEL0),
   #endif
   #ifdef __MSP430_HAS_PORT6_R__
      (uint16_t) (P6_BASE+OFS_P6SEL0),
   #endif
   #ifdef __MSP430_HAS_PORT7_R__
      (uint16_t) (P7_BASE+OFS_P7SEL0),
   #endif
   #ifdef __MSP430_HAS_PORT8_R__
      (uint16_t) (P8_BASE+OFS_P8SEL0),
   #endif
   #ifdef __MSP430_HAS_PORT9_R__
      (uint16_t) (P9_BASE+OFS_P9SEL0),
   #endif
   #ifdef __MSP430_HAS_PORT10_R__
      (uint16_t) (P10_BASE+OFS_P10SEL0),
   #endif
   #ifdef __MSP430_HAS_PORTJ_R__
      (uint16_t) (PJ_BASE+OFS_PJSEL0),
   #endif
};

const uint16_t port_to_sel1[] = {
      NOT_A_PORT,
      (uint16_t) (P1_BASE+OFS_P1SEL1),
      (uint16_t) (P2_BASE+OFS_P2SEL1),
   #ifdef __MSP430_HAS_PORT3_R__
      (uint16_t) (P3_BASE+OFS_P3SEL1),
   #endif
   #ifdef __MSP430_HAS_PORT4_R__
      (uint16_t) (P4_BASE+OFS_P4SEL1),
   #endif
   #ifdef __MSP430_HAS_PORT5_R__
      (uint16_t) (P5_BASE+OFS_P5SEL1),
   #endif
   #ifdef __MSP430_HAS_PORT6_R__
      (uint16_t) (P6_BASE+OFS_P6SEL1),
   #endif
   #ifdef __MSP430_HAS_PORT7_R__
      (uint16_t) (P7_BASE+OFS_P7SEL1),
   #endif
   #ifdef __MSP430_HAS_PORT8_R__
      (uint16_t) (P8_BASE+OFS_P8SEL1),
   #endif
   #ifdef __MSP430_HAS_PORT9_R__
      (uint16_t) (P9_BASE+OFS_P9SEL1),
   #endif
   #ifdef __MSP430_HAS_PORT10_R__
      (uint16_t) (P10_BASE+OFS_P10SEL1),
   #endif
   #ifdef __MSP430_HAS_PORTJ_R__
      (uint16_t) (PJ_BASE+OFS_PJSEL1),
   #endif
};

const uint8_t digital_pin_to_timer[] = {
   NOT_ON_TIMER, /*  dummy */
   NOT_ON_TIMER, /*  1 - 3.3v */
   NOT_ON_TIMER, /*  2 - P1.6 */
   NOT_ON_TIMER, /*  3 - P2.5 */
   NOT_ON_TIMER, /*  4 - P2.6 */
   NOT_ON_TIMER, /*  5 - P2.2 */
   NOT_ON_TIMER, /*  6 - P5.4 */
   NOT_ON_TIMER, /*  7 - P3.5 */
   NOT_ON_TIMER, /*  8 - P4.5 */
   NOT_ON_TIMER, /*  9 - P1.3 */
   NOT_ON_TIMER, /* 10 - P1.2 */
   NOT_ON_TIMER, /* 11 - P2.4 */
   NOT_ON_TIMER, /* 12 - P2.7 */
   NOT_ON_TIMER, /* 13 - P4.2 */
   NOT_ON_TIMER, /* 14 - P3.6 */
   NOT_ON_TIMER, /* 15 - P3.2 */
   NOT_ON_TIMER, /* 16 - RST  */
   NOT_ON_TIMER, /* 17 - P2.0 */
   NOT_ON_TIMER, /* 18 - P2.1 */
   T3A1_SEL1,    /* 19 - P4.6 */
   NOT_ON_TIMER, /* 20 - GND  */
   NOT_ON_TIMER, /* 21 - 5.0v */
   NOT_ON_TIMER, /* 22 - GND  */
   NOT_ON_TIMER, /* 23 - P1.7 */
   T0B5_SEL1,    /* 24 - P4.3 */
   T0B6_SEL1,    /* 25 - P4.4 */
   NOT_ON_TIMER, /* 26 - P5.3 */
   NOT_ON_TIMER, /* 27 - P1.0 */
   T0A1_SEL1,    /* 28 - P1.1 */
   T2A1,         /* 29 - P5.7 */
   T2A2,         /* 30 - P3.7 */
   NOT_ON_TIMER, /* 31 - P3.1 */
   NOT_ON_TIMER, /* 32 - P4.1 */
   NOT_ON_TIMER, /* 33 - P6.2 */
   NOT_ON_TIMER, /* 34 - P6.1 */
   NOT_ON_TIMER, /* 35 - P3.3 */
   T2A2,         /* 36 - P6.0 */
   T0B1_SEL1,    /* 37 - P4.7 */
   T0B2_SEL1,    /* 38 - P5.0 */
   T0B3_SEL1,    /* 39 - P5.1 */
   T0B4_SEL1,    /* 40 - P5.2 */
   T1A2_SEL1,    /* 41 - P1.4 */
   T1A1_SEL1,    /* 42 - P1.5 */
   T3A1,         /* 43 - P4.0 */
   NOT_ON_TIMER, /* 44 - P2.3 */
};

const uint8_t digital_pin_to_port[] = {
   NOT_A_PIN, /*  dummy */
   NOT_A_PIN, /*  1 - 3.3v */
   P1,        /*  2 - P1.6 */
   P2,        /*  3 - P2.5 */
   P2,        /*  4 - P2.6 */
   P2,        /*  5 - P2.2 */
   P5,        /*  6 - P5.4 */
   P3,        /*  7 - P3.5 */
   P4,        /*  8 - P4.5 */
   P1,        /*  9 - P1.3 */
   P1,        /* 10 - P1.2 */
   P2,        /* 11 - P2.4 */
   P2,        /* 12 - P2.7 */
   P4,        /* 13 - P4.2 */
   P3,        /* 14 - P3.6 */
   P3,        /* 15 - P3.2 */
   NOT_A_PIN, /* 16 - RST  */
   P2,        /* 17 - P2.0 */
   P2,        /* 18 - P2.1 */
   P4,        /* 19 - P4.6 */
   NOT_A_PIN, /* 20 - GND  */
   NOT_A_PIN, /* 21 - 5.0v */
   NOT_A_PIN, /* 22 - GND  */
   P1,        /* 23 - P1.7 */
   P4,        /* 24 - P4.3 */
   P4,        /* 25 - P4.4 */
   P5,        /* 26 - P5.3 */
   P1,        /* 27 - P1.0 */
   P1,        /* 28 - P1.1 */
   P5,        /* 29 - P5.7 */
   P3,        /* 30 - P3.7 */
   P3,        /* 31 - P3.1 */
   P4,        /* 32 - P4.1 */
   P6,        /* 33 - P6.2 */
   P6,        /* 34 - P6.1 */
   P3,        /* 35 - P3.3 */
   P6,        /* 36 - P6.0 */
   P4,        /* 37 - P4.7 */
   P5,        /* 38 - P5.0 */
   P5,        /* 39 - P5.1 */
   P5,        /* 40 - P5.2 */
   P1,        /* 41 - P1.4 */
   P1,        /* 42 - P1.5 */
   P4,        /* 43 - P4.0 */
   P2,        /* 44 - P2.3 */
};

const uint8_t digital_pin_to_bit_mask[] = {
   NOT_A_PIN, /* 0,  pin count starts at 1 */
   NOT_A_PIN, /*  1 - 3.3v */
   BV(6),     /*  2 - P1.6 */
   BV(5),     /*  3 - P2.5 */
   BV(6),     /*  4 - P2.6 */
   BV(2),     /*  5 - P2.2 */
   BV(4),     /*  6 - P5.4 */
   BV(5),     /*  7 - P3.5 */
   BV(5),     /*  8 - P4.5 */
   BV(3),     /*  9 - P1.3 */
   BV(2),     /* 10 - P1.2 */
   BV(4),     /* 11 - P2.4 */
   BV(7),     /* 12 - P2.7 */
   BV(2),     /* 13 - P4.2 */
   BV(6),     /* 14 - P3.6 */
   BV(2),     /* 15 - P3.2 */
   NOT_A_PIN, /* 16 - RST  */
   BV(0),     /* 17 - P2.0 */
   BV(1),     /* 18 - P2.1 */
   BV(6),     /* 19 - P4.6 */
   NOT_A_PIN, /* 20 - GND  */
   NOT_A_PIN, /* 21 - 5.0v */
   NOT_A_PIN, /* 22 - GND  */
   BV(7),     /* 23 - P1.7 */
   BV(3),     /* 24 - P4.3 */
   BV(4),     /* 25 - P4.4 */
   BV(3),     /* 26 - P5.3 */
   BV(0),     /* 27 - P1.0 */
   BV(1),     /* 28 - P1.1 */
   BV(7),     /* 29 - P5.7 */
   BV(7),     /* 30 - P3.7 */
   BV(1),     /* 31 - P3.1 */
   BV(1),     /* 32 - P4.1 */
   BV(2),     /* 33 - P6.2 */
   BV(1),     /* 34 - P6.1 */
   BV(3),     /* 35 - P3.3 */
   BV(0),     /* 36 - P6.0 */
   BV(7),     /* 37 - P4.7 */
   BV(0),     /* 38 - P5.0 */
   BV(1),     /* 39 - P5.1 */
   BV(2),     /* 40 - P5.2 */
   BV(4),     /* 41 - P1.4 */
   BV(5),     /* 42 - P1.5 */
   BV(0),     /* 43 - P4.0 */
   BV(3),     /* 44 - P2.3 */
};

const uint32_t digital_pin_to_analog_in[] = {
        NOT_ON_ADC,   /*  dummy */
        NOT_ON_ADC,   /*  1 - 3.3v */
        6,            /*  2 - P1.6 */
        NOT_ON_ADC,   /*  3 - P2.5 */
        NOT_ON_ADC,   /*  4 - P2.6 */
        NOT_ON_ADC,   /*  5 - P2.2 */
        11,           /*  6 - P5.4 */
        NOT_ON_ADC,   /*  7 - P3.5 */
        NOT_ON_ADC,   /*  8 - P4.5 */
        NOT_ON_ADC,   /*  9 - P1.3 */
        NOT_ON_ADC,   /* 10 - P1.2 */
        NOT_ON_ADC,   /* 11 - P2.4 */
        NOT_ON_ADC,   /* 12 - P2.7 */
        NOT_ON_ADC,   /* 13 - P4.2 */
        NOT_ON_ADC,   /* 14 - P3.6 */
        NOT_ON_ADC,   /* 15 - P3.2 */
        NOT_ON_ADC,   /* 16 - RST  */
        NOT_ON_ADC,   /* 17 - P2.0 */
        NOT_ON_ADC,   /* 18 - P2.1 */
        NOT_ON_ADC,   /* 19 - P4.6 */
        NOT_ON_ADC,   /* 20 - GND  */
        NOT_ON_ADC,   /* 21 - 5.0v */
        NOT_ON_ADC,   /* 22 - GND  */
        7,            /* 23 - P1.7 */
        8,            /* 24 - P4.3 */
        9,            /* 25 - P4.4 */
        10,           /* 26 - P5.3 */
        0,            /* 27 - P1.0 */
        1,            /* 28 - P1.1 */
        NOT_ON_ADC,   /* 29 - P5.7 */
        NOT_ON_ADC,   /* 30 - P3.7 */
        NOT_ON_ADC,   /* 31 - P3.1 */
        NOT_ON_ADC,   /* 32 - P4.1 */
        NOT_ON_ADC,   /* 33 - P6.2 */
        NOT_ON_ADC,   /* 34 - P6.1 */
        NOT_ON_ADC,   /* 35 - P3.3 */
        NOT_ON_ADC,   /* 36 - P6.0 */
        NOT_ON_ADC,   /* 37 - P4.7 */
        NOT_ON_ADC,   /* 38 - P5.0 */
        NOT_ON_ADC,   /* 39 - P5.1 */
        NOT_ON_ADC,   /* 40 - P5.2 */
        NOT_ON_ADC,   /* 41 - P1.4 */
        NOT_ON_ADC,   /* 42 - P1.5 */
        NOT_ON_ADC,   /* 43 - P4.0 */
        NOT_ON_ADC,   /* 44 - P2.3 */
};

typedef uint8_t boolean;
typedef uint8_t byte;

#ifdef __cplusplus
}
#endif

#endif /* UBLOX_GPS_ENERGIA_LIB_H_ */
