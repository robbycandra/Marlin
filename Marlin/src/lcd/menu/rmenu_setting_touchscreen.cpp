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
#if HAS_FULL_SCALE_TFT
  #include "../../lcd/TFT/ultralcd_TFT.h"
  #include "../../lcd/TFT/TFT_screen_defines.h"
#else
  #include "../../lcd/dogm/ultralcd_DOGM.h"
  #include "../../lcd/dogm/fontdata/fontdata_ISO10646_1.h"
#endif
#include "../../feature/touch/xpt2046.h"

#if HAS_FULL_SCALE_TFT
  #define TOUCH_TEXT_OFFSET 15
  #define TOUCH_POINT_OFFSET 50
  #define TOUCH_POINT_BOTTOM 10
  #define LINE_LENGTH 15
#else
  #define TOUCH_TEXT_OFFSET 0
  #define TOUCH_POINT_OFFSET 5
#endif

#define DISP_PT_X0 (LCD_PIXEL_WIDTH/2)
#define DISP_PT_Y0 TOUCH_POINT_OFFSET
#define DISP_PT_X1 (LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET)
#define DISP_PT_Y1 ((LCD_PIXEL_HEIGHT + TOUCH_POINT_OFFSET - TOUCH_POINT_BOTTOM)/2)
#define DISP_PT_X2 TOUCH_POINT_OFFSET
#define DISP_PT_Y2 (LCD_PIXEL_HEIGHT - TOUCH_POINT_BOTTOM)

typedef struct Point { int32_t x, y; } POINT;

static uint8_t touch_point_index;
static uint32_t touched_x[9], touched_y[9];
char pointStr[15];
bool isfail;

inline uint32_t middleOfThree(uint32_t a, uint32_t b, uint32_t c) {
    if (a > b)
      return (b > c) ? b : ((a > c) ? c : a);
    return   (a > c) ? a : ((b > c) ? c : b);
}

void _lcd_touch_cal_result() {

  ui.refresh(LCDVIEW_CALL_REDRAW_NEXT);
  u8g.setColorIndex(1);
  for(touch_point_index=0; touch_point_index<3; touch_point_index++) {
    sprintf(pointStr,"[%d:%d]",(uint16_t)touched_x[touch_point_index],(uint16_t)touched_y[touch_point_index]);
    const uint8_t slen = strlen(pointStr);
    switch(touch_point_index) {
      case 0:
        if(PAGE_CONTAINS(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET  + MENU_FONT_ASCENT)) {
          lcd_moveto((LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH)/2, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
          lcd_put_u8str(pointStr);
        }
        break;
      case 1:
        if(PAGE_CONTAINS((LCD_PIXEL_HEIGHT - MENU_FONT_ASCENT)/2, (LCD_PIXEL_HEIGHT + MENU_FONT_ASCENT)/2)) {
          lcd_moveto(LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH - TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT + MENU_FONT_ASCENT)/2);
          lcd_put_u8str(pointStr);
        }
        break;
      case 2:
        if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - MENU_FONT_ASCENT, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - 1)) {
          lcd_moveto(TOUCH_TEXT_OFFSET, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
          lcd_put_u8str(pointStr);
        }
        break;
    }
  }

  if (isfail) {
    lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_HEIGHT*0.5), "Fail, Retry");
    bool got_click = ui.use_click();
    if (got_click) {
      ui.goto_previous_screen();
    }
  }
  else {
    const int32_t divider = ((touched_x[0] - touched_x[2]) * (touched_y[1] - touched_y[2])) -
                            ((touched_x[1] - touched_x[2]) * (touched_y[0] - touched_y[2])) ;

    const int32_t x_calx = (((DISP_PT_X0) - (DISP_PT_X2)) * (touched_y[1] - touched_y[2])) -
                          (((DISP_PT_X1) - (DISP_PT_X2)) * (touched_y[0] - touched_y[2])) ;

    const int32_t x_caly = (((DISP_PT_X1) - (DISP_PT_X2)) * (touched_x[0] - touched_x[2])) -
                          (((DISP_PT_X0) - (DISP_PT_X2)) * (touched_x[1] - touched_x[2])) ;

    const int32_t x_offs = (touched_x[1] * touched_y[2] - touched_x[2] * touched_y[1]) * (DISP_PT_X0) -
                          (touched_x[0] * touched_y[2] - touched_x[2] * touched_y[0]) * (DISP_PT_X1) +
                          (touched_x[0] * touched_y[1] - touched_x[1] * touched_y[0]) * (DISP_PT_X2) ;

    const int32_t y_calx = (((DISP_PT_Y0) - (DISP_PT_Y2)) * (touched_y[1] - touched_y[2])) -
                          (((DISP_PT_Y1) - (DISP_PT_Y2)) * (touched_y[0] - touched_y[2])) ;

    const int32_t y_caly = (((DISP_PT_Y1) - (DISP_PT_Y2)) * (touched_x[0] - touched_x[2])) -
                          (((DISP_PT_Y0) - (DISP_PT_Y2)) * (touched_x[1] - touched_x[2])) ;

    const int32_t y_offs = (touched_x[1] * touched_y[2] - touched_x[2] * touched_y[1]) * (DISP_PT_Y0) -
                          (touched_x[0] * touched_y[2] - touched_x[2] * touched_y[0]) * (DISP_PT_Y1) +
                          (touched_x[0] * touched_y[1] - touched_x[1] * touched_y[0]) * (DISP_PT_Y2) ;

    #if HAS_FULL_SCALE_TFT
      sprintf(pointStr,"X-CalX:%d", (int16_t)round((float)x_calx*65536/divider));
      lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_HEIGHT *2), pointStr);
      sprintf(pointStr,"X-CalY:%d", (int16_t)round((float)x_caly*65536/divider));
      lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_HEIGHT*1), pointStr);
      sprintf(pointStr,"X-Offs:%d", (int16_t)round(x_offs/divider));
      lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_HEIGHT*0), pointStr);
      sprintf(pointStr,"Y-CalX:%d", (int16_t)round((float)y_calx*65536/divider));
      lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_HEIGHT*1), pointStr);
      sprintf(pointStr,"Y-CalY:%d", (int16_t)round((float)y_caly*65536/divider));
      lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_HEIGHT*2), pointStr);
      sprintf(pointStr,"Y-Offs:%d", (int16_t)round(y_offs/divider));
      lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_HEIGHT*3), pointStr);
    #endif

    bool got_click = ui.use_click();
    if (got_click) {
      touch.tscalibration[0] = (int16_t)round((float)x_calx*65536/divider);
      touch.tscalibration[1] = (int16_t)round((float)x_caly*65536/divider);
      touch.tscalibration[2] = (int16_t)round(x_offs/divider);
      touch.tscalibration[3] = (int16_t)round((float)y_calx*65536/divider);
      touch.tscalibration[4] = (int16_t)round((float)y_caly*65536/divider);
      touch.tscalibration[5] = (int16_t)round(y_offs/divider);
      ui.goto_previous_screen();
    }
  }
}

