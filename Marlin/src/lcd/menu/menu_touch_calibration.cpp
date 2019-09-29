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
  #define TOUCH_TEXT_OFFSET 20
  #define TOUCH_POINT_OFFSET 30
#else
  #define TOUCH_TEXT_OFFSET 0
  #define TOUCH_POINT_OFFSET 5
#endif

#define DISP_PT_X0 TOUCH_POINT_OFFSET
#define DISP_PT_Y0 TOUCH_POINT_OFFSET
#define DISP_PT_X1 (LCD_PIXEL_WIDTH-TOUCH_POINT_OFFSET)
#define DISP_PT_Y1 (LCD_PIXEL_HEIGHT/2)
#define DISP_PT_X2 (LCD_PIXEL_WIDTH/2)
#define DISP_PT_Y2 (LCD_PIXEL_HEIGHT-TOUCH_POINT_OFFSET)

typedef struct Point { int32_t x, y; } POINT;

static uint8_t touch_point_index;
static uint32_t touched_x[3], touched_y[3];
char pointStr[15];

void _lcd_touch_cal_result() {
  ui.refresh(LCDVIEW_CALL_REDRAW_NEXT);
  u8g.setColorIndex(1);
  for(touch_point_index=0; touch_point_index<4; touch_point_index++) {
    sprintf(pointStr,"[%d:%d]",touched_x[touch_point_index],touched_y[touch_point_index]);
    const uint8_t slen = strlen(pointStr);
    switch(touch_point_index) {
      case 0:
        if(PAGE_CONTAINS(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET  + MENU_FONT_ASCENT)) {
          lcd_moveto(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
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
          lcd_moveto((LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH)/2, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
          lcd_put_u8str(pointStr);
        }
        break;
    }
  }

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
    sprintf(pointStr,"X-CalX:%d", (int16_t)round((float)x_calx*65535/divider));
    lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_ASCENT*2.5), pointStr);
    sprintf(pointStr,"X-CalY:%d", (int16_t)round((float)x_caly*65535/divider));
    lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_ASCENT*1.5), pointStr);
    sprintf(pointStr,"X-Offs:%d", (int16_t)round(x_offs/divider));
    lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) - (MENU_FONT_ASCENT*0.5), pointStr);
    sprintf(pointStr,"Y-CalX:%d", (int16_t)round((float)y_calx*65535/divider));
    lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_ASCENT*0.5), pointStr);
    sprintf(pointStr,"Y-CalY:%d", (int16_t)round((float)y_caly*65535/divider));
    lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_ASCENT*1.5), pointStr);
    sprintf(pointStr,"Y-Offs:%d", (int16_t)round(y_offs/divider));
    lcd_put_u8str(TOUCH_TEXT_OFFSET, (LCD_PIXEL_HEIGHT/2) + (MENU_FONT_ASCENT*2.5), pointStr);
  #endif  

  bool got_click = ui.use_click();
  if (got_click) {
    touch.tscalibration[0] = (int16_t)round((float)x_calx*65535/divider);
    touch.tscalibration[1] = (int16_t)round((float)x_caly*65535/divider);
    touch.tscalibration[2] = (int16_t)round(x_offs/divider);
    touch.tscalibration[3] = (int16_t)round((float)y_calx*65535/divider);
    touch.tscalibration[4] = (int16_t)round((float)y_caly*65535/divider);
    touch.tscalibration[5] = (int16_t)round(y_offs/divider);
    ui.goto_previous_screen();
  }
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
      if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT/2 - TOUCH_POINT_OFFSET, LCD_PIXEL_HEIGHT/2 + TOUCH_POINT_OFFSET - 1)) {
        u8g.drawHLine(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET*2, LCD_PIXEL_HEIGHT/2, TOUCH_POINT_OFFSET*2);
        u8g.drawVLine(LCD_PIXEL_WIDTH - TOUCH_POINT_OFFSET, LCD_PIXEL_HEIGHT/2 - TOUCH_POINT_OFFSET, TOUCH_POINT_OFFSET*2);
      }
      break;
    case 2:
      if(PAGE_CONTAINS(LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2, LCD_PIXEL_HEIGHT - 1)) {
        u8g.drawHLine(LCD_PIXEL_WIDTH/2 - TOUCH_POINT_OFFSET, LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET, TOUCH_POINT_OFFSET*2);
        u8g.drawVLine(LCD_PIXEL_WIDTH/2, LCD_PIXEL_HEIGHT - TOUCH_POINT_OFFSET*2, TOUCH_POINT_OFFSET*2);
      }
      break;
  }
  #if HAS_FULL_SCALE_TFT 
    u8g.setFont(MENU_FONT_NAME);
    for(uint8_t idx=0; idx < touch_point_index; idx++) {
      sprintf(pointStr,"[%d:%d]",touched_x[idx],touched_y[idx]);
      const uint8_t slen = strlen(pointStr);
      switch(idx) {
        case 0:
          if(PAGE_CONTAINS(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET  + MENU_FONT_ASCENT)) {
            lcd_moveto(TOUCH_TEXT_OFFSET, TOUCH_TEXT_OFFSET + MENU_FONT_ASCENT);
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
            lcd_moveto((LCD_PIXEL_WIDTH - slen*MENU_FONT_WIDTH)/2, LCD_PIXEL_HEIGHT - TOUCH_TEXT_OFFSET);
            lcd_put_u8str(pointStr);
          }
          break;
      }
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
  if (touch_point_index >= 3) {
    ui.goto_screen(_lcd_touch_cal_result, SCRMODE_CALIBRATION);
    //ui.goto_previous_screen();
  }
}

void menu_touch_calibration() {
  ui.screenMode = SCRMODE_CALIBRATION;
  touch_point_index = 0;
  ui.lcd_menu_touched_coord = 0;     
  ui.wait_for_untouched = true;
  ui.first_touch = false;
  ui.save_previous_screen();
  ui.defer_status_screen();
  ui.goto_screen(_lcd_touch_point_screen, SCRMODE_CALIBRATION);
}

#endif // HAS_LCD_MENU
