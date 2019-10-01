/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfigPre.h"

#if ENABLED(TOUCH_BUTTONS)

#include "xpt2046.h"
#include "../../inc/MarlinConfig.h"
#include "../../lcd/TFT/TFT_screen_defines.h"

#ifndef TOUCH_INT_PIN
  #define TOUCH_INT_PIN  -1
#endif
#ifndef TOUCH_MISO_PIN
  #define TOUCH_MISO_PIN MISO_PIN
#endif
#ifndef TOUCH_MOSI_PIN
  #define TOUCH_MOSI_PIN MOSI_PIN
#endif
#ifndef TOUCH_SCK_PIN
  #define TOUCH_SCK_PIN  SCK_PIN
#endif
#ifndef TOUCH_CS_PIN
  #define TOUCH_CS_PIN   CS_PIN
#endif

//define DEBUG_TOUCH_POINT

XPT2046 touch;
extern int8_t encoderDiff;
uint16_t XPT2046::raw_x, XPT2046::raw_y, XPT2046::pixel_x, XPT2046::pixel_y;
int16_t XPT2046::tscalibration[6];

void XPT2046::init() {
  SET_INPUT(TOUCH_MISO_PIN);
  SET_OUTPUT(TOUCH_MOSI_PIN);
  SET_OUTPUT(TOUCH_SCK_PIN);
  OUT_WRITE(TOUCH_CS_PIN, HIGH);

  #if PIN_EXISTS(TOUCH_INT)
    // Optional Pendrive interrupt pin
    SET_INPUT(TOUCH_INT_PIN);
  #endif

  // Read once to enable pendrive status pin
  getInTouch(XPT2046_X);
}

#include "../../lcd/ultralcd.h" // For EN_C bit mask

inline uint16_t middleOfThree(uint16_t a, uint16_t b, uint16_t c) {
    if (a > b)
      return (b > c) ? b : ((a > c) ? c : a);
    return   (a > c) ? a : ((b > c) ? c : b);
}

uint8_t XPT2046::read_buttons() {

  uint16_t raw_x1, raw_x2, raw_x3, raw_y1, raw_y2, raw_y3;

  if (!isTouched()) return 0;

  raw_x1 = getInTouch(XPT2046_X);
  raw_y1 = getInTouch(XPT2046_Y);

  if (!isTouched()) return 0;

  raw_x2 = getInTouch(XPT2046_X);
  raw_y2 = getInTouch(XPT2046_Y);

  if (!isTouched()) return 0;

  raw_x3 = getInTouch(XPT2046_X);
  raw_y3 = getInTouch(XPT2046_Y);

  raw_x = middleOfThree(raw_x1, raw_x2, raw_x3);
  raw_y = middleOfThree(raw_y1, raw_y2, raw_y3);
  pixel_x = uint16_t(((uint32_t(raw_x)) * tscalibration[0] + (uint32_t(raw_y)) * tscalibration[1]) >> 16) + tscalibration[2],
  pixel_y = uint16_t(((uint32_t(raw_x)) * tscalibration[3] + (uint32_t(raw_y)) * tscalibration[4]) >> 16) + tscalibration[5];

  if (!isTouched()) return 0; // Fingers must still be on the TS for a valid read.

  const uint8_t row_touched = pixel_y / LCD_CELL_HEIGHT;
  const uint8_t col_touched = (pixel_x * 12 + 4) / LCD_PIXEL_WIDTH;

  if (row_touched < 12) {
    return (((row_touched+1) << 4) + col_touched);
  }
  else {
   #if ENABLED(FULL_SCALE_TFT_480X320)
    return  WITHIN(pixel_x,  12, 119) ? EN_D
          : WITHIN(pixel_x, 128, 235) ? EN_A
          : WITHIN(pixel_x, 244, 351) ? EN_B
          : WITHIN(pixel_x, 360, 467) ? EN_C
          : 0;
   #else
    return  WITHIN(pixel_x,   7,  78) ? EN_D
          : WITHIN(pixel_x,  85, 156) ? EN_A
          : WITHIN(pixel_x, 163, 234) ? EN_B
          : WITHIN(pixel_x, 241, 312) ? EN_C
          : 0;
   #endif
  }
}

bool XPT2046::isTouched() {
  return (
    #if PIN_EXISTS(TOUCH_INT)
      READ(TOUCH_INT_PIN) != HIGH
    #else
      getInTouch(XPT2046_Z1) >= XPT2046_Z1_THRESHOLD
    #endif
  );
}

uint16_t XPT2046::getInTouch(const XPTCoordinate coordinate) {
  uint16_t data[3];

  OUT_WRITE(TOUCH_CS_PIN, LOW);

  const uint8_t coord = uint8_t(coordinate) | XPT2046_CONTROL | XPT2046_DFR_MODE;
  for (uint16_t i = 0; i < 3 ; i++) {
    for (uint8_t j = 0x80; j; j >>= 1) {
      WRITE(TOUCH_SCK_PIN, LOW);
      WRITE(TOUCH_MOSI_PIN, bool(coord & j));
      WRITE(TOUCH_SCK_PIN, HIGH);
    }

    data[i] = 0;
    for (uint16_t j = 0x8000; j; j >>= 1) {
      WRITE(TOUCH_SCK_PIN, LOW);
      if (READ(TOUCH_MISO_PIN)) data[i] |= j;
      WRITE(TOUCH_SCK_PIN, HIGH);
    }
    WRITE(TOUCH_SCK_PIN, LOW);
    data[i] >>= 4;
  }

  WRITE(TOUCH_CS_PIN, HIGH);

  uint16_t delta01 = _MAX(data[0], data[1]) - _MIN(data[0], data[1]),
           delta02 = _MAX(data[0], data[2]) - _MIN(data[0], data[2]),
           delta12 = _MAX(data[1], data[2]) - _MIN(data[1], data[2]);

  if (delta01 <= delta02 && delta01 <= delta12)
    return (data[0] + data[1]) >> 1;

  if (delta02 <= delta12)
    return (data[0] + data[2]) >> 1;

  return (data[1] + data[2]) >> 1;
}

bool XPT2046::getTouchPoint(uint16_t &x, uint16_t &y) {
  if (isTouched()) {
    x = getInTouch(XPT2046_X);
    y = getInTouch(XPT2046_Y);
  }
  return isTouched();
}

#endif // TOUCH_BUTTONS
