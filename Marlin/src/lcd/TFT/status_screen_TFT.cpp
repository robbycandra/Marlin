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
// status_screen_320x240.cpp
// Standard Status Screen for Graphical Display
//

#include "../../inc/MarlinConfigPre.h"

#if HAS_GRAPHICAL_LCD && DISABLED(LIGHTWEIGHT_UI) && HAS_FULL_SCALE_TFT

#if ENABLED(FULL_SCALE_TFT_480X320)
  #include "status_screen_480x320.h"
#endif
#if ENABLED(FULL_SCALE_TFT_320X240)
  #include "status_screen_320x240.h"
#endif

#include "ultralcd_TFT.h"
#include "../ultralcd.h"
#include "../lcdprint.h"
#include "../../libs/numtostr.h"

#include "../../module/motion.h"
#include "../../module/temperature.h"
#include "../../lcd/menu/menu.h"

#if ENABLED(FILAMENT_LCD_DISPLAY)
  #include "../../feature/filwidth.h"
  #include "../../module/planner.h"
  #include "../../gcode/parser.h"
#elif HAS_ABL_OR_UBL
  #include "../../module/planner.h"
#endif

#if ENABLED(SDSUPPORT)
  #include "../../sd/cardreader.h"
#endif

#if HAS_FILAMENT_SENSOR
  #include "../../feature/runout.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../feature/power_loss_recovery.h"
#endif

#if HAS_PRINT_PROGRESS
  #include "../../module/printcounter.h"
#endif

#if DUAL_MIXING_EXTRUDER
  #include "../../feature/mixing.h"
#endif

#define X_LABEL_POS     (COL_WIDTH - STATUS_FONT_WIDTH) / 2
#define X_VALUE_POS     (COL_WIDTH - (MENU_FONT_WIDTH*7)) / 2

#define DO_DRAW_BED (HAS_HEATED_BED && STATUS_BED_WIDTH && HOTENDS <= 3 && DISABLED(STATUS_COMBINE_HEATERS))
#define DO_DRAW_FAN (HAS_FAN0 && STATUS_FAN_WIDTH && STATUS_FAN_FRAMES)
#define ANIM_HOTEND (HOTENDS && ENABLED(STATUS_HOTEND_ANIM))
#define ANIM_BED (DO_DRAW_BED && ENABLED(STATUS_BED_ANIM))
#define ANIM_CHAMBER (HAS_HEATED_CHAMBER && ENABLED(STATUS_CHAMBER_ANIM))

#if ANIM_HOTEND || ANIM_BED
  uint8_t heat_bits;
#endif
#if ANIM_HOTEND
  #define HOTEND_ALT(N) TEST(heat_bits, N)
#else
  #define HOTEND_ALT(N) false
#endif
#if ANIM_BED
  #define BED_ALT() TEST(heat_bits, 7)
#else
  #define BED_ALT() false
#endif
#if ANIM_CHAMBER
  #define CHAMBER_ALT() TEST(heat_bits, 6)
#else
  #define CHAMBER_ALT() false
#endif

#define MAX_HOTEND_DRAW _MIN(HOTENDS, ((LCD_PIXEL_WIDTH - (STATUS_LOGO_BYTEWIDTH + STATUS_FAN_BYTEWIDTH) * 8) / (STATUS_HEATERS_XSPACE)))

#if ENABLED(MARLIN_DEV_MODE)
  #define SHOW_ON_STATE READ(X_MIN_PIN)
#else
  #define SHOW_ON_STATE false
#endif

u8g_uint_t off_x = 0, row_h = 0;
u8g_uint_t row_str1_base, row_str2_base, row_str1_top, row_str2_top, row_str1_botm, row_str2_botm;

