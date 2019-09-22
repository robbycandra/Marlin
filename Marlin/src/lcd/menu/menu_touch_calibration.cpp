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
  #define TOUCH_TEXT_OFFSET 10
  #define TOUCH_POINT_OFFSET 20
#else
  #define TOUCH_TEXT_OFFSET 0
  #define TOUCH_POINT_OFFSET 5
#endif

static uint8_t touch_point_index;
static uint16_t cal_x[4], cal_y[4];
char pointStr[15];

void _lcd_touch_cal_result() {
  ui.refresh(LCDVIEW_CALL_REDRAW_NEXT);
  u8g.setColorIndex(1);
  for(touch_point_index=0; touch_point_index<4; touch_point_index++) {
    sprintf(pointStr,"[%d:%d]",cal_x[touch_point_index],cal_y[touch_point_index]);
    const uint8_t slen = strlen(pointStr);
    switch(touch_point_index) {
      case 0:
        lcd_moveto(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
        break;
      case 1:
        lcd_moveto(LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH - TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
        break;
      case 2:
        lcd_moveto(LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH - TOUCH_TEXT_OFFSET, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
        break;
      case 3:
        lcd_moveto(10, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
        break;
    }
    lcd_put_u8str(pointStr);
  }

  const uint16_t x1 = (cal_x[0] + cal_x[3])/2;
  const uint16_t x2 = (cal_x[1] + cal_x[2])/2;
  const uint16_t y2 = (cal_y[2] + cal_y[3])/2;
  const uint16_t y1 = (cal_y[0] + cal_y[1])/2;
  const uint16_t dx = abs(x2 - x1);
  const uint16_t dy = abs(y2 - y1);

  bool isFail = false;
  if (abs(x1 - cal_x[0]) * 5 > dx ) isFail = true;
  if (abs(x2 - cal_x[1]) * 5 > dx ) isFail = true;
  if (abs(y1 - cal_y[0]) * 5 > dy ) isFail = true;
  if (abs(y2 - cal_y[2]) * 5 > dy ) isFail = true;

  if(isFail) {
    lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 19 )/2, (LCD_PIXEL_HEIGHT+MENU_FONT_ASCENT)/2,"Fail, Please Retry!");
  } else {
    #if HAS_FULL_SCALE_TFT
      int16_t x_cal = (((int32_t)(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET*2)) << 16) / (x2 - x1);
      int16_t x_off = TOUCH_POINT_OFFSET - x1 * (LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET*2) / (x2 - x1); 
      int16_t y_cal = (((int32_t)(LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2)) << 16) / (y2 - y1);
      int16_t y_off = TOUCH_POINT_OFFSET - y1 * (LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2) / (y2 - y1); 
    #else
      int16_t x_cal = (((int32_t)(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET*2)) << 17) / (x2 - x1);
      int16_t x_off = 32 + TOUCH_POINT_OFFSET * 2 - x1 * (LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET*2)*2 / (x2 - x1); 
      int16_t y_cal = (((int32_t)(LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2)) << 17) / (y2 - y1);
      int16_t y_off = 32 + TOUCH_POINT_OFFSET * 2 - y1 * (LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2)*2 / (y2 - y1); 
    #endif  

    #if HAS_FULL_SCALE_TFT
      sprintf(pointStr,"X-Cal:%d", x_cal);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_HEIGHT), pointStr);
      sprintf(pointStr,"X-Off:%d", x_off);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2), pointStr);
      sprintf(pointStr,"Y-Cal:%d", y_cal);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_HEIGHT), pointStr);
      sprintf(pointStr,"Y-Off:%d", y_off);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_HEIGHT) * 2, pointStr);
    #else
      sprintf(pointStr,"X-Cal:%d", x_cal);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_ASCENT), pointStr);
      sprintf(pointStr,"X-Off:%d", x_off);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2), pointStr);
      sprintf(pointStr,"Y-Cal:%d", y_cal);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_ASCENT), pointStr);
      sprintf(pointStr,"Y-Off:%d", y_off);
      lcd_put_u8str((LCD_PIXEL_WIDTH - MENU_FONT_WIDTH * 11 )/2, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_ASCENT) * 2, pointStr);
    #endif  

    bool got_click = ui.use_click();
    if (got_click) {
      touch.tscalibration[0] = x_cal;
      touch.tscalibration[1] = x_off;
      touch.tscalibration[2] = y_cal;
      touch.tscalibration[3] = y_off;
      ui.goto_previous_screen();
    }
  }

  /*
  if (ui.lcd_menu_touched_coord & B10000000) {
    #if HAS_BUZZER
      ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
    #endif
    ui.lcd_menu_touched_coord = 0;     
    ui.wait_for_untouched = true;
    ui.first_touch = false;
    ui.goto_previous_screen();
  }
  */
}

