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

#if ENABLED(PIDTEMP)
  #include "../../module/temperature.h"
#endif

void menu_tmc();
void menu_backlash();

#if ENABLED(DAC_STEPPER_CURRENT)

  #include "../../feature/dac/stepper_dac.h"

  xyze_uint8_t driverPercent;
  inline void dac_driver_getValues() { LOOP_XYZE(i) driverPercent[i] = dac_current_get_percent((AxisEnum)i); }
  static void dac_driver_commit() { dac_current_set_percents(driverPercent); }

  void menu_dac() {
    dac_driver_getValues();
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);
    #define EDIT_DAC_PERCENT(N) MENU_ITEM_EDIT_CALLBACK(uint8, MSG_##N " " MSG_DAC_PERCENT, &driverPercent[_AXIS(N)], 0, 100, dac_driver_commit)
    EDIT_DAC_PERCENT(X);
    EDIT_DAC_PERCENT(Y);
    EDIT_DAC_PERCENT(Z);
    EDIT_DAC_PERCENT(E);
    MENU_ITEM(function, MSG_DAC_EEPROM_WRITE, dac_commit_eeprom);
    END_MENU();
  }

#endif

#if HAS_MOTOR_CURRENT_PWM

  #include "../../module/stepper.h"

  void menu_pwm() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);
    #define EDIT_CURRENT_PWM(LABEL,I) MENU_ITEM_EDIT_CALLBACK(long5, LABEL, &stepper.motor_current_setting[I], 100, 2000, stepper.refresh_motor_power)
    #if PIN_EXISTS(MOTOR_CURRENT_PWM_XY)
      EDIT_CURRENT_PWM(MSG_X MSG_Y, 0);
    #endif
    #if PIN_EXISTS(MOTOR_CURRENT_PWM_Z)
      EDIT_CURRENT_PWM(MSG_Z, 1);
    #endif
    #if PIN_EXISTS(MOTOR_CURRENT_PWM_E)
      EDIT_CURRENT_PWM(MSG_E, 2);
    #endif
    END_MENU();
  }

#endif