void run_status_screen_touch_command() {
  int8_t touched_item_number;
  if (ui.lcd_menu_touched_coord & B10000000) {
    const uint8_t row = (ui.lcd_menu_touched_coord & B01111000) >> 3;
    const uint8_t col = (ui.lcd_menu_touched_coord & B00000111); 
    touched_item_number = (int)(row / 3) * 4 + (col >> 1);
    switch(touched_item_number) {
      case 0:
        #if HAS_BUZZER
          ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
        MenuItem_gcode::action("G28");   
        break;
      case 1:
        #if HAS_BUZZER
          ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
        if (!TEST(axis_homed, X_AXIS) || !TEST(axis_known_position, X_AXIS)) {
          MenuItem_gcode::action(PSTR("G28 X"));   
        } 
        move_menu_scale = 10;
        ui.goto_screen(lcd_move_x);
        break;
      case 2:
        #if HAS_BUZZER
          ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
        if (!TEST(axis_homed, Y_AXIS) || !TEST(axis_known_position, Y_AXIS)) {
          MenuItem_gcode::action(PSTR("G28 Y"));   
        } 
        move_menu_scale = 10;
        ui.goto_screen(lcd_move_y);
        break;
      case 3:
        #if HAS_BUZZER
          ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
        if (!TEST(axis_homed, Z_AXIS) || !TEST(axis_known_position, Z_AXIS)) {
          MenuItem_gcode::action(PSTR("G28 Z"));   
        } 
        move_menu_scale = 10;
        ui.goto_screen(lcd_move_z);
        break;
      case 5:
        #if HAS_BUZZER
          ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
        MenuItem_int3::action_edit(PSTR(MSG_NOZZLE),&thermalManager.temp_hotend[0].target, 0, HEATER_0_MAXTEMP - 15, thermalManager.start_watching_E0);
        break;
      case 6:
        #if HAS_HEATED_BED
          #if HAS_BUZZER
            ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
          #endif
          MenuItem_int3::action_edit(PSTR(MSG_BED),&thermalManager.temp_bed.target, 0, BED_MAXTEMP - 10, thermalManager.start_watching_bed);
          break;
        #endif
      case 7:
        #if HAS_BUZZER
          ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
        MenuItem_percent::action_edit(PSTR(MSG_FAN_SPEED),&thermalManager.lcd_tmpfan_speed[0], 0, 255, thermalManager.lcd_setFanSpeed0);
        break;
      case 9:
        #if HAS_BUZZER
          ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
        #endif
        if (card.isMounted()) {
          if (!card.isFileOpen()) {
            ui.goto_screen(menu_media);
          }
        }
        break;
    }
    ui.lcd_menu_touched_coord = 0;     
  } 
}

FORCE_INLINE void _draw_centered_temp(const int16_t temp, const uint8_t tx, const uint8_t ty) {
  const char *str = i16tostr3(temp);
  const uint8_t len = str[0] != ' ' ? 3 : str[1] != ' ' ? 2 : 1;
  lcd_moveto(tx - len * (MENU_FONT_WIDTH) / 2 + 1, ty);
  lcd_put_u8str(&str[3-len]);
  lcd_put_wchar(LCD_STR_DEGREE[0]);
}