void _lcd_touch_point_screen() {
  ui.refresh(LCDVIEW_CALL_REDRAW_NEXT);
  //u8g.setColorIndex(0);
  //u8g.drawBox(0,0,LCD_PIXEL_WIDTH,8);
  u8g.setColorIndex(1);
  switch(touch_point_index) {
    case 0:
      if(PAGE_CONTAINS(0,TOUCH_POINT_OFFSET*2-1)) {
        u8g.drawHLine(0, TOUCH_POINT_OFFSET, TOUCH_POINT_OFFSET*2);
        u8g.drawVLine(TOUCH_POINT_OFFSET, 0, TOUCH_POINT_OFFSET*2);
      }
      break;
    case 1:
      if(PAGE_CONTAINS(0,TOUCH_POINT_OFFSET*2-1)) {
        u8g.drawHLine(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET*2, TOUCH_POINT_OFFSET, TOUCH_POINT_OFFSET*2);
        u8g.drawVLine(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET, 0, TOUCH_POINT_OFFSET*2);
      }
      break;
    case 2:
      if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2, LCD_PIXEL_HEIGHT - 1)) {
        u8g.drawHLine(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET*2, LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET, TOUCH_POINT_OFFSET*2);
        u8g.drawVLine(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET, LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2, TOUCH_POINT_OFFSET*2);
      }
      break;
    case 3:
      if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2, LCD_PIXEL_HEIGHT - 1)) {
        u8g.drawHLine(0, LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET, TOUCH_POINT_OFFSET*2);
        u8g.drawVLine(TOUCH_POINT_OFFSET,  LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2, TOUCH_POINT_OFFSET*2);
      }
      break;
  }
  #if HAS_FULL_SCALE_TFT 
    u8g.setFont(MENU_FONT_NAME);
    for(uint8_t idx=0; idx < touch_point_index; idx++) {
      sprintf(pointStr,"[%d:%d]",cal_x[idx],cal_y[idx]);
      const uint8_t slen = strlen(pointStr);
      switch(idx) {
        case 0:
          if(PAGE_CONTAINS(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET  + MENU_FONT_ASCENT)) {
            lcd_moveto(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
            lcd_put_u8str(pointStr);
          }
          break;
        case 1:
          if(PAGE_CONTAINS(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET  + MENU_FONT_ASCENT)) {
            lcd_moveto(LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH - TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
            lcd_put_u8str(pointStr);
          }
          break;
        case 2:
          if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - MENU_FONT_ASCENT, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - 1)) {
            lcd_moveto(LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH - TOUCH_TEXT_OFFSET, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
            lcd_put_u8str(pointStr);
          }
          break;
        case 3:
          if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - MENU_FONT_ASCENT, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET - 1)) {
            lcd_moveto(10, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
            lcd_put_u8str(pointStr);
          }
          break;
      }
    }
  #endif  
  if (ui.lcd_menu_touched_coord & B10000000) {
    #if HAS_BUZZER
      ui.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
    #endif
    cal_x[touch_point_index] = touch.raw_x;
    cal_y[touch_point_index] = touch.raw_y;
    touch_point_index++;
    ui.lcd_menu_touched_coord = 0;     
    ui.wait_for_untouched = true;
    ui.first_touch = false;
  }
  if (touch_point_index >= 4) {
    ui.goto_screen(_lcd_touch_cal_result, SCRMODE_CALIBRATION);
    //ui.goto_previous_screen();
  }
}

void menu_touch_calibration() {
  ui.screenMode = SCRMODE_MENU_2X4; 
  touch_point_index = 0;
  ui.lcd_menu_touched_coord = 0;     
  ui.wait_for_untouched = true;
  ui.first_touch = false;
  ui.save_previous_screen();
  ui.defer_status_screen();
  ui.goto_screen(_lcd_touch_point_screen, SCRMODE_CALIBRATION);
}

#endif // HAS_LCD_MENU