#if DISABLED(SLIM_LCD_MENUS)

  void _reset_acceleration_rates() { planner.reset_acceleration_rates(); }
  #if ENABLED(DISTINCT_E_FACTORS)
    void _reset_e_acceleration_rate(const uint8_t e) { if (e == active_extruder) _reset_acceleration_rates(); }
    void _reset_e0_acceleration_rate() { _reset_e_acceleration_rate(0); }
    void _reset_e1_acceleration_rate() { _reset_e_acceleration_rate(1); }
    #if E_STEPPERS > 2
      void _reset_e2_acceleration_rate() { _reset_e_acceleration_rate(2); }
      #if E_STEPPERS > 3
        void _reset_e3_acceleration_rate() { _reset_e_acceleration_rate(3); }
        #if E_STEPPERS > 4
          void _reset_e4_acceleration_rate() { _reset_e_acceleration_rate(4); }
          #if E_STEPPERS > 5
            void _reset_e5_acceleration_rate() { _reset_e_acceleration_rate(5); }
          #endif // E_STEPPERS > 5
        #endif // E_STEPPERS > 4
      #endif // E_STEPPERS > 3
    #endif // E_STEPPERS > 2
  #endif

  void _planner_refresh_positioning() { planner.refresh_positioning(); }
  #if ENABLED(DISTINCT_E_FACTORS)
    void _planner_refresh_e_positioning(const uint8_t e) {
      if (e == active_extruder)
        _planner_refresh_positioning();
      else
        planner.steps_to_mm[E_AXIS_N(e)] = 1.0f / planner.settings.axis_steps_per_mm[E_AXIS_N(e)];
    }
    void _planner_refresh_e0_positioning() { _planner_refresh_e_positioning(0); }
    void _planner_refresh_e1_positioning() { _planner_refresh_e_positioning(1); }
    #if E_STEPPERS > 2
      void _planner_refresh_e2_positioning() { _planner_refresh_e_positioning(2); }
      #if E_STEPPERS > 3
        void _planner_refresh_e3_positioning() { _planner_refresh_e_positioning(3); }
        #if E_STEPPERS > 4
          void _planner_refresh_e4_positioning() { _planner_refresh_e_positioning(4); }
          #if E_STEPPERS > 5
            void _planner_refresh_e5_positioning() { _planner_refresh_e_positioning(5); }
          #endif // E_STEPPERS > 5
        #endif // E_STEPPERS > 4
      #endif // E_STEPPERS > 3
    #endif // E_STEPPERS > 2
  #endif

  // M203 / M205 Velocity options
  void menu_advanced_velocity() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);

    // M203 Max Feedrate
    static constexpr uint32_t max_feedrate[] = MAX_FEEDRATE_LIMIT;
    #define EDIT_VMAX(N) MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_VMAX MSG_##N, &planner.settings.max_feedrate_mm_s[_AXIS(N)], 1, max_feedrate[_AXIS(N)])

    EDIT_VMAX(A);
    EDIT_VMAX(B);
    EDIT_VMAX(C);

    #if ENABLED(DISTINCT_E_FACTORS)
      #define EDIT_VMAX_E(N) MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_VMAX MSG_E##N, &planner.settings.max_feedrate_mm_s[E_AXIS_N(N-1)], 1, 999)
      MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_VMAX MSG_E, &planner.settings.max_feedrate_mm_s[E_AXIS_N(active_extruder)], 1, 999);
      EDIT_VMAX_E(1);
      EDIT_VMAX_E(2);
      #if E_STEPPERS > 2
        EDIT_VMAX_E(3);
        #if E_STEPPERS > 3
          EDIT_VMAX_E(4);
          #if E_STEPPERS > 4
            EDIT_VMAX_E(5);
            #if E_STEPPERS > 5
              EDIT_VMAX_E(6);
            #endif // E_STEPPERS > 5
          #endif // E_STEPPERS > 4
        #endif // E_STEPPERS > 3
      #endif // E_STEPPERS > 2
    #elif E_STEPPERS
      MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_VMAX MSG_E, &planner.settings.max_feedrate_mm_s[E_AXIS], 1, max_feedrate[E_AXIS]);
    #endif

    // M205 S Min Feedrate
    MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_VMIN, &planner.settings.min_feedrate_mm_s, 0, 999);

    // M205 T Min Travel Feedrate
    MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_VTRAV_MIN, &planner.settings.min_travel_feedrate_mm_s, 0, 999);

    END_MENU();
  }

  // M201 / M204 Accelerations
  void menu_advanced_acceleration() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);

    // M204 P Acceleration
    MENU_MULTIPLIER_ITEM_EDIT(float5_25, MSG_ACC, &planner.settings.acceleration, 25, 99000);

    // M204 R Retract Acceleration
    MENU_MULTIPLIER_ITEM_EDIT(float5, MSG_A_RETRACT, &planner.settings.retract_acceleration, 100, 99000);

    // M204 T Travel Acceleration
    MENU_MULTIPLIER_ITEM_EDIT(float5_25, MSG_A_TRAVEL, &planner.settings.travel_acceleration, 25, 99000);

    // M201 settings
    static constexpr uint32_t max_accel[] = MAX_ACCELERATION_LIMIT;
    #define EDIT_AMAX(Q,L) MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(long5_25, MSG_AMAX MSG_##Q, &planner.settings.max_acceleration_mm_per_s2[_AXIS(Q)], L, max_accel[_AXIS(Q)], _reset_acceleration_rates)

    EDIT_AMAX(A,100);
    EDIT_AMAX(B,100);
    EDIT_AMAX(C, 10);

    #if ENABLED(DISTINCT_E_FACTORS)
      #define EDIT_AMAX_E(N,E) MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_E##N, &planner.settings.max_acceleration_mm_per_s2[E_AXIS_N(E)], 100, 99000, _reset_e##E##_acceleration_rate)
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_E, &planner.settings.max_acceleration_mm_per_s2[E_AXIS_N(active_extruder)], 100, 99000, _reset_acceleration_rates);
      EDIT_AMAX_E(1,0);
      EDIT_AMAX_E(2,1);
      #if E_STEPPERS > 2
        EDIT_AMAX_E(3,2);
        #if E_STEPPERS > 3
          EDIT_AMAX_E(4,3);
          #if E_STEPPERS > 4
            EDIT_AMAX_E(5,4);
            #if E_STEPPERS > 5
              EDIT_AMAX_E(6,5);
            #endif // E_STEPPERS > 5
          #endif // E_STEPPERS > 4
        #endif // E_STEPPERS > 3
      #endif // E_STEPPERS > 2
    #elif E_STEPPERS
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_E, &planner.settings.max_acceleration_mm_per_s2[E_AXIS], 100, max_accel[E_AXIS], _reset_acceleration_rates);
    #endif

    END_MENU();
  }

  // M205 Jerk
  void menu_advanced_jerk() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);

    #if ENABLED(JUNCTION_DEVIATION)
      #if ENABLED(LIN_ADVANCE)
        MENU_ITEM_EDIT_CALLBACK(float43, MSG_JUNCTION_DEVIATION, &planner.junction_deviation_mm, 0.01f, MAX_JUNCTION_DEVIATION_MM, planner.recalculate_max_e_jerk);
      #else
        MENU_ITEM_EDIT(float43, MSG_JUNCTION_DEVIATION, &planner.junction_deviation_mm, 0.01f, MAX_JUNCTION_DEVIATION_MM);
      #endif
    #endif
    #if HAS_CLASSIC_JERK
      #define EDIT_JERK(N) MENU_MULTIPLIER_ITEM_EDIT(float3, MSG_V##N##_JERK, &planner.max_jerk[_AXIS(N)], 1, 990)
      EDIT_JERK(A);
      EDIT_JERK(B);
      #if ENABLED(DELTA)
        EDIT_JERK(C);
      #else
        MENU_MULTIPLIER_ITEM_EDIT(float52sign, MSG_VC_JERK, &planner.max_jerk.c, 0.1f, 990);
      #endif
      #if !BOTH(JUNCTION_DEVIATION, LIN_ADVANCE)
        EDIT_JERK(E);
      #endif
    #endif

    END_MENU();
  }

  // M92 Steps-per-mm
  void menu_advanced_steps_per_mm() {
    START_MENU();
    MENU_BACK(MSG_ADVANCED_SETTINGS);

    #define EDIT_QSTEPS(Q) MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float51, MSG_##Q##STEPS, &planner.settings.axis_steps_per_mm[_AXIS(Q)], 5, 9999, _planner_refresh_positioning)
    EDIT_QSTEPS(A);
    EDIT_QSTEPS(B);
    EDIT_QSTEPS(C);

    #if ENABLED(DISTINCT_E_FACTORS)
      #define EDIT_ESTEPS(N,E) MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float51, MSG_E##N##STEPS, &planner.settings.axis_steps_per_mm[E_AXIS_N(E)], 5, 9999, _planner_refresh_e##E##_positioning)
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float51, MSG_ESTEPS, &planner.settings.axis_steps_per_mm[E_AXIS_N(active_extruder)], 5, 9999, _planner_refresh_positioning);
      EDIT_ESTEPS(1,0);
      EDIT_ESTEPS(2,1);
      #if E_STEPPERS > 2
        EDIT_ESTEPS(3,2);
        #if E_STEPPERS > 3
          EDIT_ESTEPS(4,3);
          #if E_STEPPERS > 4
            EDIT_ESTEPS(5,4);
            #if E_STEPPERS > 5
              EDIT_ESTEPS(6,5);
            #endif // E_STEPPERS > 5
          #endif // E_STEPPERS > 4
        #endif // E_STEPPERS > 3
      #endif // E_STEPPERS > 2
    #elif E_STEPPERS
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float51, MSG_ESTEPS, &planner.settings.axis_steps_per_mm[E_AXIS], 5, 9999, _planner_refresh_positioning);
    #endif

    END_MENU();
  }

