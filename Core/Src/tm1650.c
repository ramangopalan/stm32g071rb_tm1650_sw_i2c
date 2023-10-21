/*
 * tm1650.c
 *
 *  Created on: Jul 17, 2021
 *      Author: raman
 *
 *  Hacked by Raman Gopalan <ramangopalan@gmail.com>.
 *  Derived from the "7 segment display driver for JY-MCU module
 *  based on TM1650 chip" by Anatoli Arkhipenko.
 */

#include "tm1650.h"

const uint8_t tm1650_cdigits[128] = {
//0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x00
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
  0x00, 0x82, 0x21, 0x00, 0x00, 0x00, 0x00, 0x02, 0x39, 0x0F, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, // 0x20
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7f, 0x6f, 0x00, 0x00, 0x00, 0x48, 0x00, 0x53, // 0x30
  0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x6F, 0x76, 0x06, 0x1E, 0x00, 0x38, 0x00, 0x54, 0x3F, // 0x40
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x00, 0x0F, 0x00, 0x08, // 0x50
  0x63, 0x5F, 0x7C, 0x58, 0x5E, 0x7B, 0x71, 0x6F, 0x74, 0x02, 0x1E, 0x00, 0x06, 0x00, 0x54, 0x5C, // 0x60
  0x73, 0x67, 0x50, 0x6D, 0x78, 0x1C, 0x00, 0x00, 0x00, 0x6E, 0x00, 0x39, 0x30, 0x0F, 0x00, 0x00  // 0x70
};

/** Constructor, uses default values for the parameters
 * so could be called with no parameters.
 * d - number of display digits (default = 4)
 */
void tm1638_construct_digits (tm1650_t *t, unsigned int d) {
  t->inumdigits = (d > TM1650_NUM_DIGITS) ? TM1650_NUM_DIGITS : d;
}

/** Set i2c handler.
 * h - HAL I2C handle.
 */
// Raman: Use SW I2C instead.
//void tm1650_set_i2c_handle (tm1650_t *t, I2C_HandleTypeDef *h) {
//  t->i2c_handle = h;
//}

/** Initialization
 * initializes the driver. Turns display on, but clears all digits.
 */
void tm1650_init (tm1650_t *t) {
  /* Set the i2c handle from the HAL */
  // Raman: Use SW I2C instead.
//  tm1650_set_i2c_handle(t, &hi2c1);
  I2C_init();
  delay_us(1000); // 1 ms

  /* Set the number of digits in use */
  tm1638_construct_digits(t, 4);

  /* States */
  t->iposition = NULL;
  for (int i=0; i<t->inumdigits; i++) {
    t->ibuffer[i] = 0;
    t->ictrl[i] = 0;
  }

  /* Raman: Set active for now. Come back later. */
  t->iactive = 1;
  tm1650_clear(t);
  tm1650_display_on(t);
}

/** Set brightness of all digits equally
 * b - brightness value with 1 being the lowest, and 7 being the brightest
 */
void tm1650_set_brightness (tm1650_t *t, unsigned int b) {
  if (!t->iactive) return;
  t->ibrightness = (b > TM1650_MAX_BRIGHT) ? TM1650_MAX_BRIGHT : b;
  for (int i = 0; i < t->inumdigits; i++) {
    t->ictrl[i] = (t->ictrl[i] & TM1650_MSK_BRIGHT) | (t->ibrightness << TM1650_BRIGHT_SHIFT);
//    HAL_I2C_Master_Transmit(t->i2c_handle, (TM1650_DCTRL_BASE + i) << 1, &t->ictrl[i], 1, TM1650_MAX_I2C_DELAY);
    I2C_transmit((TM1650_DCTRL_BASE + i) << 1, &t->ictrl[i], 1);
  }
}

/** Set brightness of all digits equally
 * b - brightness value with 1 being the lowest, and 7 being the brightest
 */
void tm1650_set_brightness_gradually (tm1650_t *t, unsigned int b) {
  if (!t->iactive || b == t->ibrightness) return;
  if (b > TM1650_MAX_BRIGHT) b = TM1650_MAX_BRIGHT;
  int step = (b < t->ibrightness) ? -1 : 1;
  unsigned int i = t->ibrightness;
  do {
    tm1650_set_brightness(t, i);
    HAL_Delay(50);
    i += step;
  } while (i != b);
}

/** Turns display on or off according to state (s)
 */
void tm1650_display_state (tm1650_t *t, bool s) {
  if (s) tm1650_display_on(t);
  else tm1650_display_off(t);
}

/** Turns the display on
 */
void tm1650_display_on (tm1650_t *t) {
  if (!t->iactive) return;
  for (int i=0; i < t->inumdigits; i++) {
	t->ictrl[i] = (t->ictrl[i] & TM1650_MSK_ONOFF) | TM1650_BIT_DOT;
//	HAL_I2C_Master_Transmit(t->i2c_handle, (TM1650_DCTRL_BASE + i) << 1, &t->ictrl[i], 1, TM1650_MAX_I2C_DELAY);
	I2C_transmit((TM1650_DCTRL_BASE + i) << 1, &t->ictrl[i], 1);
  }
}
/** Turns the display off
 */
