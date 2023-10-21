/*
 * tm1650.h
 *
 *  Created on: Jul 17, 2021
 *      Author: raman
 *
 *  Hacked by Raman Gopalan <ramangopalan@gmail.com>.
 *  Derived from the "7 segment display driver for JY-MCU module
 *  based on TM1650 chip" by Anatoli Arkhipenko.
 */

#ifndef INC_TM1650_H_
#define INC_TM1650_H_

#include "main.h"
#include <stdbool.h>
#include <string.h>
#include "stm32_sw_i2c.h"

#define TM1650_DISPLAY_BASE    0x34 // Address of the left-most digit
#define TM1650_DCTRL_BASE      0x24 // Address of the control register of the left-most digit
#define TM1650_NUM_DIGITS      16   // max number of digits
#define TM1650_MAX_STRING      128  // number of digits
#define TM1650_MAX_I2C_DELAY   (10)

#define TM1650_BIT_ONOFF	0b00000001
#define TM1650_MSK_ONOFF	0b11111110
#define TM1650_BIT_DOT		0b00000001
#define TM1650_MSK_DOT		0b11110111
#define TM1650_BRIGHT_SHIFT	4
#define TM1650_MSK_BRIGHT	0b10001111
#define TM1650_MIN_BRIGHT	0
#define TM1650_MAX_BRIGHT	7

/* Main structure */
typedef struct tm1650_t {
  char *iposition;
  bool iactive;
  unsigned int inumdigits;
  unsigned int ibrightness;
  char istring[TM1650_MAX_STRING + 1];
  uint8_t ibuffer[TM1650_NUM_DIGITS + 1];
  uint8_t ictrl[TM1650_NUM_DIGITS];
  // Raman: Use SW I2C instead.
  // I2C_HandleTypeDef *i2c_handle;
} tm1650_t;

// Raman: Use SW I2C instead.
// extern I2C_HandleTypeDef hi2c1;

void tm1638_construct_digits (tm1650_t *t, unsigned int digits);
// Raman: Use SW I2C instead.
//void tm1650_set_i2c_handle (tm1650_t *t, I2C_HandleTypeDef *h);
void tm1650_init (tm1650_t *t);
void tm1650_clear (tm1650_t *t);
void tm1650_display_on (tm1650_t *t);
void tm1650_display_off (tm1650_t *t);
void tm1650_display_state (tm1650_t *t, bool s);
void tm1650_display_string (tm1650_t *t, char *s);
int tm1650_display_running (tm1650_t *t, char *s);
int tm1650_display_running_shift (tm1650_t *t);
void tm1650_set_brightness (tm1650_t *t, unsigned int b);
void tm1650_set_brightness_gradually (tm1650_t *t, unsigned int b);
inline unsigned int tm1650_get_brightness (tm1650_t *t) { return t->ibrightness; }
void tm1650_control_position (tm1650_t *t, unsigned int apos, uint8_t avalue);
void tm1650_set_position (tm1650_t *t, unsigned int apos, uint8_t avalue);
void tm1650_set_dot (tm1650_t *t, unsigned int apos, bool astate);
inline uint8_t tm1650_get_position (tm1650_t *t, unsigned int apos) { return t->ibuffer[apos]; }
inline unsigned int	tm1650_get_num_positions (tm1650_t *t) { return t->inumdigits; }

#endif /* INC_TM1650_H_ */
