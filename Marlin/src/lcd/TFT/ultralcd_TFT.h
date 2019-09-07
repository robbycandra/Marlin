/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
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
#pragma once

/**
 * ultralcd_TFT.h
 */

#include "../../inc/MarlinConfigPre.h"

#include <U8glib.h>
#include "../dogm/HAL_LCD_class_defines.h"

// No More LCD selection
#define U8G_CLASS U8GLIB_TFT_320X240_TOUCH
#define U8G_PARAM FSMC_CS_PIN, FSMC_RS_PIN
#define LCD_PIXEL_WIDTH  320
#define LCD_PIXEL_HEIGHT 192

// For selective rendering within a Y range
#define PAGE_OVER(ya)         ((ya) <= u8g.getU8g()->current_page.y1) // Does the current page follow a region top?
#define PAGE_UNDER(yb)        ((yb) >= u8g.getU8g()->current_page.y0) // Does the current page precede a region bottom?
#define PAGE_CONTAINS(ya, yb) ((yb) >= u8g.getU8g()->current_page.y0 && (ya) <= u8g.getU8g()->current_page.y1) // Do two vertical regions overlap?

// Only Western languages support big / small fonts
#if DISABLED(DISPLAY_CHARSET_ISO10646_1)
  #undef USE_BIG_EDIT_FONT
  #undef USE_SMALL_INFOFONT
#endif

#define MENU_FONT_NAME    u8g_font_10x20
#define MENU_FONT_WIDTH   10
#define MENU_FONT_ASCENT  13
#define MENU_FONT_DESCENT  4
#define MENU_FONT_HEIGHT  20

#define STATUS_FONT_NAME u8g_font_profont22
#define STATUS_FONT_ASCENT 14
#define STATUS_FONT_DESCENT  4
#define STATUS_FONT_HEIGHT  20
#define STATUS_FONT_WIDTH   12

#if ENABLED(USE_BIG_EDIT_FONT)
  #define EDIT_FONT_NAME    u8g_font_9x18
  #define EDIT_FONT_WIDTH    9
  #define EDIT_FONT_ASCENT  10
  #define EDIT_FONT_DESCENT  3
  #define EDIT_FONT_HEIGHT (EDIT_FONT_ASCENT + EDIT_FONT_DESCENT)
#else
  #define EDIT_FONT_NAME    MENU_FONT_NAME
  #define EDIT_FONT_WIDTH   MENU_FONT_WIDTH
  #define EDIT_FONT_ASCENT  MENU_FONT_ASCENT
  #define EDIT_FONT_DESCENT MENU_FONT_DESCENT
  #define EDIT_FONT_HEIGHT  MENU_FONT_HEIGHT
#endif

// Get the Ascent, Descent, and total Height for the Info Screen font
#if ENABLED(USE_SMALL_INFOFONT)
  extern const u8g_fntpgm_uint8_t u8g_font_6x9[];
  #define INFO_FONT_ASCENT 7
#else
  #define INFO_FONT_ASCENT 8
#endif
#define INFO_FONT_DESCENT 2
#define INFO_FONT_HEIGHT (INFO_FONT_ASCENT + INFO_FONT_DESCENT)
#define INFO_FONT_WIDTH   6

extern U8G_CLASS u8g;