void _lcd_touch_point_screen() {
  ui.refresh(LCDVIEW_CALL_REDRAW_NEXT);
  u8g.setColorIndex(1);
  switch(touch_point_index % 3) {
    case 0:
      if(PAGE_CONTAINS(DISP_PT_Y0 - LINE_LENGTH, DISP_PT_Y0 + LINE_LENGTH-1)) {
        u8g.drawHLine(DISP_PT_X0 - LINE_LENGTH, DISP_PT_Y0, LINE_LENGTH*2);
        u8g.drawVLine(DISP_PT_X0, DISP_PT_Y0 - LINE_LENGTH, LINE_LENGTH*2);
      }
      break;
    case 1:
      if(PAGE_CONTAINS(DISP_PT_Y1 - LINE_LENGTH, DISP_PT_Y1 + LINE_LENGTH-1)) {
        u8g.drawHLine(DISP_PT_X1 - LINE_LENGTH, DISP_PT_Y1, LINE_LENGTH*2);
        u8g.drawVLine(DISP_PT_X1, DISP_PT_Y1 - LINE_LENGTH, LINE_LENGTH*2);
      }
      break;
    case 2:
      if(PAGE_CONTAINS(DISP_PT_Y2 - LINE_LENGTH, DISP_PT_Y2 + LINE_LENGTH-1)) {
        u8g.drawHLine(DISP_PT_X2 - LINE_LENGTH, DISP_PT_Y2, LINE_LENGTH*2);
        u8g.drawVLine(DISP_PT_X2, DISP_PT_Y2 - LINE_LENGTH, LINE_LENGTH*2);
      }
      break;
  }
  #if HAS_FULL_SCALE_TFT
    u8g.setFont(MENU_FONT_NAME);
    if (touch_point_index > 0) {
      int8_t idx = touch_point_index - 1;
      sprintf(pointStr,"[%d:%d]",(uint16_t)touched_x[idx],(uint16_t)touched_y[idx]);
      const uint8_t slen = strlen(pointStr);
      switch(idx % 3) {
        case 0:
          if(PAGE_CONTAINS(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET  + MENU_FONT_ASCENT)) {
            lcd_moveto((LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH)/2, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
            lcd_put_u8str(pointStr);
          }
          break;
        case 1:
          if(PAGE_CONTAINS((LCD_PIXEL_HEIGHT - MENU_FONT_ASCENT)/2, (LCD_PIXEL_HEIGHT + MENU_FONT_ASCENT)/2)) {
            lcd_moveto(LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH - TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT + MENU_FONT_ASCENT)/2);
            lcd_put_u8str(pointStr);
          }
          break;
        case 2:
          if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - MENU_FONT_ASCENT, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - 1)) {
            lcd_moveto(TOUCH_TEXT_OFFSET, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
            lcd_put_u8str(pointStr);
          }
          break;
      }
    }
    if(PAGE_CONTAINS((LCD_PIXEL_HEIGHT - MENU_FONT_ASCENT)/2, (LCD_PIXEL_HEIGHT + MENU_FONT_ASCENT)/2)) {
      sprintf(pointStr,"Test %d of 9]",touch_point_index+1);
      lcd_moveto(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT + MENU_FONT_ASCENT)/2);
      lcd_put_u8str(pointStr);
    }
  #endif
  if (ui.lcd_menu_touched_coord & 0xF0) {
    #if HAS_BUZZER
      ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
    #endif
    touched_x[touch_point_index] = touch.raw_x;
    touched_y[touch_point_index] = touch.raw_y;
    touch_point_index++;
    ui.lcd_menu_touched_coord = 0;
    ui.wait_for_untouched = true;
    ui.first_touch = false;
  }
  if (touch_point_index >= 9) {

    touched_x[0] = middleOfThree(touched_x[0], touched_x[3], touched_x[6]);
    touched_x[1] = middleOfThree(touched_x[1], touched_x[4], touched_x[7]);
    touched_x[2] = middleOfThree(touched_x[2], touched_x[5], touched_x[8]);
    touched_y[0] = middleOfThree(touched_y[0], touched_y[3], touched_y[6]);
    touched_y[1] = middleOfThree(touched_y[1], touched_y[4], touched_y[7]);
    touched_y[2] = middleOfThree(touched_y[2], touched_y[5], touched_y[8]);

    isfail = false;
    const int16_t delta_x = ((int32_t(touched_x[1] + touched_x[2]) - int32_t(touched_x[0]<<1))<<7) / (int32_t(touched_x[1]) - int32_t(touched_x[2])),
                  delta_y = ((int32_t(touched_y[2] + touched_y[0]) - int32_t(touched_y[1]<<1))<<7) / (int32_t(touched_y[2]) - int32_t(touched_y[0]));

    #if ENABLED(DEBUG_TOUCH_CALIBRATION)
      SERIAL_ECHOLNPAIR("Delta X = ", delta_x);
      SERIAL_ECHOLNPAIR("Delta Y = ", delta_y);
    #endif

    if (abs(delta_x) > 10) isfail = true;
    if (abs(delta_y) > 15) isfail = true;

    ui.goto_screen(_lcd_touch_cal_result, SCRMODE_STATIC);
  }
}

