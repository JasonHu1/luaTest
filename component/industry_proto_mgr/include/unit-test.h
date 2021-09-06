/* tests/unit-test.h.  Generated from unit-test.h.in by configure.  */
/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

/* Constants defined by configure.ac */
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# ifndef _MSC_VER
# include <stdint.h>
# else
# include "stdint.h"
# endif
#endif

enum {
    TCP,
    TCP_PI,
    RTU
};

#define SERVER_ID         11
#define INVALID_SERVER_ID 18
//
//const uint16_t UT_BITS_ADDRESS ;
//const uint16_t UT_BITS_NB ;
//const uint8_t UT_BITS_TAB[];
//
//const uint16_t UT_INPUT_BITS_ADDRESS ;
//const uint16_t UT_INPUT_BITS_NB ;
//const uint8_t UT_INPUT_BITS_TAB[] ;
//
//const uint16_t UT_REGISTERS_ADDRESS ;
//const uint16_t UT_REGISTERS_NB ;
//const uint16_t UT_REGISTERS_NB_MAX ;
//const uint16_t UT_REGISTERS_TAB[] ;
//
///* Raise a manual exception when this address is used for the first byte */
//const uint16_t UT_REGISTERS_ADDRESS_SPECIAL ;
///* The response of the server will contains an invalid TID or slave */
//const uint16_t UT_REGISTERS_ADDRESS_INVALID_TID_OR_SLAVE ;
///* The server will wait for 1 second before replying to test timeout */
//const uint16_t UT_REGISTERS_ADDRESS_SLEEP_500_MS ;
///* The server will wait for 5 ms before sending each byte */
//const uint16_t UT_REGISTERS_ADDRESS_BYTE_SLEEP_5_MS ;
//
///* If the following value is used, a bad response is sent.
//   It's better to test with a lower value than
//   UT_REGISTERS_NB_POINTS to try to raise a segfault. */
//const uint16_t UT_REGISTERS_NB_SPECIAL ;
//
//const uint16_t UT_INPUT_REGISTERS_ADDRESS ;
//const uint16_t UT_INPUT_REGISTERS_NB ;
//const uint16_t UT_INPUT_REGISTERS_TAB[] ;
//
//const float UT_REAL ;
//
//const uint32_t UT_IREAL_ABCD ;
//const uint32_t UT_IREAL_DCBA ;
//const uint32_t UT_IREAL_BADC ;
//const uint32_t UT_IREAL_CDAB ;

/* const uint32_t UT_IREAL_ABCD = 0x47F12000);
const uint32_t UT_IREAL_DCBA = 0x0020F147;
const uint32_t UT_IREAL_BADC = 0xF1470020;
const uint32_t UT_IREAL_CDAB = 0x200047F1;*/

#endif /* _UNIT_TEST_H_ */