FORCE_INLINE void _draw_heater_status(const heater_ind_t heater, const bool blink) {
  #if !HEATER_IDLE_HANDLER
    UNUSED(blink);
  #endif

  #if HAS_HEATED_BED
    const bool isBed = heater < 0;
    #define IFBED(A,B) (isBed ? (A) : (B))
  #else
    #define IFBED(A,B) (B)
  #endif

  #if ENABLED(MARLIN_DEV_MODE)
    constexpr bool isHeat = true;
  #else
    const bool isHeat = IFBED(BED_ALT(), HOTEND_ALT(heater));
  #endif

  #if HOTENDS > 1
    const u8b_uint_t tx = IFBED(STATUS_BED_TEXT_X, STATUS_HOTEND_TEXT_X(heater));
  #else
    const u8g_uint_t tx = IFBED(STATUS_BED_TEXT_X, STATUS_HOTEND_TEXT_X(heater+1));
  #endif

  #if ENABLED(MARLIN_DEV_MODE)
    const float temp = 20 + (millis() >> 8) % IFBED(100, 200);
    const float target = IFBED(100, 200);
  #else
    const float temp = IFBED(thermalManager.degBed(), thermalManager.degHotend(heater)),
                target = IFBED(thermalManager.degTargetBed(), thermalManager.degTargetHotend(heater));
  #endif

  #if DISABLED(STATUS_HOTEND_ANIM)
    #define STATIC_HOTEND true
    #define HOTEND_DOT    isHeat
  #else
    #define STATIC_HOTEND false
    #define HOTEND_DOT    false
  #endif

  #if HAS_HEATED_BED && DISABLED(STATUS_BED_ANIM)
    #define STATIC_BED    true
    #define BED_DOT       isHeat
  #else
    #define STATIC_BED    false
    #define BED_DOT       false
  #endif

  #if ANIM_HOTEND && ENABLED(STATUS_HOTEND_INVERTED)
    #define OFF_BMP(N) status_hotend_24x24_##N##b_bmp
    #define ON_BMP(N)  status_hotend_24x24_##N##a_bmp
  #else
    #define OFF_BMP(N) status_hotend_24x24_##N##a_bmp
    #define ON_BMP(N)  status_hotend_24x24_##N##b_bmp
  #endif

  #if STATUS_HOTEND_BITMAPS > 1
    static const unsigned char* const status_hotend_gfx[STATUS_HOTEND_BITMAPS] PROGMEM = ARRAY_N(STATUS_HOTEND_BITMAPS, OFF_BMP(1), OFF_BMP(2), OFF_BMP(3), OFF_BMP(4), OFF_BMP(5), OFF_BMP(6));
    #if ANIM_HOTEND
      static const unsigned char* const status_hotend_on_gfx[STATUS_HOTEND_BITMAPS] PROGMEM = ARRAY_N(STATUS_HOTEND_BITMAPS, ON_BMP(1), ON_BMP(2), ON_BMP(3), ON_BMP(4), ON_BMP(5), ON_BMP(6));
      #define HOTEND_BITMAP(N,S) (unsigned char*)pgm_read_ptr((S) ? &status_hotend_on_gfx[(N) % (STATUS_HOTEND_BITMAPS)] : &status_hotend_gfx[(N) % (STATUS_HOTEND_BITMAPS)])
    #else
      #define HOTEND_BITMAP(N,S) (unsigned char*)pgm_read_ptr(&status_hotend_gfx[(N) % (STATUS_HOTEND_BITMAPS)])
    #endif
  #elif ANIM_HOTEND
    #define HOTEND_BITMAP(N,S) ((S) ? ON_BMP() : OFF_BMP())
  #else
    #define HOTEND_BITMAP(N,S) status_hotend_a_bmp
  #endif

  if (PAGE_CONTAINS(row_str1_top, row_str1_top + STATUS_HEATERS_HEIGHT - 1)) {

    #define BAR_TALL (STATUS_HEATERS_HEIGHT - 2)

    const float prop = target - 20,
                perc = prop > 0 && temp >= 20 ? (temp - 20) / prop : 0;
    uint8_t tall = uint8_t(perc * BAR_TALL + 0.5f);
    NOMORE(tall, BAR_TALL);

    #ifdef STATUS_HOTEND_ANIM
      // Draw hotend bitmap, either whole or split by the heating percent
      if (IFBED(0, 1)) {
        #if HOTENDS > 1
          const uint8_t hx = STATUS_HOTEND_X(heater), bw = STATUS_HOTEND_BYTEWIDTH(heater);
        #else
          const uint8_t hx = STATUS_HOTEND_X(heater) + STATUS_HEATERS_XSPACE, bw = STATUS_HOTEND_BYTEWIDTH(heater+1);
        #endif
        #if ENABLED(STATUS_HEAT_PERCENT)
          if (isHeat && tall <= BAR_TALL) {
            const uint8_t ph = STATUS_HEATERS_HEIGHT - 1 - tall;
            u8g.drawBitmapP(hx, STATUS_HEATERS_Y, bw, ph, HOTEND_BITMAP(heater, false));
            u8g.drawBitmapP(hx, STATUS_HEATERS_Y + ph, bw, tall + 1, HOTEND_BITMAP(heater, true) + ph * bw);
          }
          else
        #endif
            u8g.drawBitmapP(hx, row_str1_top, bw, STATUS_HEATERS_HEIGHT, HOTEND_BITMAP(heater, isHeat));
      }
    #endif

    // Draw a heating progress bar, if specified
    #if ENABLED(STATUS_HEAT_PERCENT)

      if (IFBED(true, STATIC_HOTEND) && isHeat) {
        const uint8_t bx = IFBED(STATUS_BED_X + STATUS_BED_WIDTH, STATUS_HOTEND_X(heater) + STATUS_HOTEND_WIDTH(heater)) + 1;
        u8g.drawFrame(bx, STATUS_HEATERS_Y, 3, STATUS_HEATERS_HEIGHT);
        if (tall) {
          const uint8_t ph = STATUS_HEATERS_HEIGHT - 1 - tall;
          if (PAGE_OVER(STATUS_HEATERS_Y + ph))
            u8g.drawVLine(bx + 1, STATUS_HEATERS_Y + ph, tall);
        }
      }

    #endif

  } // PAGE_CONTAINS

  if (PAGE_CONTAINS(row_str1_top, row_str1_botm)) {
    #if HEATER_IDLE_HANDLER
      const bool is_idle = IFBED(thermalManager.bed_idle.timed_out, thermalManager.hotend_idle[heater].timed_out),
                 dodraw = (blink || !is_idle);
    #else
      constexpr bool dodraw = true;
    #endif
    //if (dodraw) _draw_centered_temp(target + 0.5, tx, row_str1_base);
    if (dodraw) {
      lcd_moveto(tx, row_str1_base);
      lcd_put_u8str(i16tostr3(target + 0.5));
    } 
  }
  if (PAGE_CONTAINS(row_str2_top, row_str2_botm)) {
    // _draw_centered_temp(temp + 0.5f, tx, row_str2_base);
    lcd_moveto(tx,row_str2_base);
    lcd_put_u8str(i16tostr3(temp + 0.5));
  }
}

