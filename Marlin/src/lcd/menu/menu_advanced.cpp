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
// Advanced Settings Menus
//

#include "../../inc/MarlinConfigPre.h"

#if HAS_LCD_MENU

#include "menu.h"
#include "../../module/planner.h"

#if DISABLED(NO_VOLUMETRICS)
  #include "../../gcode/parser.h"
#endif

#if HAS_BED_PROBE
  #include "../../module/probe.h"
#endif

#if ENABLED(PIDTEMP)
  #include "../../module/temperature.h"
#endif

#if ENABLED(FILAMENT_RUNOUT_SENSOR) && FILAMENT_RUNOUT_DISTANCE_MM
  #include "../../feature/runout.h"
  float lcd_runout_distance_mm;
#endif



#if HAS_M206_COMMAND
  //
  // Set the home offset based on the current_position
  //
  void _lcd_set_home_offsets() {
    queue.inject_P(PSTR("M428"));
    ui.return_to_status();
  }
#endif

#if ENABLED(SD_FIRMWARE_UPDATE)
  #include "../../module/configuration_store.h"
#endif

#if DISABLED(NO_VOLUMETRICS) || ENABLED(ADVANCED_PAUSE_FEATURE)
  //
  // Advanced Settings > Filament
  //
  void menu_advanced_filament() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);

    #if ENABLED(LIN_ADVANCE)
      #if EXTRUDERS == 1
        MENU_ITEM_EDIT(float52, MSG_ADVANCE_K, &planner.extruder_advance_K[0], 0, 999);
      #elif EXTRUDERS > 1
        #define EDIT_ADVANCE_K(N) MENU_ITEM_EDIT(float52, MSG_ADVANCE_K MSG_E##N, &planner.extruder_advance_K[N-1], 0, 999)
        EDIT_ADVANCE_K(1);
        EDIT_ADVANCE_K(2);
        #if EXTRUDERS > 2
          EDIT_ADVANCE_K(3);
          #if EXTRUDERS > 3
            EDIT_ADVANCE_K(4);
            #if EXTRUDERS > 4
              EDIT_ADVANCE_K(5);
              #if EXTRUDERS > 5
                EDIT_ADVANCE_K(6);
              #endif // EXTRUDERS > 5
            #endif // EXTRUDERS > 4
          #endif // EXTRUDERS > 3
        #endif // EXTRUDERS > 2
      #endif // EXTRUDERS > 1
    #endif

    #if DISABLED(NO_VOLUMETRICS)
      MENU_ITEM_EDIT_CALLBACK(bool, MSG_VOLUMETRIC_ENABLED, &parser.volumetric_enabled, planner.calculate_volumetric_multipliers);

      if (parser.volumetric_enabled) {
        #if EXTRUDERS == 1
          MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_DIAM, &planner.filament_size[0], 1.5f, 3.25f, planner.calculate_volumetric_multipliers);
        #else // EXTRUDERS > 1
          #define EDIT_FIL_DIAM(N) MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_DIAM MSG_DIAM_E##N, &planner.filament_size[N-1], 1.5f, 3.25f, planner.calculate_volumetric_multipliers)
          MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_DIAM, &planner.filament_size[active_extruder], 1.5f, 3.25f, planner.calculate_volumetric_multipliers);
          EDIT_FIL_DIAM(1);
          EDIT_FIL_DIAM(2);
          #if EXTRUDERS > 2
            EDIT_FIL_DIAM(3);
            #if EXTRUDERS > 3
              EDIT_FIL_DIAM(4);
              #if EXTRUDERS > 4
                EDIT_FIL_DIAM(5);
                #if EXTRUDERS > 5
                  EDIT_FIL_DIAM(6);
                #endif // EXTRUDERS > 5
              #endif // EXTRUDERS > 4
            #endif // EXTRUDERS > 3
          #endif // EXTRUDERS > 2
        #endif // EXTRUDERS > 1
      }
    #endif

    #if ENABLED(ADVANCED_PAUSE_FEATURE)
      constexpr float extrude_maxlength =
        #if ENABLED(PREVENT_LENGTHY_EXTRUDE)
          EXTRUDE_MAXLENGTH
        #else
          999
        #endif
      ;

      #if EXTRUDERS == 1
        MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_FILAMENT_UNLOAD, &fc_settings[0].unload_length, 0, extrude_maxlength);
      #elif EXTRUDERS > 1
        #define EDIT_FIL_UNLOAD(N) MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_FILAMENT_UNLOAD MSG_DIAM_E##N, &fc_settings[N-1].unload_length, 0, extrude_maxlength)
        MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_FILAMENT_UNLOAD, &fc_settings[active_extruder].unload_length, 0, extrude_maxlength);
        EDIT_FIL_UNLOAD(1);
        EDIT_FIL_UNLOAD(2);
        #if EXTRUDERS > 2
          EDIT_FIL_UNLOAD(3);
          #if EXTRUDERS > 3
            EDIT_FIL_UNLOAD(4);
            #if EXTRUDERS > 4
              EDIT_FIL_UNLOAD(5);
              #if EXTRUDERS > 5
                EDIT_FIL_UNLOAD(6);
              #endif // EXTRUDERS > 5
            #endif // EXTRUDERS > 4
          #endif // EXTRUDERS > 3
        #endif // EXTRUDERS > 2
      #endif // EXTRUDERS > 1

      #if EXTRUDERS == 1
        MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_FILAMENT_LOAD, &fc_settings[0].load_length, 0, extrude_maxlength);
      #elif EXTRUDERS > 1
        #define EDIT_FIL_LOAD(N) MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_FILAMENT_LOAD MSG_DIAM_E##N, &fc_settings[N-1].load_length, 0, extrude_maxlength)
        MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_FILAMENT_LOAD, &fc_settings[active_extruder].load_length, 0, extrude_maxlength);
        EDIT_FIL_LOAD(1);
        EDIT_FIL_LOAD(2);
        #if EXTRUDERS > 2
          EDIT_FIL_LOAD(3);
          #if EXTRUDERS > 3
            EDIT_FIL_LOAD(4);
            #if EXTRUDERS > 4
              EDIT_FIL_LOAD(5);
              #if EXTRUDERS > 5
                EDIT_FIL_LOAD(6);
              #endif // EXTRUDERS > 5
            #endif // EXTRUDERS > 4
          #endif // EXTRUDERS > 3
        #endif // EXTRUDERS > 2
      #endif // EXTRUDERS > 1
    #endif

    #if ENABLED(FILAMENT_RUNOUT_SENSOR) && FILAMENT_RUNOUT_DISTANCE_MM
      MENU_ITEM_EDIT_CALLBACK(float3, MSG_RUNOUT_DISTANCE_MM, &lcd_runout_distance_mm, 1, 30, []{
        runout.set_runout_distance(lcd_runout_distance_mm);
      });
    #endif

    END_MENU();
  }