void menu_touch_calibration() {
  touch_point_index = 0;
  isfail = false;
  for (int i=0; i<9; i++) touched_x[i] = touched_y[i] = 0;
  ui.lcd_menu_touched_coord = 0;
  ui.wait_for_untouched = true;
  ui.first_touch = false;
  ui.defer_status_screen();
  ui.goto_previous_screen();
  ui.goto_screen(_lcd_touch_point_screen, SCRMODE_STATIC_BACK);
}

void menu_touch_calibration_confirm() {
  do_select_screen(
    PSTR("Calibrate"), GET_TEXT(MSG_BUTTON_CANCEL),
    []{
      menu_touch_calibration();
      #if HAS_BUZZER
        ui.completion_feedback(true);
      #endif
    },
    ui.goto_previous_screen,
    PSTR("Warning, Perhatian"),
    PSTR("Kalibrasi Touchscreen dengan serampangan berpotensi membuat menu tidak bisa terakses. Yang mengakibatkan printer tidak bisa dioperasikan"),
    PSTR("!")
  );
}

void _lcd_touch_test_screen() {
  ui.refresh(LCDVIEW_CALL_REDRAW_NEXT);
  u8g.setColorIndex(1);
  for (int i = 0; i < 3; i++) {
    if (touched_x[i] > 0 and touched_y[i] > 0) {
      u8g.drawHLine(touched_x[i] - 10, touched_y[i], 20);
      u8g.drawVLine(touched_x[i], touched_y[i] - 10, 20);
    }
  }
  if (ui.lcd_menu_touched_coord & 0xF0) {
    #if HAS_BUZZER
      ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
    #endif
    touched_x[touch_point_index] = touch.pixel_x;
    touched_y[touch_point_index] = touch.pixel_y;
    touch_point_index++;
    ui.lcd_menu_touched_coord = 0;
    ui.wait_for_untouched = true;
    ui.first_touch = false;
  }
  if (touch_point_index >= 3) {
    touch_point_index = 0;
  }
}

void menu_touch_testing() {
  touch_point_index = 0;
  for (int i=0; i<9; i++) touched_x[i] = touched_y[i] = 0;
  ui.lcd_menu_touched_coord = 0;
  ui.wait_for_untouched = true;
  ui.first_touch = false;
  ui.defer_status_screen();
  ui.goto_screen(_lcd_touch_test_screen, SCRMODE_STATIC_BACK);
}

void rmenu_setting_touchscreen() {
  if (rexyz_menu_mode == MENUMODE_BASIC) {
    menu_touch_testing();
  }
  else {
    START_MENU();
    STATIC_ITEM_P(PSTR("TouchScreen"), SS_CENTER | SS_INVERT);
    SUBSELECT_P(PSTR("Calibration"), menu_touch_calibration_confirm);
    ACTION_ITEM_P(PSTR("Testing"), menu_touch_testing);
    END_MENU();
  }
}

#endif // HAS_LCD_MENU && ENABLED(TOUCH_CALIBRATION)