void tm1650_display_off (tm1650_t *t) {
  if (!t->iactive) return;
  for (int i = 0; i < t->inumdigits; i++) {
	t->ictrl[i] = (t->ictrl[i] & TM1650_MSK_ONOFF);
//	HAL_I2C_Master_Transmit(t->i2c_handle, (TM1650_DCTRL_BASE + i) << 1, &t->ictrl[i], 1, TM1650_MAX_I2C_DELAY);
	I2C_transmit((TM1650_DCTRL_BASE + i) << 1, &t->ictrl[i], 1);
  }
}

/** Directly write to the CONTROL register of the digital position
 * apos = position to set the control register for
 * avalue = value to write to the position
 *
 * Internal control buffer is updated as well
 */
void tm1650_control_position (tm1650_t *t, unsigned int apos, uint8_t avalue) {
  if (!t->iactive) return;
  if (apos < t->inumdigits) {
    t->ictrl[apos] = avalue;
//    HAL_I2C_Master_Transmit(t->i2c_handle, (TM1650_DCTRL_BASE + (int)apos) << 1, &t->ictrl[apos], 1, TM1650_MAX_I2C_DELAY);
    I2C_transmit((TM1650_DCTRL_BASE + (int)apos) << 1, &t->ictrl[apos], 1);
  }
}

/** Directly write to the digit register of the digital position
 * apos = position to set the digit register for
 * avalue = value to write to the position
 *
 * Internal position buffer is updated as well
 */
void tm1650_set_position (tm1650_t *t, unsigned int apos, uint8_t avalue) {
  if (!t->iactive) return;
  if (apos < t->inumdigits) {
    t->ibuffer[apos] = avalue;
//    HAL_I2C_Master_Transmit(t->i2c_handle, (TM1650_DISPLAY_BASE + (int)apos) << 1, &t->ibuffer[apos], 1, TM1650_MAX_I2C_DELAY);
    I2C_transmit((TM1650_DISPLAY_BASE + (int)apos) << 1, &t->ibuffer[apos], 1);
  }
}

/** Directly set/clear a 'dot' next to a specific position
 * apos = position to set/clear the dot for
 * astate = display the dot if true, clear if false
 *
 * Internal buffer is updated as well
 */
void tm1650_set_dot (tm1650_t *t, unsigned int apos, bool astate) {
  t->ibuffer[apos] = (t->ibuffer[apos] & 0x7F) | (astate ? 0b10000000 : 0);
  tm1650_set_position(t, apos, t->ibuffer[apos]);
}

/** Clear all digits. Keep the display on.
 */
void tm1650_clear (tm1650_t *t) {
  uint8_t v = 0;
  if (!t->iactive) return;
  for (int i=0; i<t->inumdigits; i++) {
//    HAL_I2C_Master_Transmit(t->i2c_handle, (TM1650_DISPLAY_BASE + i) << 1, &v, 1, TM1650_MAX_I2C_DELAY);
    I2C_transmit((TM1650_DISPLAY_BASE + i) << 1, &v, 1);
 	t->ibuffer[i] = 0;
  }
}

/** Display string on the display
 * s = character array to be displayed
 *
 * Internal buffer is updated as well
 * Only first N positions of the string are displayed if
 *  the string is longer than the number of digits
 */
void tm1650_display_string (tm1650_t *t, char *s) {
  uint8_t l;
  if (!t->iactive) return;
  for (int i=0; i<t->inumdigits; i++) {
    uint8_t a = ((uint8_t) s[i]) & 0b01111111;
    uint8_t dot = ((uint8_t) s[i]) & 0b10000000;
    t->ibuffer[i] = tm1650_cdigits[a];
    if (a) {
      l = t->ibuffer[i] | dot;
//      HAL_I2C_Master_Transmit(t->i2c_handle, (TM1650_DISPLAY_BASE + i) << 1, &l, 1, TM1650_MAX_I2C_DELAY);
      I2C_transmit((TM1650_DISPLAY_BASE + i) << 1, &l, 1);
    }
    else
      break;
  }
}

/** Display string on the display in a running fashion
 * s = character array to be displayed
 *
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift() is called
 *
 * returns: number of iterations remaining to display the whole string
 */
int tm1650_display_running (tm1650_t *t, char *s) {
  strncpy(t->istring, s, TM1650_MAX_STRING + 1);
  t->iposition = t->istring;
  t->istring[TM1650_MAX_STRING] = '\0'; //just in case.
  tm1650_display_string(t, t->iposition);
  int l = strlen(t->iposition);
  if (l <= t->inumdigits) return 0;
  return (l - t->inumdigits);
}

/** Display next segment (shifting to the left) of the string set by displayRunning()
 * Starts with first N positions of the string.
 * Subsequent characters are displayed with 1 char shift each time displayRunningShift is called
 *
 * returns: number of iterations remaining to display the whole string
 */
int tm1650_display_running_shift (tm1650_t *t) {
  if (strlen(t->iposition) <= t->inumdigits) return 0;
  tm1650_display_string(t, ++t->iposition);
  return (strlen(t->iposition) - t->inumdigits);
}