#if HAS_HEATED_CHAMBER

  FORCE_INLINE void _draw_chamber_status(const bool blink) {
    const float temp = thermalManager.degChamber(),
                target = thermalManager.degTargetChamber();
    #if !HEATER_IDLE_HANDLER
      UNUSED(blink);
    #endif
    if (PAGE_UNDER(7)) {
      #if HEATER_IDLE_HANDLER
        const bool is_idle = false, // thermalManager.chamber_idle.timed_out,
                   dodraw = (blink || !is_idle);
      #else
        constexpr bool dodraw = true;
      #endif
      if (dodraw) _draw_centered_temp(target + 0.5, STATUS_CHAMBER_TEXT_X, 7);
    }
    if (PAGE_CONTAINS(28 - INFO_FONT_ASCENT, 28 - 1))
      _draw_centered_temp(temp + 0.5f, STATUS_CHAMBER_TEXT_X, 28);
  }

#endif

//
// Before homing, blink '123' <-> '???'.
// Homed but unknown... '123' <-> '   '.
// Homed and known, display constantly.
//
FORCE_INLINE void _draw_axis_value(const AxisEnum axis, const char *value, const bool blink) {
  const u8g_uint_t offs = off_x * (axis + 1);
  u8g.setFont(MENU_FONT_NAME);
  lcd_moveto(X_VALUE_POS + offs, row_str2_base);
  if (blink)
    lcd_put_u8str(value);
  else {
    if (!TEST(axis_homed, axis)) {
      u8g.setColorIndex(2);
      while (const char c = *value++) lcd_put_wchar(c <= '.' ? c : '?');
      u8g.setColorIndex(1);
    }
    else {
      #if NONE(HOME_AFTER_DEACTIVATE, DISABLE_REDUCED_ACCURACY_WARNING)
        if (!TEST(axis_known_position, axis))
          lcd_put_u8str_P(axis == Z_AXIS ? PSTR("       ") : PSTR("    "));
        else
      #endif
          lcd_put_u8str(value);
    }
  }
}

#if ENABLED(MARLIN_DEV_MODE)
  uint16_t count_renders = 0;
  uint32_t total_cycles = 0;
#endif

void draw_4colom_box () {
  draw_item_box(false);
  u8g.setColorIndex(3);
  u8g.drawVLine(col_x1 + off_x   - 1, row_y1, row_h);
  u8g.drawVLine(col_x1 + off_x      , row_y1, row_h);
  u8g.drawVLine(col_x1 + off_x*2 - 1, row_y1, row_h);
  u8g.drawVLine(col_x1 + off_x*2    , row_y1, row_h);
  u8g.drawVLine(col_x1 + off_x*3 - 1, row_y1, row_h);
  u8g.drawVLine(col_x1 + off_x*3    , row_y1, row_h);
  u8g.setColorIndex(1);
}

