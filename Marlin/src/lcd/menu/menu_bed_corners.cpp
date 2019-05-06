/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2019 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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
// Level Bed Corners menu
//

#include "../../inc/MarlinConfigPre.h"

#if HAS_LCD_MENU && ENABLED(LEVEL_BED_CORNERS)

#include "menu.h"
#include "../../module/motion.h"
#include "../../module/planner.h"
#if HAS_BED_PROBE
  #include "../../module/probe.h"
#endif

#if HAS_LEVELING
  #include "../../feature/bedlevel/bedlevel.h"
#endif

#ifndef LEVEL_CORNERS_Z_HOP
  #define LEVEL_CORNERS_Z_HOP 4.0
#endif

#ifndef LEVEL_CORNERS_HEIGHT
  #define LEVEL_CORNERS_HEIGHT 0.0
#endif

static_assert(LEVEL_CORNERS_Z_HOP >= 0, "LEVEL_CORNERS_Z_HOP must be >= 0. Please update your configuration.");

#if HAS_LEVELING
  static bool leveling_was_active = false;
#endif

#if HAS_BED_PROBE
static float measured_z = NAN;
static float previous_zoffset;
static bool firstprobe = true;
#endif

/**
 * Level corners, starting in the front-left corner.
 */
static int8_t bed_corner;
static inline void _lcd_goto_next_corner() {
  line_to_z(LEVEL_CORNERS_Z_HOP);
  switch (bed_corner) {
    case 0:
      current_position[X_AXIS] = X_MIN_BED + LEVEL_CORNERS_INSET;
      current_position[Y_AXIS] = Y_MIN_BED + LEVEL_CORNERS_INSET;
      break;
    case 1:
      current_position[X_AXIS] = X_MAX_BED - LEVEL_CORNERS_INSET;
      break;
    case 2:
      current_position[Y_AXIS] = Y_MAX_BED - LEVEL_CORNERS_INSET;
      break;
    case 3:
      current_position[X_AXIS] = X_MIN_BED + LEVEL_CORNERS_INSET;
      break;
    #if ENABLED(LEVEL_CENTER_TOO)
      case 4:
        current_position[X_AXIS] = X_CENTER;
        current_position[Y_AXIS] = Y_CENTER;
        break;
    #endif
  }
  planner.buffer_line(current_position, MMM_TO_MMS(manual_feedrate_mm_m[X_AXIS]), active_extruder);
  line_to_z(LEVEL_CORNERS_HEIGHT);
  if (++bed_corner > 3
    #if ENABLED(LEVEL_CENTER_TOO)
      + 1
    #endif
  ) bed_corner = 0;
}

static inline void menu_level_bed_corners() {
  do_select_screen(
    PSTR(MSG_BUTTON_NEXT), PSTR(MSG_BUTTON_DONE),
    _lcd_goto_next_corner,
    []{
      #if HAS_LEVELING
        set_bed_leveling_enabled(leveling_was_active);
      #endif
      ui.goto_previous_screen_no_defer();
    },
    PSTR(
      #if ENABLED(LEVEL_CENTER_TOO)
        MSG_LEVEL_BED_NEXT_POINT
      #else
        MSG_NEXT_CORNER
      #endif
    ), NULL, PSTR("?")
  );
}

static inline void _lcd_level_bed_corners_homing() {
  _lcd_draw_homing();
  if (all_axes_homed()) {
    bed_corner = 0;
    ui.goto_screen(menu_level_bed_corners);
    set_ui_selection(true);
    _lcd_goto_next_corner();
  }
}

void _lcd_level_bed_corners() {
  ui.defer_status_screen();
  if (!all_axes_known()) {
    set_all_unhomed();
    enqueue_and_echo_commands_P(PSTR("G28"));
  }

  // Disable leveling so the planner won't mess with us
  #if HAS_LEVELING
    leveling_was_active = planner.leveling_active;
    set_bed_leveling_enabled(false);
  #endif

  ui.goto_screen(_lcd_level_bed_corners_homing);
}

//================================================

#if HAS_BED_PROBE

static inline void _lcd_probe_calibration_back() {
  STOW_PROBE();
  #if HAS_LEVELING
    set_bed_leveling_enabled(leveling_was_active);
  #endif
  zprobe_zoffset = previous_zoffset;
  ui.goto_previous_screen_no_defer();
}

static inline void _lcd_probe_corner() {
  line_to_z(Z_CLEARANCE_BETWEEN_PROBES);
  if (!DEPLOY_PROBE())
  {
    switch (bed_corner) {
      case 1:
        if (firstprobe) {
          measured_z = probe_pt(X_MIN_BED + LEVEL_CORNERS_INSET, Y_MIN_BED + LEVEL_CORNERS_INSET, PROBE_PT_RAISE, 1, true);
          firstprobe = false;
        }
        measured_z = probe_pt(X_MIN_BED + LEVEL_CORNERS_INSET, Y_MIN_BED + LEVEL_CORNERS_INSET, PROBE_PT_RAISE, 1, true);
        break;
      case 2:
        measured_z = probe_pt(X_MAX_BED - LEVEL_CORNERS_INSET, Y_MIN_BED + LEVEL_CORNERS_INSET, PROBE_PT_RAISE, 1, true);
        break;
      case 3:
        measured_z = probe_pt(X_MAX_BED - LEVEL_CORNERS_INSET, Y_MAX_BED - LEVEL_CORNERS_INSET, PROBE_PT_RAISE, 1, true);
        break;
      case 4:
        measured_z = probe_pt(X_MIN_BED + LEVEL_CORNERS_INSET, Y_MAX_BED - LEVEL_CORNERS_INSET, PROBE_PT_RAISE, 1, true);
        break;
    }
    if (DEBUGGING(LEVELING)) SERIAL_ECHOLNPAIR_F("Z Offset = ", measured_z);
    ui.lcdDrawUpdate = LCDVIEW_CALL_REDRAW_NEXT;
  }
}