#endif // !NO_VOLUMETRICS || ADVANCED_PAUSE_FEATURE

void menu_advanced_homePos() {
  START_MENU();
  MENU_BACK(MSG_ADVANCED_SETTINGS);
  #if HAS_M206_COMMAND
    //
    // Set Home Offsets
    //
    MENU_ITEM(function, MSG_SET_HOME_OFFSETS, _lcd_set_home_offsets);
  #endif
  MENU_ITEM_EDIT(uint16_3, "Z Max Pos", &zv_max_pos, Z_MAX_POS - 30,  Z_MAX_POS + 20);
  END_MENU();
}

#if DISABLED(BABYSTEP_ZPROBE_OFFSET) && HAS_BED_PROBE
  //
  // Advanced Settings > Probe
  //
  void lcd_menu_choose_probe() {
    ui.encoder_direction_normal();
    #if ENABLED(PROBE_MANUALLY)
      if (int16_t(ui.encoderPosition) < REXYZPROBE_NO_PROBE) ui.encoderPosition = REXYZPROBE_NO_PROBE;
    #else
      if (int16_t(ui.encoderPosition) < REXYZPROBE_PROXYMITY) ui.encoderPosition = REXYZPROBE_PROXYMITY;
    #endif
    #if ENABLED(FIX_MOUNTED_PROBE)
      if (int16_t(ui.encoderPosition) > REXYZPROBE_MANUAL_DEPLOY) ui.encoderPosition = REXYZPROBE_MANUAL_DEPLOY;
    #else
      if (int16_t(ui.encoderPosition) > REXYZPROBE_NO_PROBE) ui.encoderPosition = REXYZPROBE_NO_PROBE;
    #endif
    if (ui.should_draw())
      switch (ui.encoderPosition) {
      case REXYZPROBE_NO_PROBE:
        draw_edit_screen(PSTR("Choose Probe"), "No Probe / Manual");
        break;
      case REXYZPROBE_PROXYMITY:
        draw_edit_screen(PSTR("Choose Probe"), "Proximity Sensor.");
        break;
      case REXYZPROBE_MANUAL_DEPLOY:
        draw_edit_screen(PSTR("Choose Probe"), "Manual Deploy Prb");
        break;
      case REXYZPROBE_BLTOUCH:
        draw_edit_screen(PSTR("Choose Probe"), "BL/3D Touch");
        break;
      }
    if (ui.lcd_clicked) {
      rexyz_probe_mode = ui.encoderPosition;
      if (ui.use_click()) ui.goto_previous_screen();
    }
  }

  void menu_advanced_probe() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);
    /*
    do { 
      _skipStatic = false; 
      if (_menuLineNr == _thisItemNr) { 
        if (encoderLine == _thisItemNr && ui.use_click()) { 
          _MENU_ITEM_MULTIPLIER_CHECK(false); 
          ui.save_previous_screen();
          ui.refresh();
          ui.encoderPosition = rexyz_probe_mode;
          ui.currentScreen = lcd_menu_choose_probe;
          if (screen_changed) return; 
        } 
        if (ui.should_draw()) 
          switch (rexyz_probe_mode) {
          case REXYZPROBE_NO_PROBE:
            draw_menu_item_edit(encoderLine == _thisItemNr, _lcdLineNr, PSTR("Probe"), "No Probe");
            break;
          case REXYZPROBE_PROXYMITY:
            draw_menu_item_edit(encoderLine == _thisItemNr, _lcdLineNr, PSTR("Probe"), "Proximity");
            break;
          case REXYZPROBE_MANUAL_DEPLOY:
            draw_menu_item_edit(encoderLine == _thisItemNr, _lcdLineNr, PSTR("Probe"), "Manual Deploy");
            break;
          case REXYZPROBE_BLTOUCH:
            draw_menu_item_edit(encoderLine == _thisItemNr, _lcdLineNr, PSTR("Probe"), "BL/3D Touch");
            break;
          }
      } 
      ++_thisItemNr; 
    }while(0);
     */

    _MENU_ITEM_VARIANT_P(float3, _edit, true, PSTR("Probe X Offset"), PSTR("Probe X Offset"),&zprobe_xoffset, 0, 30);
    _MENU_ITEM_VARIANT_P(float3, _edit, true, PSTR("Probe Y Offset"), PSTR("Probe Y Offset"),&zprobe_yoffset, -30, 30);
    _MENU_ITEM_VARIANT_P(float52, _edit, true, PSTR("Probe Z Offset"), PSTR("Probe Z Offset"),&zprobe_zoffset, Z_PROBE_OFFSET_RANGE_MIN, Z_PROBE_OFFSET_RANGE_MAX);
    END_MENU();
  }