void MarlinUI::draw_status_screen() {

  ui.screen_mode = SCRMODE_STATUS;
  ui.repeat_delay = 50;

  #if ENABLED(MARLIN_DEV_MODE)
    if (first_page) count_renders++;
  #endif

  static char xstring[5], ystring[5], zstring[8];
  #if ENABLED(FILAMENT_LCD_DISPLAY)
    static char wstring[5], mstring[4];
  #endif

  // At the first page, generate new display values
  if (first_page) {
    #if ANIM_HOTEND || ANIM_BED || ANIM_CHAMBER
      uint8_t new_bits = 0;
      #if ANIM_HOTEND
        HOTEND_LOOP() if (thermalManager.isHeatingHotend(e) ^ SHOW_ON_STATE) SBI(new_bits, e);
      #endif
      #if ANIM_BED
        if (thermalManager.isHeatingBed() ^ SHOW_ON_STATE) SBI(new_bits, 7);
      #endif
      #if ANIM_CHAMBER
        if (thermalManager.isHeatingChamber() ^ SHOW_ON_STATE) SBI(new_bits, 6);
      #endif
      heat_bits = new_bits;
    #endif
    strcpy(xstring, ftostr4sign(LOGICAL_X_POSITION(current_position[X_AXIS])));
    strcpy(ystring, ftostr4sign(LOGICAL_Y_POSITION(current_position[Y_AXIS])));
    strcpy(zstring, ftostr52sp(LOGICAL_Z_POSITION(current_position[Z_AXIS])));
  } // first_page

  const bool blink = get_blink();

  #if ENABLED(MARLIN_DEV_MODE)
    TCNT5 = 0;
  #endif

//
// First Row
//
  off_x  = LCD_PIXEL_WIDTH / 4;
  row_h  = LCD_PIXEL_HEIGHT / 4;
  row_y1 = 0; 
  row_y2 = row_y1 + row_h - 1;
  col_x1 = 0;
  col_x2 = LCD_PIXEL_WIDTH - 1;
   
  if (PAGE_CONTAINS(row_y1,row_y2)) {
    draw_4colom_box();
    row_str1_top  = row_y1 + OFFSET_Y;
    row_str1_base = row_y1 + OFFSET_Y + STATUS_FONT_ASCENT;
    row_str1_botm = row_y1 + OFFSET_Y + STATUS_FONT_ASCENT + STATUS_FONT_DESCENT;

    // tidak perlu Font Descent karena tidak ada font yg punya descent.
    row_str2_top  = row_y2 - OFFSET_Y - STATUS_FONT_ASCENT;
    row_str2_base = row_y2 - OFFSET_Y;
    row_str2_botm = row_y2 - OFFSET_Y;
  //
  // Logo
  //
    // Draw Logo
    #if STATUS_LOGO_WIDTH
      if (PAGE_CONTAINS(STATUS_LOGO_Y, STATUS_LOGO_Y + STATUS_LOGO_HEIGHT - 1))
        u8g.drawBitmapP(STATUS_LOGO_X, STATUS_LOGO_Y, STATUS_LOGO_BYTEWIDTH, STATUS_LOGO_HEIGHT, status_logo_bmp);
    #endif

    if (PAGE_CONTAINS(row_str1_top, row_str1_botm)) {
      u8g.setFont(STATUS_FONT_NAME);
      lcd_put_u8str((COL_WIDTH - STATUS_FONT_WIDTH*5)/2 - 1, row_str1_base, "REXYZ");
      lcd_moveto(X_LABEL_POS + off_x, row_str1_base);
      lcd_put_wchar('X');
      lcd_moveto(X_LABEL_POS + off_x*2, row_str1_base);
      lcd_put_wchar('Y');
      lcd_moveto(X_LABEL_POS + off_x*3, row_str1_base);
      lcd_put_wchar('Z');
    }
    if (PAGE_CONTAINS(row_str2_top, row_str2_botm)) {
      u8g.setFont(STATUS_FONT_NAME);
      lcd_put_u8str((COL_WIDTH - STATUS_FONT_WIDTH*2)/2 - 1, row_str2_base, REXYZ_MACHINE_FRAME_TYPE);
      _draw_axis_value(X_AXIS, xstring, blink);
      _draw_axis_value(Y_AXIS, ystring, blink);
      _draw_axis_value(Z_AXIS, zstring, blink);
    }
  }

  #if ENABLED(MARLIN_DEV_MODE)
    total_cycles += TCNT5;
  #endif

  //
  // Second Row
  //
  row_h  = LCD_PIXEL_HEIGHT / 4;
  row_y1 = row_h;
  row_y2 = row_y1 + row_h - 1;
  col_x1 = 0;
  col_x2 = LCD_PIXEL_WIDTH - 1;
  off_x  = LCD_PIXEL_WIDTH / 4;
  
  if (PAGE_CONTAINS(row_y1,row_y2)) {
    draw_4colom_box();
    row_str1_top  = row_y1 + OFFSET_Y;
    row_str1_base = row_y1 + OFFSET_Y + STATUS_FONT_ASCENT;
    row_str1_botm = row_y1 + OFFSET_Y + STATUS_FONT_ASCENT + STATUS_FONT_DESCENT;

    row_str2_top  = row_y2 - OFFSET_Y - STATUS_FONT_ASCENT;
    row_str2_base = row_y2 - OFFSET_Y;
    row_str2_botm = row_y2 - OFFSET_Y;

    #if STATUS_HEATERS_WIDTH
      // Draw all heaters (and maybe the bed) in one go
      if (PAGE_CONTAINS(STATUS_HEATERS_Y, STATUS_HEATERS_Y + STATUS_HEATERS_HEIGHT - 1))
        u8g.drawBitmapP(STATUS_HEATERS_X, STATUS_HEATERS_Y, STATUS_HEATERS_BYTEWIDTH, STATUS_HEATERS_HEIGHT, status_heaters_bmp);
    #endif

    #if DO_DRAW_BED
      #if ANIM_BED
        #define BED_BITMAP(S) ((S) ? status_bed_40x24_on_bmp : status_bed_40x24_bmp)
      #else
        #define BED_BITMAP(S) status_bed_bmp
      #endif
      const uint8_t bedy = row_str2_botm - STATUS_BED_HEIGHT(BED_ALT()), bedh = STATUS_BED_HEIGHT(BED_ALT());
      if (PAGE_CONTAINS(bedy, bedy + bedh - 1))
        u8g.drawBitmapP(STATUS_BED_X, bedy, STATUS_BED_BYTEWIDTH, bedh, BED_BITMAP(BED_ALT()));
    #endif

    #if DO_DRAW_CHAMBER
      #if ANIM_CHAMBER
        #define CHAMBER_BITMAP(S) ((S) ? status_chamber_on_bmp : status_chamber_bmp)
      #else
        #define CHAMBER_BITMAP(S) status_chamber_bmp
      #endif
      if (PAGE_CONTAINS(STATUS_CHAMBER_Y, STATUS_CHAMBER_Y + STATUS_CHAMBER_HEIGHT - 1))
        u8g.drawBitmapP(
          STATUS_CHAMBER_X, STATUS_CHAMBER_Y,
          STATUS_CHAMBER_BYTEWIDTH, STATUS_CHAMBER_HEIGHT,
          CHAMBER_BITMAP(CHAMBER_ALT())
        );
    #endif

    #if DO_DRAW_FAN
      #if STATUS_FAN_FRAMES > 2
        static bool old_blink;
        static uint8_t fan_frame;
        if (old_blink != blink) {
          old_blink = blink;
          if (!thermalManager.fan_speed[0] || ++fan_frame >= STATUS_FAN_FRAMES) fan_frame = 0;
        }
      #endif
      if (PAGE_CONTAINS(row_y1 + (row_h - STATUS_FAN_HEIGHT)/2, row_y1 + (row_h - STATUS_FAN_HEIGHT)/2 + STATUS_FAN_HEIGHT))
        u8g.drawBitmapP(
          STATUS_FAN_X, row_y1 + (row_h - STATUS_FAN_HEIGHT)/2,
          STATUS_FAN_BYTEWIDTH, STATUS_FAN_HEIGHT,
          #if STATUS_FAN_FRAMES > 2
            fan_frame == 1 ? status_fan_40x36_1_bmp :
            fan_frame == 2 ? status_fan_40x36_2_bmp :
            #if STATUS_FAN_FRAMES > 3
              fan_frame == 3 ? status_fan_40x36_3_bmp :
            #endif
          #elif STATUS_FAN_FRAMES > 1
            blink && thermalManager.fan_speed[0] ? status_fan_40x36_1_bmp :
          #endif
          status_fan_40x36_0_bmp
        );
    #endif
  //
  // Feedrate
  //
    u8g.setFont(MENU_FONT_NAME);

    if (PAGE_CONTAINS(row_str1_top, row_str1_botm)) {
      draw_item_box(false); 
      lcd_moveto((COL_WIDTH-STATUS_FONT_WIDTH*5)/2 - 1, row_str1_base);
      lcd_put_u8str("Speed");
    }

    if (PAGE_CONTAINS(row_str2_top, row_str2_botm)) {
      lcd_moveto((COL_WIDTH-MENU_FONT_WIDTH*4)/2, row_str2_base);
      lcd_put_u8str(i16tostr3(feedrate_percentage));
      lcd_put_wchar('%');
    }  
  //
  // Temperature Graphics and Info
  //
    // Extruders
    for (uint8_t e = 0; e < MAX_HOTEND_DRAW; ++e)
      _draw_heater_status((heater_ind_t)e, blink);

    // Heated bed
    #if HAS_HEATED_BED && HOTENDS < 4
      _draw_heater_status(H_BED, blink);
    #endif

    #if HAS_HEATED_CHAMBER
      _draw_chamber_status(blink);
    #endif

    // Fan, if a bitmap was provided
    #if DO_DRAW_FAN
      if (PAGE_CONTAINS(row_str1_top, row_str2_botm)) {
        uint16_t spd = thermalManager.fan_speed[0];
        if (spd) {
          #if ENABLED(ADAPTIVE_FAN_SLOWING)
            if (!blink && thermalManager.fan_speed_scaler[0] < 128) {
              spd = thermalManager.scaledFanSpeed(0, spd);
              c = '*';
            }
          #endif
          u8g.setFont(MENU_FONT_NAME);
          lcd_moveto((LCD_PIXEL_WIDTH/4*3)+45, row_str1_base);
          lcd_put_u8str(i16tostr3(thermalManager.fanPercent(spd)));
        }
      }
      if (PAGE_CONTAINS(row_str2_top, row_str2_botm)) {
        char c = '%';
        uint16_t spd = thermalManager.fan_speed[0];
        if (spd) {
          u8g.setFont(MENU_FONT_NAME);
          lcd_moveto((LCD_PIXEL_WIDTH/4*3)+65, row_str2_base);
          lcd_put_wchar(c);
        }
      }
    #endif
  }

  row_y1 = LCD_PIXEL_HEIGHT * 3/6; 
  row_y2 = LCD_PIXEL_HEIGHT * 5/6 - 1;
  col_x1 = 0;
  col_x2 = LCD_PIXEL_WIDTH - 1;
  if (PAGE_CONTAINS(row_y1,row_y2)) {
    row_str1_base = row_y1+ ROW_HEIGHT + STATUS_FONT_HEIGHT/2 - STATUS_FONT_DESCENT;
  //
  // ABL_STATUS
  //
    #if HAS_ABL_OR_UBL
      if (planner.leveling_active) 
        u8g.setColorIndex(2);
      else 
        u8g.setColorIndex(3);
      #if ENABLED(AUTO_BED_LEVELING_UBL)
        lcd_put_u8str((COL_WIDTH - MENU_FONT_WIDTH*3)/2 - 1, row_str1_base, "UBL");
      #else
        lcd_put_u8str((COL_WIDTH - MENU_FONT_WIDTH*3)/2 - 1, row_str1_base, "ABL");
      #endif
      u8g.setColorIndex(1);
    #endif
  //
  // SD Card Symbol
  //
    #if ENABLED(SDSUPPORT)
      if (card.isMounted()) {
        if (card.isFileOpen()) 
          u8g.setColorIndex(2);
        else
          u8g.setColorIndex(1);
      }
      else
        u8g.setColorIndex(3);
      lcd_put_u8str(COL_WIDTH + (COL_WIDTH - MENU_FONT_WIDTH*5)/2 - 1, row_str1_base, "MEDIA"); // 160 + (80 - 12 *5) /2
      u8g.setColorIndex(1);
    #endif
  //
  // Filament Sensor
  //
    #if HAS_FILAMENT_SENSOR
      if (runout.enabled) {
        if(runout.filament_ran_out)
          u8g.setColorIndex(2);
        else
          u8g.setColorIndex(1);
      }
      else
        u8g.setColorIndex(3);
      lcd_put_u8str(COL_WIDTH*2 + (COL_WIDTH - MENU_FONT_WIDTH*3)/2- 1, row_str1_base, "FIL"); // 160 + (80 - 12 *3) /2
      u8g.setColorIndex(1);
    #endif
    #if ENABLED(POWER_LOSS_RECOVERY)
      if (recovery.enabled) 
        u8g.setColorIndex(1);
      else
        u8g.setColorIndex(3);
      lcd_put_u8str(COL_WIDTH*3 + (COL_WIDTH - MENU_FONT_WIDTH*3)/2- 1, row_str1_base, "PLR"); // 160 + (80 - 12 *3) /2
      u8g.setColorIndex(1);
    #endif
  }

  //
  // Progress bar frame
  //
  #if HAS_PRINT_PROGRESS

    //
    // Progress bar frame
    //
    #define PROGRESS_BAR_X OFFSET_X //(STATUS_FONT_WIDTH * 9)
    #define PROGRESS_BAR_Y LCD_PIXEL_HEIGHT * 5 / 6  - 10
    #define PROGRESS_BAR_WIDTH (LCD_PIXEL_WIDTH - OFFSET_X * 2)

    if (PAGE_CONTAINS(PROGRESS_BAR_Y, PROGRESS_BAR_Y + 3))
      u8g.drawFrame(PROGRESS_BAR_X, PROGRESS_BAR_Y, PROGRESS_BAR_WIDTH, 4);

    row_y1 += STATUS_FONT_HEIGHT + 5;
    row_y2 += STATUS_FONT_HEIGHT + 5;
    row_str1_base += STATUS_FONT_HEIGHT + 5;
    if (PAGE_CONTAINS(row_y1,row_y2)) {
    //
    // Elapsed Time
    //
      #if DISABLED(DOGM_SD_PERCENT)
        #define SD_DURATION_X (PROGRESS_BAR_X + (PROGRESS_BAR_WIDTH / 2) - len * (MENU_FONT_WIDTH / 2)) - OFFSET_X
      #else
        #define SD_DURATION_X (LCD_PIXEL_WIDTH - len * STATUS_FONT_WIDTH) - OFFSET_X
      #endif

      if (PAGE_CONTAINS(row_y1,row_y2)) {
        u8g.setFont(STATUS_FONT_NAME);
        char buffer[13];
        duration_t elapsed = print_job_timer.duration();
        bool has_days = (elapsed.value >= 60*60*24L);
        uint8_t len = elapsed.toDigital(buffer, has_days);
        lcd_put_u8str(SD_DURATION_X, row_str1_base, buffer);
      }
    }

    const uint8_t progress = get_progress();
    if (progress > 1) {

      //
      // Progress bar solid part
      //
      if (PAGE_CONTAINS(PROGRESS_BAR_Y+1, PROGRESS_BAR_Y+2))     // 50-51 (or just 50)
        u8g.drawBox(
          PROGRESS_BAR_X + 1, PROGRESS_BAR_Y+1,
          (uint16_t)((PROGRESS_BAR_WIDTH - 2) * progress * 0.01), 2
        );

      if (PAGE_CONTAINS(row_y1,row_y2)) {
        //
        // SD Percent Complete
        //
          #if ENABLED(DOGM_SD_PERCENT)
            if (PAGE_CONTAINS(row_y1,row_y2)) {
              // Percent complete
              lcd_moveto(PROGRESS_BAR_X, row_str1_base);
              lcd_put_u8str(ui8tostr3(progress));
              lcd_put_wchar('%');
            }
          #endif
      }
    }
  #endif // HAS_PRINT_PROGRESS
  //
  // Status line
  //
  draw_status_message(blink);

  run_status_screen_touch_command();
}