static inline void _lcd_probe_next_corner() {
  if (++bed_corner > 4) bed_corner = 1;
  _lcd_probe_corner();
  ui.encoderPosition = 2;
}

static inline void menu_adjust_corner() {
  char mea_z[10];
  dtostrf(measured_z,1,2,mea_z);
  START_MENU();
  STATIC_ITEM("Offset ",false,false, mea_z);
  MENU_ITEM(function, MSG_BACK, _lcd_probe_calibration_back);
  MENU_ITEM(function,"Check Corner", _lcd_probe_corner);
  MENU_ITEM(function, MSG_NEXT_CORNER, _lcd_probe_next_corner);
  END_MENU();
}

static inline void _lcd_adjust_corner_homing() {
  _lcd_draw_homing();
  if (all_axes_homed()) {
    bed_corner = 1;
    measured_z = 0;
    firstprobe = true;
    ui.goto_screen(menu_adjust_corner);
    line_to_z(Z_CLEARANCE_BETWEEN_PROBES);
    current_position[X_AXIS] = X_MIN_BED + LEVEL_CORNERS_INSET - zprobe_xoffset;
    current_position[Y_AXIS] = Y_MIN_BED + LEVEL_CORNERS_INSET - zprobe_yoffset;
    planner.buffer_line(current_position, MMM_TO_MMS(manual_feedrate_mm_m[X_AXIS]), active_extruder);
  }
}

void _lcd_adjust_corner() {
  ui.defer_status_screen();
  STOW_PROBE();
  if (!all_axes_known()) {
    set_all_unhomed();
    enqueue_and_echo_commands_P(PSTR("G28"));
  }

  // Disable leveling so the planner won't mess with us
  #if HAS_LEVELING
    leveling_was_active = planner.leveling_active;
    set_bed_leveling_enabled(false);
  #endif
  ui.goto_screen(_lcd_adjust_corner_homing);
}

//========================================

static inline void _lcd_measure_offset() {
  line_to_z(Z_CLEARANCE_BETWEEN_PROBES);
  if (bed_corner == 0) ++bed_corner;
  if (!DEPLOY_PROBE()) {
    if (firstprobe) {
      measured_z = probe_pt(X_MIN_BED + LEVEL_CORNERS_INSET, Y_MIN_BED + LEVEL_CORNERS_INSET, PROBE_PT_RAISE, 1, true);
      firstprobe = false;
    }
    measured_z = probe_pt(X_MIN_BED + LEVEL_CORNERS_INSET, Y_MIN_BED + LEVEL_CORNERS_INSET, PROBE_PT_RAISE, 1, true);
    if (DEBUGGING(LEVELING)) SERIAL_ECHOLNPAIR_F("Z Offset = ", measured_z);
    ui.lcdDrawUpdate = LCDVIEW_CALL_REDRAW_NEXT;
  }
}

static inline void _lcd_save_offset() {
  //note: measured_z = run_z_probe() + zprobe_zoffset;
  zprobe_zoffset -= measured_z;
  previous_zoffset = zprobe_zoffset;
  measured_z = 0;
  ui.lcdDrawUpdate = LCDVIEW_REDRAW_NOW; 
  _lcd_probe_calibration_back();
}

static inline void menu_measure_probe_offset() {
  char mea_z[10];
  dtostrf(measured_z,1,2,mea_z);
  START_MENU();
  if (bed_corner == 0)
    STATIC_ITEM("Adjust Bed Height");
  else
    STATIC_ITEM("Offset ",false,false, mea_z);
  MENU_ITEM(function, MSG_BACK, _lcd_probe_calibration_back);
  MENU_ITEM(function, "Measure Offset", _lcd_measure_offset);
  MENU_ITEM(function, "Save Offset", _lcd_save_offset);
  END_MENU();
}

static inline void _lcd_measure_probe_offset_homing() {
  _lcd_draw_homing();
  if (all_axes_homed()) {
    bed_corner = 0;
    previous_zoffset = zprobe_zoffset;
    zprobe_zoffset = 0;
    measured_z = 0;
    firstprobe = true;
    ui.goto_screen(menu_measure_probe_offset);
    line_to_z(Z_CLEARANCE_BETWEEN_PROBES);
    current_position[X_AXIS] = X_MIN_BED + LEVEL_CORNERS_INSET;
    current_position[Y_AXIS] = Y_MIN_BED + LEVEL_CORNERS_INSET;
    planner.buffer_line(current_position, MMM_TO_MMS(manual_feedrate_mm_m[X_AXIS]), active_extruder);
    line_to_z(LEVEL_CORNERS_HEIGHT);
  }
}

void _lcd_measure_probe_offset() {
  ui.defer_status_screen();
  STOW_PROBE();
  if (!all_axes_known()) {
    set_all_unhomed();
    enqueue_and_echo_commands_P(PSTR("G28"));
  }

  // Disable leveling so the planner won't mess with us
  #if HAS_LEVELING
    leveling_was_active = planner.leveling_active;
    set_bed_leveling_enabled(false);
  #endif
  ui.goto_screen(_lcd_measure_probe_offset_homing);
}

#endif // HAS_BED_PROBE
#endif // HAS_LCD_MENU && LEVEL_BED_CORNERS