#endif // !BABYSTEP_ZPROBE_OFFSET && HAS_BED_PROBE


//
// Advanced Settings > Temperature helpers
//

#if ENABLED(PID_AUTOTUNE_MENU)

  #if ENABLED(PIDTEMP)
    int16_t autotune_temp[HOTENDS] = ARRAY_BY_HOTENDS1(200);
  #endif

  #if ENABLED(PIDTEMPBED)
    int16_t autotune_temp_bed = 70;
  #endif

  void _lcd_autotune(const int16_t e) {
    char cmd[30];
    sprintf_P(cmd, PSTR("M303 U1 E%i S%i"), e,
      #if HAS_PID_FOR_BOTH
        e < 0 ? autotune_temp_bed : autotune_temp[e]
      #elif ENABLED(PIDTEMPBED)
        autotune_temp_bed
      #else
        autotune_temp[e]
      #endif
    );
    lcd_enqueue_one_now(cmd);
  }

#endif // PID_AUTOTUNE_MENU

#if ENABLED(PID_EDIT_MENU)

  float raw_Ki, raw_Kd; // place-holders for Ki and Kd edits

  // Helpers for editing PID Ki & Kd values
  // grab the PID value out of the temp variable; scale it; then update the PID driver
  void copy_and_scalePID_i(int16_t e) {
    #if DISABLED(PID_PARAMS_PER_HOTEND) || HOTENDS == 1
      UNUSED(e);
    #endif
    PID_PARAM(Ki, e) = scalePID_i(raw_Ki);
    thermalManager.updatePID();
  }
  void copy_and_scalePID_d(int16_t e) {
    #if DISABLED(PID_PARAMS_PER_HOTEND) || HOTENDS == 1
      UNUSED(e);
    #endif
    PID_PARAM(Kd, e) = scalePID_d(raw_Kd);
    thermalManager.updatePID();
  }
  #define _DEFINE_PIDTEMP_BASE_FUNCS(N) \
    void copy_and_scalePID_i_E ## N() { copy_and_scalePID_i(N); } \
    void copy_and_scalePID_d_E ## N() { copy_and_scalePID_d(N); }

