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

/*

  u8g_dev_tft_fullScale_touch.cpp

  Universal 8bit Graphics Library

  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "../../inc/MarlinConfig.h"

#if HAS_GRAPHICAL_LCD && HAS_FULL_SCALE_TFT
#include "../ultralcd.h"
#include "../../module/temperature.h"

#include <U8glib.h>
#include "../dogm/HAL_LCD_com_defines.h"

#include <string.h>

#if ENABLED(FULL_SCALE_TFT_480X320)
  #include "TFT_screen_480x320.h"
#endif
#if ENABLED(FULL_SCALE_TFT_320X240)
  #include "TFT_screen_320x240.h"
#endif
#define PAGE_HEIGHT 8

#if ENABLED(LCD_USE_DMA_FSMC)
  extern void LCD_IO_WriteSequence(uint16_t *data, uint16_t length);
  extern void LCD_IO_WriteSequence_Async(uint16_t *data, uint16_t length);
  extern void LCD_IO_WaitSequence_Async();
  extern void LCD_IO_WriteMultiple(uint16_t color, uint32_t count);
#endif

#define WIDTH  LCD_PIXEL_WIDTH
#define HEIGHT LCD_PIXEL_HEIGHT

#define X_LO 0
#define Y_LO 0
#define X_HI (X_LO + WIDTH  - 1)
#define Y_HI (Y_LO + HEIGHT - 1)

// see https://ee-programming-notepad.blogspot.com/2016/10/16-bit-color-generator-picker.html

#ifndef TFT_MARLINUI_COLOR
  #define TFT_MARLINUI_COLOR COLOR_BLACK
#endif
#ifndef TFT_MARLINBG_COLOR
  #define TFT_MARLINBG_COLOR COLOR_WHITE
#endif
#ifndef TFT_SELECTED_COLOR
  #define TFT_SELECTED_COLOR COLOR_RED
#endif
#ifndef TFT_DISABLED_COLOR
  #define TFT_DISABLED_COLOR COLOR_BLUE
#endif
#ifndef TFT_BUTTON_COLOR
  #define TFT_BUTTON_COLOR COLOR_ORANGE
#endif

static uint32_t lcd_id = 0;
static uint8_t fan_frame = 0;

static const uint8_t page_first_sequence[] = {
  U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), U8G_ESC_DATA(X_LO), U8G_ESC_DATA(X_HI),
  U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), U8G_ESC_DATA(Y_LO), U8G_ESC_DATA(Y_HI),
  U8G_ESC_ADR(0), LCD_WRITE_RAM, U8G_ESC_ADR(1),
  U8G_ESC_END
};

// 480 = 0x1DF
// 320 = 0x13F
// 240 = 0x0EF
static const uint8_t clear_screen_sequence[] = {
  U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), 0x00, 0x00, U8G_ESC_DATA(LCD_FULL_PIXEL_WIDTH),
  U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), 0x00, 0x00, U8G_ESC_DATA(LCD_FULL_PIXEL_HEIGHT),
  U8G_ESC_ADR(0), LCD_WRITE_RAM, U8G_ESC_ADR(1),
  U8G_ESC_END
};

static const uint8_t st7789v_init_sequence[] = { // 0x8552 - ST7789V
  U8G_ESC_ADR(0),
  0x10,
  U8G_ESC_DLY(10),
  0x01,
  U8G_ESC_DLY(100), U8G_ESC_DLY(100),
  0x11,
  U8G_ESC_DLY(120),
  0x36, U8G_ESC_ADR(1), 0xA0,
  U8G_ESC_ADR(0), 0x3A, U8G_ESC_ADR(1), 0x05,
  U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), 0x00, 0x00, 0x01, 0x3F,
  U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), 0x00, 0x00, 0x00, 0xEF,
  U8G_ESC_ADR(0), 0xB2, U8G_ESC_ADR(1), 0x0C, 0x0C, 0x00, 0x33, 0x33,
  U8G_ESC_ADR(0), 0xB7, U8G_ESC_ADR(1), 0x35,
  U8G_ESC_ADR(0), 0xBB, U8G_ESC_ADR(1), 0x1F,
  U8G_ESC_ADR(0), 0xC0, U8G_ESC_ADR(1), 0x2C,
  U8G_ESC_ADR(0), 0xC2, U8G_ESC_ADR(1), 0x01, 0xC3,
  U8G_ESC_ADR(0), 0xC4, U8G_ESC_ADR(1), 0x20,
  U8G_ESC_ADR(0), 0xC6, U8G_ESC_ADR(1), 0x0F,
  U8G_ESC_ADR(0), 0xD0, U8G_ESC_ADR(1), 0xA4, 0xA1,
  U8G_ESC_ADR(0), 0xE0, U8G_ESC_ADR(1), 0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39, 0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D,
  U8G_ESC_ADR(0), 0xE1, U8G_ESC_ADR(1), 0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39, 0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31,
  U8G_ESC_ADR(0), 0x29, 0x11, 0x35, U8G_ESC_ADR(1), 0x00,
  U8G_ESC_END
};

static const uint8_t ili9341_init_sequence[] = { // 0x9341 - ILI9341
  U8G_ESC_ADR(0),
  0x10,
  U8G_ESC_DLY(10),
  0x01,                                                              // ? Di jalankan di Reset
  U8G_ESC_DLY(100), U8G_ESC_DLY(100),
  0x36, U8G_ESC_ADR(1), 0xE8,                                         // Set Rotation : 0xE8
  U8G_ESC_ADR(0), 0x3A, U8G_ESC_ADR(1), 0x55,                         // Pixel Format Set : 0x55 = 16 Bit
  U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), 0x00, 0x00, 0x01, 0x3F,
  U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), 0x00, 0x00, 0x00, 0xEF,
  U8G_ESC_ADR(0), 0xC5, U8G_ESC_ADR(1), 0x3E, 0x28,                   // VCOM Control 1 : 0x3E, 0x28
  U8G_ESC_ADR(0), 0xC7, U8G_ESC_ADR(1), 0x86,                         // VCOM Control 2 : 0x86
  U8G_ESC_ADR(0), 0xB1, U8G_ESC_ADR(1), 0x00, 0x18,                   // Frame Control : 0x00, 0x18
  U8G_ESC_ADR(0), 0xC0, U8G_ESC_ADR(1), 0x23,                         // Power Control 1 : 0x10
  U8G_ESC_ADR(0), 0xC1, U8G_ESC_ADR(1), 0x10,                         // Power Control 2 : 0x10
  U8G_ESC_ADR(0), 0x29,                                               // Turn On Display
  U8G_ESC_ADR(0), 0x11,                                               // Sleep
  U8G_ESC_DLY(100),
  U8G_ESC_END
};

// factory Manual setting is flickering
static const uint8_t ili9486_factory_init_sequence[] = { 
  U8G_ESC_ADR(0),
  0x10,
  U8G_ESC_DLY(10),
  0x01,
  U8G_ESC_DLY(100), U8G_ESC_DLY(100),
  // Adjust Control 2
  U8G_ESC_ADR(0), 0xF2, U8G_ESC_ADR(1), 0x18, 0xA3, 0x12, 0x02, 0xB2, 0x12, 0xFF, 0x10, 0x00,
  // Adjust Control 4
  U8G_ESC_ADR(0), 0xF8, U8G_ESC_ADR(1), 0x21, 0x04, 
  // Adjust Control 5
  U8G_ESC_ADR(0), 0xF9, U8G_ESC_ADR(1), 0x00, 0x08, 

  // Memory Access Control
  // BGR Pixel Order = 1<<3 (0x08)
  // Row Column Exchange = 1<<5 (0x20)
  // Column Adress Order Swap = 1<<6 (0x40)
  // Row Address Order Swap 1<<7 (0x80)
  // Rotate 180 Degress (Column Adress Order Swap | Row Address Order Swap)
  U8G_ESC_ADR(0), 0x36, U8G_ESC_ADR(1), 0xA8,

  // Inversion Control
  U8G_ESC_ADR(0), 0xB4, U8G_ESC_ADR(1), 0x00,

  // Power Control 2
  U8G_ESC_ADR(0), 0xC1, U8G_ESC_ADR(1), 0x41, 
 
  // VCOM Control
  U8G_ESC_ADR(0), 0xC5, U8G_ESC_ADR(1), 0x00, 0x53,

  // Gamma Mode 
  U8G_ESC_ADR(0), 0xE0, U8G_ESC_ADR(1), 0x0F, 0x1B, 0x18, 0x0B, 0x0E, 0x09, 0x47, 0x94, 0x35, 0x0A, 0x13, 0x05, 0x08, 0x03, 0x00, 
  U8G_ESC_ADR(0), 0xE1, U8G_ESC_ADR(1), 0x0F, 0x3A, 0x37, 0x0B, 0x0C, 0x05, 0x4A, 0x24, 0x39, 0x07, 0x10, 0x04, 0x27, 0x25, 0x00, 

  // Sleep Out
  U8G_ESC_ADR(0), 0x11,
  U8G_ESC_DLY(120),

  // Display On
  U8G_ESC_ADR(0), 0x29,
  U8G_ESC_END
};

static const uint8_t ili9486_init_sequence[] = { 
  // Sleep In
  U8G_ESC_ADR(0), 0x10,
  U8G_ESC_DLY(10),

  // Software Reset
  U8G_ESC_ADR(0), 0x01,
  U8G_ESC_DLY(100), U8G_ESC_DLY(100),

  // Init Dari Factory PDF
  // Adjust Control 2 - 
  // Make to much Brightness - better not use it.
  // U8G_ESC_ADR(0), 0xF2, U8G_ESC_ADR(1), 0x18, 0xA3, 0x12, 0x02, 0xB2, 0x12, 0xFF, 0x10, 0x00,
  // Adjust Control 4 - Gamma & Dither Control
  U8G_ESC_ADR(0), 0xF8, U8G_ESC_ADR(1), 0x21, 0x07, 
  // Adjust Control 5 - Chopper Opt
  U8G_ESC_ADR(0), 0xF9, U8G_ESC_ADR(1), 0x00, 0x08, 

  // Memory Access Control
  // BGR Pixel Order = 1<<3 (0x08)
  // Row Column Exchange = 1<<5 (0x20)
  // Column Adress Order Swap = 1<<6 (0x40)
  // Row Address Order Swap 1<<7 (0x80)
  // Rotate 180 Degress (Column Adress Order Swap | Row Address Order Swap)
  U8G_ESC_ADR(0), 0x36, U8G_ESC_ADR(1), 0xA8,

  // Pixel Format 16 bit
  U8G_ESC_ADR(0), 0x3A, U8G_ESC_ADR(1), 0x55,

  // Inversion Control
  U8G_ESC_ADR(0), 0xB4, U8G_ESC_ADR(1), 0x01,

  // Display Function Control
  U8G_ESC_ADR(0), 0xB6, U8G_ESC_ADR(1), 0x02, 0x22,

  // Interface Mode DE polarity=High enable, PCKL polarity=data fetched at rising time, HSYNC polarity=Low level sync clock, VSYNC polarity=Low level sync clock
  //U8G_ESC_ADR(0), 0xB0, U8G_ESC_ADR(1), 0x00,

  // Color Inversion
  // Color Inversion ON
  // U8G_ESC_ADR(0), 0x21, 
  // Color Inversion OFF
  // U8G_ESC_ADR(0), 0x20, 

  //Power Control 1  
  //U8G_ESC_ADR(0), 0xC0, U8G_ESC_ADR(1), 0x0D, 0x0D,                         
  // Power Control 2
  U8G_ESC_ADR(0), 0xC1, U8G_ESC_ADR(1), 0x41, 
  // Power Control 3
  //U8G_ESC_ADR(0), 0xC2, U8G_ESC_ADR(1), 0x33,

  // VCOM Control
  U8G_ESC_ADR(0), 0xC5, U8G_ESC_ADR(1), 0x00, 0x07,
  // V Com COntrol 2 ???
  //U8G_ESC_ADR(0), 0xC7, U8G_ESC_ADR(1), 0x86,

  // Write CTRL Display Value 
  //U8G_ESC_ADR(0), 0x53, U8G_ESC_ADR(1), 0x28,

  // Write Display Brightness Value 
  //U8G_ESC_ADR(0), 0x51, U8G_ESC_ADR(1), 0x40,

  // Write Display Brightness Value 
  //U8G_ESC_ADR(0), 0x55, U8G_ESC_ADR(1), 0x90,

  // Row and Column -> Do we really need it ?
  //U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), 0x00, 0x00, 0x01, 0xDF,
  //U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), 0x00, 0x00, 0x01, 0x3F,

  // Gamma Mode accroding to MCUFRIEND
  // 2
  U8G_ESC_ADR(0), 0xE0, U8G_ESC_ADR(1), 0x0F, 0x1B, 0x18, 0x0B, 0x0E, 0x09, 0x47, 0x94, 0x35, 0x0A, 0x13, 0x05, 0x08, 0x03, 0x00, 
  U8G_ESC_ADR(0), 0xE1, U8G_ESC_ADR(1), 0x0F, 0x3A, 0x37, 0x0B, 0x0C, 0x05, 0x4A, 0x24, 0x39, 0x07, 0x10, 0x04, 0x27, 0x25, 0x00, 

  // 3
  //U8G_ESC_ADR(0), 0xE0, U8G_ESC_ADR(1), 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98, 0x37, 0x0A, 0x13, 0x04, 0x11, 0x0D, 0x00, 
  //U8G_ESC_ADR(0), 0xE1, U8G_ESC_ADR(1), 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75, 0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00, 

  // 4
  //U8G_ESC_ADR(0), 0xE0, U8G_ESC_ADR(1), 0x0F, 0x21, 0x1C, 0x0B, 0x0E, 0x08, 0x49, 0x98, 0x38, 0x09, 0x11, 0x03, 0x14, 0x10, 0x00, 
  //U8G_ESC_ADR(0), 0xE1, U8G_ESC_ADR(1), 0x0F, 0x2F, 0x2B, 0x0C, 0x0E, 0x06, 0x47, 0x76, 0x37, 0x07, 0x11, 0x04, 0x23, 0x1E, 0x00, 
  // Gamma Mode accroding to Factory PDF
  //U8G_ESC_ADR(0), 0xE0, U8G_ESC_ADR(1), 0x0F, 0x1B, 0x18, 0x0B, 0x0E, 0x09, 0x47, 0x94, 0x35, 0x0A, 0x13, 0x05, 0x08, 0x03, 0x00, 
  //U8G_ESC_ADR(0), 0xE1, U8G_ESC_ADR(1), 0x0F, 0x3A, 0x37, 0x0B, 0x0C, 0x05, 0x4A, 0x24, 0x39, 0x07, 0x10, 0x04, 0x27, 0x25, 0x00, 

  // Frame Rate Control
  //U8G_ESC_ADR(0), 0xB1, U8G_ESC_ADR(1), 0x00, 0x18,

  // Sleep Out
  U8G_ESC_ADR(0), 0x11,
  U8G_ESC_DLY(120),

  // Display On
  U8G_ESC_ADR(0), 0x29,
  U8G_ESC_END
};

// Used to fill RGB565 (16bits) background
inline void memset2(const void *ptr, uint16_t fill, size_t cnt) {
  uint16_t* wptr = (uint16_t*) ptr;
  for (size_t i = 0; i < cnt; i += 2) {
     *wptr = fill;
     wptr++;
  }
}

#if ENABLED(TOUCH_BUTTONS)

  void drawNoButton(u8g_t *u8g, u8g_dev_t *dev, uint16_t width, uint16_t height) {
    uint16_t buffer[BUTTON_IMAGE_SIZE_X];
    memset2(buffer, COLOR_BLACK, BUTTON_IMAGE_SIZE_X*2);
    for (uint16_t i = 0; i < height; i++) {
      u8g_WriteSequence(u8g, dev, width << 1, (uint8_t *)(buffer));
    }
  }

  void drawFullScaleImage(const uint8_t *data, u8g_t *u8g, u8g_dev_t *dev, uint16_t width, uint16_t height) {
    for (uint16_t i = 0; i < height; i++) {
      u8g_WriteSequence(u8g, dev, width << 1, (uint8_t *)((data)+((i*width)<<1)));
    }
  }

  void drawImage(const uint8_t *data, u8g_t *u8g, u8g_dev_t *dev, uint16_t length, uint16_t height, uint16_t color) {
    uint16_t buffer[128];

    for (uint16_t i = 0; i < height; i++) {
      uint16_t k = 0;
      for (uint16_t j = 0; j < length; j++) {
        uint16_t v = TFT_MARLINBG_COLOR;
        if (*(data + (i * (length >> 3) + (j >> 3))) & (0x80 >> (j & 7)))
          v = color;
        else
          v = TFT_MARLINBG_COLOR;
        buffer[k++] = v; buffer[k++] = v;
      }
      #ifdef LCD_USE_DMA_FSMC
        if (k <= 80) { // generally is... for our buttons
          memcpy(&buffer[k], &buffer[0], k * sizeof(uint16_t));
          LCD_IO_WriteSequence(buffer, k * sizeof(uint16_t));
        }
        else {
          LCD_IO_WriteSequence(buffer, k);
          LCD_IO_WriteSequence(buffer, k);
        }
      #else
        u8g_WriteSequence(u8g, dev, k << 1, (uint8_t *)buffer);
        u8g_WriteSequence(u8g, dev, k << 1, (uint8_t *)buffer);
      #endif
    }
  }

#endif // TOUCH_BUTTONS

static bool preinit = true;
static uint8_t page;
static bool bootIsDone = false;
static RexyzScreenMode current_screen_mode = SCRMODE_BOOT;

uint8_t u8g_dev_tft_fullScale_touch_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
  u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
  #ifdef LCD_USE_DMA_FSMC
    static uint16_t bufferA[512], bufferB[512];
    uint16_t* buffer = &bufferA[0];
    bool allow_async = true;
  #else
    uint16_t buffer[WIDTH]; // 16-bit RGB 565 pixel line buffer
  #endif
  switch (msg) {
    case U8G_DEV_MSG_INIT:
      dev->com_fn(u8g, U8G_COM_MSG_INIT, U8G_SPI_CLK_CYCLE_NONE, &lcd_id);
      if (lcd_id == 0x040404) return 0; // No connected display on FSMC
      if (lcd_id == 0xFFFFFF) return 0; // No connected display on SPI

      if ((lcd_id & 0xFFFF) == 0x8552)  // ST7789V
        u8g_WriteEscSeqP(u8g, dev, st7789v_init_sequence);
      if ((lcd_id & 0xFFFF) == 0x9341)  // ILI9341
        u8g_WriteEscSeqP(u8g, dev, ili9341_init_sequence);
      if ((lcd_id & 0xFFFF) == 0x8066)  // ILI9488 / ili9486
        u8g_WriteEscSeqP(u8g, dev, ili9486_init_sequence);

      if (preinit) {
        preinit = false;
        return u8g_dev_pb16v2_base_fn(u8g, dev, msg, arg);
      }

      // Clear Screen Sequence
      u8g_WriteEscSeqP(u8g, dev, clear_screen_sequence);
      #ifdef LCD_USE_DMA_FSMC
        LCD_IO_WriteMultiple(TFT_MARLINBG_COLOR, (320*240));
      #else
        memset2(buffer, COLOR_BLACK, WIDTH/2);
        for (uint16_t i = 0; i < LCD_PIXEL_HEIGHT*4; i++)
          u8g_WriteSequence(u8g, dev, WIDTH/2, (uint8_t *)buffer);
        memset2(buffer, COLOR_BLACK, WIDTH/2);
        for (uint16_t i = LCD_PIXEL_HEIGHT*4; i < LCD_FULL_PIXEL_HEIGHT*4; i++)
          u8g_WriteSequence(u8g, dev, WIDTH/2, (uint8_t *)buffer);
      #endif
      return 0;

    case U8G_DEV_MSG_STOP: preinit = true; break;

    case U8G_DEV_MSG_PAGE_FIRST:
      page = 0;
      if (!bootIsDone) {
        if (ui.currentScreen != nullptr) bootIsDone = true;
      }
      if (bootIsDone) {
      // bottom line and buttons
        #if ENABLED(TOUCH_BUTTONS)
          if (current_screen_mode != ui.screenMode) {
            switch(ui.screenMode) {
              case SCRMODE_STATUS:
                u8g_WriteEscSeqP(u8g, dev, buttonD_sequence);
                drawNoButton(u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonA_sequence);
                drawFullScaleImage(buttonA, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonB_sequence);
                drawFullScaleImage(buttonB, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonC_sequence);
                drawFullScaleImage(buttonC, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                break;
              case SCRMODE_SELECT_SCREEN:
              case SCRMODE_CALIBRATION:
                u8g_WriteEscSeqP(u8g, dev, buttonD_sequence);
                drawFullScaleImage(buttonD, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonA_sequence);
                drawNoButton(u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonB_sequence);
                drawNoButton(u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonC_sequence);
                drawFullScaleImage(buttonC, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                break;
              default:
                u8g_WriteEscSeqP(u8g, dev, buttonD_sequence);
                drawFullScaleImage(buttonD, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonA_sequence);
                drawFullScaleImage(buttonA, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonB_sequence);
                drawFullScaleImage(buttonB, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                u8g_WriteEscSeqP(u8g, dev, buttonC_sequence);
                drawFullScaleImage(buttonC, u8g, dev, BUTTON_IMAGE_SIZE_X, BUTTON_IMAGE_SIZE_Y);
                break;
            }
            current_screen_mode = ui.screenMode;
          }
        #endif // TOUCH_BUTTONS
      } 
      if (!thermalManager.fan_speed[0] || ++fan_frame >= 4) fan_frame = 0;
      u8g_WriteEscSeqP(u8g, dev, page_first_sequence);
      break;

    case U8G_DEV_MSG_PAGE_NEXT:
      if (++page > (HEIGHT / PAGE_HEIGHT)) return 1;

      uint16_t color_fg, color_bg; 

      if (bootIsDone) {
        if (page > (10*LCD_CELL_HEIGHT)/8)
          color_bg = ui.on_status_screen() ? COLOR_LIME : TFT_MARLINBG_COLOR;
        else
          color_bg = TFT_MARLINBG_COLOR; 
        color_fg = TFT_MARLINUI_COLOR;
      }
      else {
        color_fg = TFT_MARLINBG_COLOR; 
        color_bg = TFT_MARLINUI_COLOR;
      }

      for (uint8_t y = 0; y < PAGE_HEIGHT; y++) {
        uint32_t k = 0;
        #ifdef LCD_USE_DMA_FSMC
          buffer = (y & 1) ? bufferB : bufferA;
        #endif
        if (y < 4) {
          for (uint16_t i = 0; i < (uint32_t)pb->width; i++) {
            const uint8_t b = *(((uint8_t *)pb->buf) + i);
            const uint8_t clr = (b >> (y<<1)) & 0x03;
            switch(clr) {
              case 0: buffer[k++] = color_bg; break;
              case 1: buffer[k++] = color_fg; break;
              case 2: buffer[k++] = TFT_SELECTED_COLOR; break;
              case 3: buffer[k++] = TFT_DISABLED_COLOR; break;
            }
          }
        }
        else {
          for (uint16_t i = 0; i < (uint32_t)pb->width; i++) {
            const uint8_t b = *((uint32_t)pb->width + ((uint8_t *)pb->buf) + i);
            const uint8_t clr = (b >> ((y-4)<<1)) & 0x03;
            switch(clr) {
              case 0: buffer[k++] = color_bg; break;
              case 1: buffer[k++] = color_fg; break;
              case 2: buffer[k++] = TFT_SELECTED_COLOR; break;
              case 3: buffer[k++] = TFT_DISABLED_COLOR; break;
            }
          }
        }
        if (!bootIsDone) { 
          u8g_int_t lineNum = (page-1)*PAGE_HEIGHT + y - (LCD_FULL_PIXEL_HEIGHT-REXYZ_BOOT_IMAGE_SIZE_Y)/2 + 30;
          if (lineNum >= 0 && lineNum < REXYZ_BOOT_IMAGE_SIZE_Y) {
            const uint8_t start_pt = (LCD_FULL_PIXEL_WIDTH-REXYZ_BOOT_IMAGE_SIZE_X) / 2;
            for(u8g_uint_t j = start_pt; j < start_pt + REXYZ_BOOT_IMAGE_SIZE_X; j++)
              buffer[j] = *(uint16_t*)&RexyzBootImage[(lineNum*REXYZ_BOOT_IMAGE_SIZE_X+(j-start_pt))<<1];
          }
        } 
        if (ui.on_status_screen() && (ui.screenMode != SCRMODE_KILLSCREEN)) {
          u8g_int_t lineNum = (page-1) * PAGE_HEIGHT + y - 4;
          if (lineNum >= 0 && lineNum < LOGO_IMAGE_SIZE_Y) {
            const uint8_t start_pt = (LCD_COLUMN4_WIDTH-LOGO_IMAGE_SIZE_X) / 2;
            for(u8g_uint_t j = start_pt; j < start_pt + LOGO_IMAGE_SIZE_X; j++)
              buffer[j] = *(uint16_t*)&Rexyz_LCD_Logo[(lineNum*LOGO_IMAGE_SIZE_X+(j-start_pt))<<1];
          }
          lineNum -= (LCD_CELL_HEIGHT*3 + 1);
          // Fan has biggest image size on this row
          if (lineNum >= 0 && lineNum < FAN_IMAGE_SIZE) {
            // NOZZLE
            if (lineNum < NOZZLE_IMAGE_SIZE_Y) {
              if (thermalManager.isHeatingHotend(0)) {
                for(u8g_uint_t j = LCD_COLUMN4_WIDTH+10; j < LCD_COLUMN4_WIDTH+NOZZLE_IMAGE_SIZE_X+9; j++)
                  buffer[j] = *(uint16_t*)&nozzle_burned[(lineNum*NOZZLE_IMAGE_SIZE_X+(j-LCD_COLUMN4_WIDTH-10))<<1];
              } else {
                for(u8g_uint_t j = LCD_COLUMN4_WIDTH+10; j < LCD_COLUMN4_WIDTH+NOZZLE_IMAGE_SIZE_X+9; j++)
                  buffer[j] = *(uint16_t*)&nozzle_normal[(lineNum*NOZZLE_IMAGE_SIZE_X+(j-LCD_COLUMN4_WIDTH-10))<<1];
              }
            }
            // BED

            if (lineNum >= (FAN_IMAGE_SIZE-BED_IMAGE_SIZE_Y)) {
              for(u8g_uint_t j = LCD_COLUMN4_WIDTH*2+3; j < LCD_COLUMN4_WIDTH*2+BED_IMAGE_SIZE_X+2; j++)
                buffer[j] = *(uint16_t*)&bed_table[((lineNum-(FAN_IMAGE_SIZE-BED_IMAGE_SIZE_Y))*BED_IMAGE_SIZE_X+(j-LCD_COLUMN4_WIDTH*2-3))<<1];
            }    
            if (thermalManager.isHeatingBed()) {
              if (lineNum >= (FAN_IMAGE_SIZE-BED_IMAGE_SIZE_Y-FLAME_IMAGE_SIZE_Y) && lineNum < (FAN_IMAGE_SIZE-BED_IMAGE_SIZE_Y)) {
                for(u8g_uint_t j = LCD_COLUMN4_WIDTH*2+3; j < LCD_COLUMN4_WIDTH*2+BED_IMAGE_SIZE_X+2; j++)
                  buffer[j] = *(uint16_t*)&bed_flame[((lineNum-(FAN_IMAGE_SIZE-BED_IMAGE_SIZE_Y-FLAME_IMAGE_SIZE_Y))*BED_IMAGE_SIZE_X+(j-LCD_COLUMN4_WIDTH*2-3))<<1];
              }    
            } 

            // FAN
            switch(fan_frame) {
              case 0:
                for(u8g_uint_t j = LCD_COLUMN4_WIDTH*3+3; j < LCD_COLUMN4_WIDTH*3+FAN_IMAGE_SIZE+2; j++)
                  buffer[j] = *(uint16_t*)&rotating_fan1[(lineNum*FAN_IMAGE_SIZE+(j-LCD_COLUMN4_WIDTH*3-3))<<1];
                break;
              case 1:
                for(u8g_uint_t j = LCD_COLUMN4_WIDTH*3+3; j < LCD_COLUMN4_WIDTH*3+FAN_IMAGE_SIZE+2; j++)
                  buffer[j] = *(uint16_t*)&rotating_fan2[(lineNum*FAN_IMAGE_SIZE+(j-LCD_COLUMN4_WIDTH*3-3))<<1];
                break;
              case 2:
                for(u8g_uint_t j = LCD_COLUMN4_WIDTH*3+3; j < LCD_COLUMN4_WIDTH*3+FAN_IMAGE_SIZE+2; j++)
                  buffer[j] = *(uint16_t*)&rotating_fan3[(lineNum*FAN_IMAGE_SIZE+(j-LCD_COLUMN4_WIDTH*3-3))<<1];
                break;
              case 3:
                for(u8g_uint_t j = LCD_COLUMN4_WIDTH*3+3; j < LCD_COLUMN4_WIDTH*3+FAN_IMAGE_SIZE+2; j++)
                  buffer[j] = *(uint16_t*)&rotating_fan4[(lineNum*FAN_IMAGE_SIZE+(j-LCD_COLUMN4_WIDTH*3-3))<<1];
                break;
            }
          }
        }
        #ifdef LCD_USE_DMA_FSMC
          memcpy(&buffer[256], &buffer[0], 512);
          if (allow_async) {
            if (y > 0 || page > 1) LCD_IO_WaitSequence_Async();
            if (y == 7 && page == 8)
              LCD_IO_WriteSequence(buffer, 512); // last line of last page
            else
              LCD_IO_WriteSequence_Async(buffer, 512);
          }
          else
            LCD_IO_WriteSequence(buffer, 512);
        #else
          u8g_WriteSequence(u8g, dev, WIDTH / 2, (uint8_t*)buffer);
          u8g_WriteSequence(u8g, dev, WIDTH / 2, (uint8_t*)&(buffer[WIDTH / 4]));
          u8g_WriteSequence(u8g, dev, WIDTH / 2, (uint8_t*)&(buffer[WIDTH / 2]));
          u8g_WriteSequence(u8g, dev, WIDTH / 2, (uint8_t*)&(buffer[WIDTH * 3 / 4]));
        #endif
      }
      break;

    case U8G_DEV_MSG_SLEEP_ON:
      // Enter Sleep Mode (10h)
      return 1;
    case U8G_DEV_MSG_SLEEP_OFF:
      // Sleep Out (11h)
      return 1;
  }
  return u8g_dev_pb16v2_base_fn(u8g, dev, msg, arg);
}

//U8G_PB_DEV(u8g_dev_tft_fullScale_touch, WIDTH, HEIGHT, PAGE_HEIGHT, u8g_dev_tft_fullScale_touch_fn, U8G_COM_HAL_FSMC_FN);
//U8G_PB_DEV is for 8 bit
//lines below is for 16 bit
/*
 * #define U8G_PB_DEV(name, width, height, page_height, dev_fn, com_fn) \
 * uint8_t name##_buf[width*2] U8G_NOCOMMON ; \
 * u8g_pb_t name##_pb = { {page_height, height, 0, 0, 0},  width, name##_buf}; \
 * u8g_dev_t name = { dev_fn, &name##_pb, com_fn }
 */
uint8_t  u8g_dev_tft_fullScale_touch_buf[WIDTH*2] U8G_NOCOMMON ; 
u8g_pb_t u8g_dev_tft_fullScale_touch_pb = { {PAGE_HEIGHT, HEIGHT, 0, 0, 0},  WIDTH, u8g_dev_tft_fullScale_touch_buf}; 
u8g_dev_t u8g_dev_tft_fullScale_touch = { u8g_dev_tft_fullScale_touch_fn, &u8g_dev_tft_fullScale_touch_pb, U8G_COM_HAL_FSMC_FN };

#endif // HAS_GRAPHICAL_LCD && HAS_FULL_SCALE_TFT
