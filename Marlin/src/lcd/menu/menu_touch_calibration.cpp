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

//
// Marlin Touch Menu Calibration
//

#include "../../inc/MarlinConfigPre.h"

#if HAS_LCD_MENU && ENABLED(TOUCH_CALIBRATION)

#include "menu.h"

#include "../../module/configuration_store.h"
#include "../../lcd/TFT/ultralcd_TFT.h"

static uint8_t touch_point_index;

void _lcd_touch_point_screen () {
  u8g.drawHLine(10, 20, 20);
  u8g.drawVLine(20, 10, 20);
  if (ui.lcd_menu_touched_coord & B10000000) {
      #if HAS_BUZZER
        ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
      #endif
      ui.goto_previous_screen();
  }
}

void menu_touch_calibration() {
  ui.screen_mode = SCRMODE_MENU_2X4; 
  touch_point_index = 0;
  ui.defer_status_screen();
  ui.goto_screen(_lcd_touch_point_screen);
}

#endif // HAS_LCD_MENU