#else

  #define _DEFINE_PIDTEMP_BASE_FUNCS(N) //

#endif

#if ENABLED(PID_AUTOTUNE_MENU)
  #define DEFINE_PIDTEMP_FUNCS(N) \
    _DEFINE_PIDTEMP_BASE_FUNCS(N); \
    void lcd_autotune_callback_E ## N() { _lcd_autotune(N); } //
#else
  #define DEFINE_PIDTEMP_FUNCS(N) _DEFINE_PIDTEMP_BASE_FUNCS(N); //
#endif

#if HOTENDS
  DEFINE_PIDTEMP_FUNCS(0);
  #if ENABLED(PID_PARAMS_PER_HOTEND)
    #if HOTENDS > 1
      DEFINE_PIDTEMP_FUNCS(1);
      #if HOTENDS > 2
        DEFINE_PIDTEMP_FUNCS(2);
        #if HOTENDS > 3
          DEFINE_PIDTEMP_FUNCS(3);
          #if HOTENDS > 4
            DEFINE_PIDTEMP_FUNCS(4);
            #if HOTENDS > 5
              DEFINE_PIDTEMP_FUNCS(5);
            #endif // HOTENDS > 5
          #endif // HOTENDS > 4
        #endif // HOTENDS > 3
      #endif // HOTENDS > 2
    #endif // HOTENDS > 1
  #endif // PID_PARAMS_PER_HOTEND
#endif // HOTENDS

#define SHOW_MENU_ADVANCED_TEMPERATURE ((ENABLED(AUTOTEMP) && HAS_TEMP_HOTEND) || EITHER(PID_AUTOTUNE_MENU, PID_EDIT_MENU))