#endif // !SLIM_LCD_MENUS

void menu_adv_motion() {
  START_MENU();
  MENU_BACK(MSG_ADVANCED_SETTINGS);

  #if DISABLED(SLIM_LCD_MENUS)

    // M203 / M205 - Feedrate items
    MENU_ITEM(submenu, MSG_VELOCITY, menu_advanced_velocity);

    // M201 - Acceleration items
    MENU_ITEM(submenu, MSG_ACCELERATION, menu_advanced_acceleration);

    // M205 - Max Jerk
    MENU_ITEM(submenu, MSG_JERK, menu_advanced_jerk);

    if (!printer_busy()) {
      // M92 - Steps Per mm
      MENU_ITEM(submenu, MSG_STEPS_PER_MM, menu_advanced_steps_per_mm);
    }
  #endif // !SLIM_LCD_MENUS

  #if ENABLED(BACKLASH_GCODE)
    MENU_ITEM(submenu, MSG_BACKLASH, menu_backlash);
  #endif

  #if ENABLED(DAC_STEPPER_CURRENT)
    MENU_ITEM(submenu, MSG_DRIVE_STRENGTH, menu_dac);
  #endif
  #if HAS_MOTOR_CURRENT_PWM
    MENU_ITEM(submenu, MSG_DRIVE_STRENGTH, menu_pwm);
  #endif

  #if HAS_TRINAMIC
    MENU_ITEM(submenu, MSG_TMC_DRIVERS, menu_tmc);
  #endif

  END_MENU();
}

#endif // HAS_LCD_MENU