void MarlinUI::draw_status_message(const bool blink) {

  row_y1 = LCD_PIXEL_HEIGHT * 5/6; 
  row_y2 = LCD_PIXEL_HEIGHT - 1;
  col_x1 = 0;
  col_x2 = LCD_PIXEL_WIDTH - 1;
   
  u8g.setFont(MENU_FONT_NAME);
  row_str1_base = row_y1 + (row_y2-row_y1)/2 + MENU_FONT_HEIGHT/2 - MENU_FONT_DESCENT;

  if (!PAGE_CONTAINS(row_y1,row_y2)) return;
  draw_item_box(false); 


  // Get the UTF8 character count of the string
  uint8_t slen = utf8_strlen(status_message);

  #if ENABLED(STATUS_MESSAGE_SCROLLING)

    static bool last_blink = false;

    if (slen <= LCD_WIDTH-1) {
      // The string fits within the line. Print with no scrolling
      lcd_moveto((LCD_PIXEL_WIDTH-slen*MENU_FONT_WIDTH)/2-1, row_str1_base);
      lcd_put_u8str(status_message);
      while (slen < LCD_WIDTH-1) { lcd_put_wchar(' '); ++slen; }
    }
    else {
      // String is longer than the available space

      // Get a pointer to the next valid UTF8 character
      // and the string remaining length
      uint8_t rlen;
      const char *stat = status_and_len(rlen);
      lcd_moveto(OFFSET_X, row_str1_base);
      lcd_put_u8str_max(stat, LCD_PIXEL_WIDTH);

      // If the remaining string doesn't completely fill the screen
      if (rlen < LCD_WIDTH - 1) {
        lcd_put_wchar('.');                     // Always at 1+ spaces left, draw a dot
        uint8_t chars = LCD_WIDTH - rlen - 1;       // Amount of space left in characters
        if (--chars) {                          // Draw a second dot if there's space
          lcd_put_wchar('.');
          if (--chars) {                        // Print a second copy of the message
            lcd_put_u8str_max(status_message, LCD_PIXEL_WIDTH - (rlen + 2) * (STATUS_FONT_WIDTH));
            lcd_put_wchar(' ');
          }
        }
      }
      if (last_blink != blink) {
        last_blink = blink;
        advance_status_scroll();
      }
    }

  #else // !STATUS_MESSAGE_SCROLLING

    UNUSED(blink);

    // Just print the string to the LCD
    if (slen <= LCD_WIDTH-1) {
      lcd_moveto((LCD_PIXEL_WIDTH-slen*MENU_FONT_WIDTH)/2-1, row_str1_base);
    else 
      lcd_moveto(OFFSET_X, row_str1_base);
    lcd_put_u8str_max(status_message, LCD_PIXEL_WIDTH);

    // Fill the rest with spaces
    for (; slen < LCD_WIDTH; ++slen) lcd_put_wchar(' ');

  #endif // !STATUS_MESSAGE_SCROLLING
}

#endif // HAS_GRAPHICAL_LCD && !LIGHTWEIGHT_UI && !FULL_SCALE_TFT_320X240