//
// Advanced Settings > Temperature
//
#if SHOW_MENU_ADVANCED_TEMPERATURE

  void menu_advanced_temperature() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);
    //
    // Autotemp, Min, Max, Fact
    //
    #if ENABLED(AUTOTEMP) && HAS_TEMP_HOTEND
      MENU_ITEM_EDIT(bool, MSG_AUTOTEMP, &planner.autotemp_enabled);
      MENU_ITEM_EDIT(float3, MSG_MIN, &planner.autotemp_min, 0, float(HEATER_0_MAXTEMP) - 15);
      MENU_ITEM_EDIT(float3, MSG_MAX, &planner.autotemp_max, 0, float(HEATER_0_MAXTEMP) - 15);
      MENU_ITEM_EDIT(float52, MSG_FACTOR, &planner.autotemp_factor, 0, 10);
    #endif

    //
    // PID-P, PID-I, PID-D, PID-C, PID Autotune
    // PID-P E1, PID-I E1, PID-D E1, PID-C E1, PID Autotune E1
    // PID-P E2, PID-I E2, PID-D E2, PID-C E2, PID Autotune E2
    // PID-P E3, PID-I E3, PID-D E3, PID-C E3, PID Autotune E3
    // PID-P E4, PID-I E4, PID-D E4, PID-C E4, PID Autotune E4
    // PID-P E5, PID-I E5, PID-D E5, PID-C E5, PID Autotune E5
    //
    #if ENABLED(PID_EDIT_MENU)

      #define _PID_BASE_MENU_ITEMS(ELABEL, eindex) \
        raw_Ki = unscalePID_i(PID_PARAM(Ki, eindex)); \
        raw_Kd = unscalePID_d(PID_PARAM(Kd, eindex)); \
        MENU_ITEM_EDIT(float52sign, MSG_PID_P ELABEL, &PID_PARAM(Kp, eindex), 1, 9990); \
        MENU_ITEM_EDIT_CALLBACK(float52sign, MSG_PID_I ELABEL, &raw_Ki, 0.01f, 9990, copy_and_scalePID_i_E ## eindex); \
        MENU_ITEM_EDIT_CALLBACK(float52sign, MSG_PID_D ELABEL, &raw_Kd, 1, 9990, copy_and_scalePID_d_E ## eindex)

      #if ENABLED(PID_EXTRUSION_SCALING)
        #define _PID_EDIT_MENU_ITEMS(ELABEL, eindex) \
          _PID_BASE_MENU_ITEMS(ELABEL, eindex); \
          MENU_ITEM_EDIT(float3, MSG_PID_C ELABEL, &PID_PARAM(Kc, eindex), 1, 9990)
      #else
        #define _PID_EDIT_MENU_ITEMS(ELABEL, eindex) _PID_BASE_MENU_ITEMS(ELABEL, eindex)
      #endif

    #else

      #define _PID_EDIT_MENU_ITEMS(ELABEL, eindex) NOOP

    #endif

    #if ENABLED(PID_AUTOTUNE_MENU)
      #define PID_EDIT_MENU_ITEMS(ELABEL, eindex) \
        _PID_EDIT_MENU_ITEMS(ELABEL, eindex); \
        MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(int3, MSG_PID_AUTOTUNE ELABEL, &autotune_temp[eindex], 150, heater_maxtemp[eindex] - 15, lcd_autotune_callback_E ## eindex)
    #else
      #define PID_EDIT_MENU_ITEMS(ELABEL, eindex) _PID_EDIT_MENU_ITEMS(ELABEL, eindex)
    #endif

    #if ENABLED(PID_PARAMS_PER_HOTEND) && HOTENDS > 1
      PID_EDIT_MENU_ITEMS(" " MSG_E1, 0);
      PID_EDIT_MENU_ITEMS(" " MSG_E2, 1);
      #if HOTENDS > 2
        PID_EDIT_MENU_ITEMS(" " MSG_E3, 2);
        #if HOTENDS > 3
          PID_EDIT_MENU_ITEMS(" " MSG_E4, 3);
          #if HOTENDS > 4
            PID_EDIT_MENU_ITEMS(" " MSG_E5, 4);
            #if HOTENDS > 5
              PID_EDIT_MENU_ITEMS(" " MSG_E6, 5);
            #endif // HOTENDS > 5
          #endif // HOTENDS > 4
        #endif // HOTENDS > 3
      #endif // HOTENDS > 2
    #else // !PID_PARAMS_PER_HOTEND || HOTENDS == 1
      PID_EDIT_MENU_ITEMS("", 0);
    #endif // !PID_PARAMS_PER_HOTEND || HOTENDS == 1

    END_MENU();
  }

#endif // SHOW_MENU_ADVANCED_TEMPERATURE

#if DISABLED(SLIM_LCD_MENUS)

  #if ENABLED(EEPROM_SETTINGS)

    #include "../../module/configuration_store.h"

    static void lcd_init_eeprom_confirm() {
      do_select_screen(
        PSTR(MSG_BUTTON_INIT), PSTR(MSG_BUTTON_CANCEL),
        []{
          const bool inited = settings.init_eeprom();
          #if HAS_BUZZER
            ui.completion_feedback(inited);
          #endif
          UNUSED(inited);
        },
        ui.goto_previous_screen,
        PSTR(MSG_INIT_EEPROM), nullptr, PSTR("?")
      );
    }

  #endif

#endif // !SLIM_LCD_MENUS

void menu_advanced_settings() {
  #if ENABLED(FILAMENT_RUNOUT_SENSOR) && FILAMENT_RUNOUT_DISTANCE_MM
    lcd_runout_distance_mm = runout.runout_distance();
  #endif
  START_MENU();
  MENU_BACK(MSG_CONFIGURATION);
  MENU_ITEM(submenu, MSG_MOTION, menu_adv_motion);

  #if SHOW_MENU_ADVANCED_TEMPERATURE
    MENU_ITEM(submenu, MSG_TEMPERATURE, menu_advanced_temperature);
  #endif

  #if DISABLED(NO_VOLUMETRICS) || ENABLED(ADVANCED_PAUSE_FEATURE)
    MENU_ITEM(submenu, MSG_FILAMENT, menu_advanced_filament);
  #elif ENABLED(LIN_ADVANCE)
    #if EXTRUDERS == 1
      MENU_ITEM_EDIT(float52, MSG_ADVANCE_K, &planner.extruder_advance_K[0], 0, 999);
    #elif EXTRUDERS > 1
      #define EDIT_ADVANCE_K(N) MENU_ITEM_EDIT(float52, MSG_ADVANCE_K MSG_E##N, &planner.extruder_advance_K[N-1], 0, 999)
      EDIT_ADVANCE_K(1);
      EDIT_ADVANCE_K(2);
      #if EXTRUDERS > 2
        EDIT_ADVANCE_K(3);
        #if EXTRUDERS > 3
          EDIT_ADVANCE_K(4);
          #if EXTRUDERS > 4
            EDIT_ADVANCE_K(5);
            #if EXTRUDERS > 5
              EDIT_ADVANCE_K(6);
            #endif // EXTRUDERS > 5
          #endif // EXTRUDERS > 4
        #endif // EXTRUDERS > 3
      #endif // EXTRUDERS > 2
    #endif // EXTRUDERS > 1
  #endif

  // M540 S - Abort on endstop hit when SD printing
  #if ENABLED(SD_ABORT_ON_ENDSTOP_HIT)
    MENU_ITEM_EDIT(bool, MSG_ENDSTOP_ABORT, &planner.abort_on_endstop_hit);
  #endif

  #if ENABLED(BABYSTEP_ZPROBE_OFFSET)
    MENU_ITEM(subedit, MSG_ZPROBE_ZOFFSET, lcd_babystep_zoffset);
  #elif HAS_BED_PROBE
    MENU_ITEM(subedit, "Probe", menu_advanced_probe);
  #endif

  #if DISABLED(DELTA)
    MENU_ITEM(submenu, "Home Position", menu_advanced_homePos);
  #endif  

  #if ENABLED(TOUCH_CALIBRATION)
    MENU_ITEM(function, MSG_TOUCHSCREEN_CALIBRATION, menu_touch_calibration);
  #endif

  #if ENABLED(SD_FIRMWARE_UPDATE)
    bool sd_update_state = settings.sd_update_status();
    MENU_ITEM_EDIT_CALLBACK(bool, MSG_MEDIA_UPDATE, &sd_update_state, []{
      //
      // Toggle the SD Firmware Update state in EEPROM
      //
      const bool new_state = !settings.sd_update_status(),
                 didset = settings.set_sd_update_status(new_state);
      #if HAS_BUZZER
        ui.completion_feedback(didset);
      #endif
      ui.return_to_status();
      if (new_state) LCD_MESSAGEPGM(MSG_RESET_PRINTER); else ui.reset_status();
    });
  #endif

  #if ENABLED(EEPROM_SETTINGS) && DISABLED(SLIM_LCD_MENUS)
    MENU_ITEM(subselect, MSG_INIT_EEPROM, lcd_init_eeprom_confirm);
  #endif

  END_MENU();
}

#endif // HAS_LCD_MENU
