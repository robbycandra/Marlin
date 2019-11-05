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

void rmenu_loadsave() {
  const bool busy = printer_busy();
  START_MENU();
  #if ENABLED(EEPROM_SETTINGS)
    CONFIRM_ITEM(MSG_STORE_EEPROM,
      MSG_STORE_EEPROM, MSG_BUTTON_CANCEL,
      []{
        const bool inited = settings.save();
        #if HAS_BUZZER
          ui.completion_feedback(inited);
        #endif
        UNUSED(inited);
      },
      ui.goto_previous_screen,
      GET_TEXT(MSG_STORE_EEPROM), (PGM_P)nullptr, PSTR("?")
    );

    if (!busy)
      CONFIRM_ITEM(MSG_LOAD_EEPROM,
        MSG_LOAD_EEPROM, MSG_BUTTON_CANCEL,
        []{
          const bool inited = settings.load();
          #if HAS_BUZZER
            ui.completion_feedback(inited);
          #endif
          UNUSED(inited);
        },
        ui.goto_previous_screen,
        GET_TEXT(MSG_LOAD_EEPROM), (PGM_P)nullptr, PSTR("?")
      );

  #endif

  if (!busy)
    CONFIRM_ITEM(MSG_RESTORE_FAILSAFE,
      MSG_RESTORE_FAILSAFE, MSG_BUTTON_CANCEL,
      []{
        settings.reset();
        #if HAS_BUZZER
          ui.completion_feedback(true);
        #endif
      },
      ui.goto_previous_screen,
      GET_TEXT(MSG_RESTORE_FAILSAFE), (PGM_P)nullptr, PSTR("?")
    );

  #if ENABLED(EEPROM_SETTINGS)
    if (rexyz_menu_mode != MENUMODE_BASIC)
      CONFIRM_ITEM(MSG_INIT_EEPROM,
        MSG_BUTTON_INIT, MSG_BUTTON_CANCEL,
        []{
          const bool inited = settings.init_eeprom();
          #if HAS_BUZZER
            ui.completion_feedback(inited);
          #endif
          UNUSED(inited);
        },
        ui.goto_previous_screen,
        GET_TEXT(MSG_INIT_EEPROM), (PGM_P)nullptr, PSTR("?")
      );
  #endif

  END_MENU();
}

#endif // HAS_LCD_MENU & REXYZ_TOUCH_MENU
