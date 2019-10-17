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
// Configuration Menu
//

#include "../../inc/MarlinConfigPre.h"

#if HAS_LCD_MENU && ENABLED(REXYZ_TOUCH_MENU)

#include "menu.h"

#include "../../module/configuration_store.h"

static void lcd_store_settings_confirm() {
  do_select_screen(
    GET_TEXT(MSG_STORE_EEPROM), GET_TEXT(MSG_BUTTON_CANCEL),
    []{
      const bool inited = settings.save();
      #if HAS_BUZZER
        ui.completion_feedback(inited);
      #endif
      UNUSED(inited);
    },
    ui.goto_previous_screen,
    GET_TEXT(MSG_STORE_EEPROM), nullptr, PSTR("?")
  );
}

static void lcd_load_settings_confirm() {
  do_select_screen(
    GET_TEXT(MSG_LOAD_EEPROM), GET_TEXT(MSG_BUTTON_CANCEL),
    []{
      const bool inited = settings.load();
      #if HAS_BUZZER
        ui.completion_feedback(inited);
      #endif
      UNUSED(inited);
    },
    ui.goto_previous_screen,
    GET_TEXT(MSG_LOAD_EEPROM), nullptr, PSTR("?")
  );
}

static void lcd_factory_settings_confirm() {
  do_select_screen(
    GET_TEXT(MSG_RESTORE_FAILSAFE), GET_TEXT(MSG_BUTTON_CANCEL),
    []{
      settings.reset();
      #if HAS_BUZZER
        ui.completion_feedback(true);
      #endif
    },
    ui.goto_previous_screen,
    GET_TEXT(MSG_RESTORE_FAILSAFE), nullptr, PSTR("?")
  );
}

static void lcd_init_eeprom_confirm() {
  do_select_screen(
    GET_TEXT(MSG_BUTTON_INIT), GET_TEXT(MSG_BUTTON_CANCEL),
    []{
      const bool inited = settings.init_eeprom();
      #if HAS_BUZZER
        ui.completion_feedback(inited);
      #endif
      UNUSED(inited);
    },
    ui.goto_previous_screen,
    GET_TEXT(MSG_INIT_EEPROM), nullptr, PSTR("?")
  );
}

void rmenu_loadsave() {
  const bool busy = printer_busy();
  START_MENU();
  #if ENABLED(EEPROM_SETTINGS)
    SUBSELECT(MSG_STORE_EEPROM, lcd_store_settings_confirm);
    if (!busy)
      SUBSELECT(MSG_LOAD_EEPROM, lcd_load_settings_confirm);
  #endif

  if (!busy)
    SUBSELECT(MSG_RESTORE_FAILSAFE, lcd_factory_settings_confirm);

  #if ENABLED(EEPROM_SETTINGS)
    if (rexyz_menu_mode != MENUMODE_BASIC)
      SUBSELECT(MSG_INIT_EEPROM, lcd_init_eeprom_confirm);
  #endif

  END_MENU();
}

#endif // HAS_LCD_MENU & REXYZ_TOUCH_MENU
