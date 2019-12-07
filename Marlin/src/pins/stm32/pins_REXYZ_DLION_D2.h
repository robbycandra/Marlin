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
#pragma once

 /**
  *
  *   REXYZ 3D Printer by Rajawali3d
  *
  *   Pin assignments for 32-bit Dlion Rexyz D2 Printer
  */

#ifndef __STM32F1__
  #error "Oops! Select an STM32F1 board in 'Tools > Board.'"
#elif HOTENDS > 1 || E_STEPPERS > 1
  #error "Dlion Rexyz D2 32-bit board only supports 1 hotend / E-stepper. Comment out this line to continue."
#endif
#define BOARD_INFO_NAME "Dlion Rexyz D2 board"

//#define STM32_XL_DENSITY
// #define MCU_STM32F103ZE // not yet required
// Enable EEPROM Emulation for this board, so that we don't overwrite factory data

//#define I2C_EEPROM   // AT24C64
//#define E2END 0x7FFF // 64KB
//#define FLASH_EEPROM_EMULATION
//#define E2END 0xFFF // 4KB
//#define E2END uint32(EEPROM_START_ADDRESS + (EEPROM_PAGE_SIZE * 2) - 1)
//#define EEPROM_CHITCHAT
//#define DEBUG_EEPROM_READWRITE

//
// Limit Switches
//
#define X_MIN_PIN          PD6
#define Y_MIN_PIN          PG9
#define Z_MIN_PIN          PG10
//#define X_MAX_PIN          PG15
//#define Y_MAX_PIN          PG14
#define Z_MAX_PIN          PG13

#define FIL_RUNOUT_PIN     PG11
#define EX_IO_PIN          PB8

//
// Steppers
//
#define X_STEP_PIN         PD12
#define X_DIR_PIN          PD11
#define X_ENABLE_PIN       PD13

#define Y_STEP_PIN         PG3
#define Y_DIR_PIN          PG2
#define Y_ENABLE_PIN       PG4

#define E1_STEP_PIN        PG6
#define E1_DIR_PIN         PG5
#define E1_ENABLE_PIN      PG7

#define E0_STEP_PIN        PC6
#define E0_DIR_PIN         PG8
#define E0_ENABLE_PIN      PC7

#define Z_STEP_PIN         PA12
#define Z_DIR_PIN          PA11
#define Z_ENABLE_PIN       PG7  // This PIN is share with Z2 (E1)

//
// Temperature Sensors
//
#define TEMP_0_PIN         PC1
#define TEMP_BED_PIN       PC0

//
// Heaters / Fans
//
#define HEATER_0_PIN       PB0
#define HEATER_BED_PIN     PB1

#define FAN_PIN            PA1
#define MANUAL_FAN_PIN     PE1

#define FAN_SOFT_PWM

//
// LCD
//
#define LCD_BACKLIGHT_PIN  PF11
#define FSMC_CS_PIN        PD7
#define FSMC_RS_PIN        PG0

#define LCD_USE_DMA_FSMC          // Use DMA transfers to send data to the TFT
#define FSMC_DMA_DEV       DMA2
#define FSMC_DMA_CHANNEL   DMA_CH5

#define DOGLCD_MOSI        -1  // Prevent auto-define by Conditionals_post.h
#define DOGLCD_SCK         -1

//
// SD Card
//
#define SD_DETECT_PIN     PC2
//#define SCK_PIN           PC12
//#define MISO_PIN          PB8
//#define MOSI_PIN          PD2
//#define SS_PIN            PC11

//
// Misc.
//
#define BEEPER_PIN         PE4  //Rexyz PC3    // use PB7 to shut up if desired

/**
 * STM32F1 Default SPI Pins
 *
 *         SS     SCK     MISO    MOSI
 *       +-----------------------------+
 *  SPI1 | PA4    PA5     PA6     PA7  |
 *  SPI2 | PB12   PB13    PB14    PB15 |
 *  SPI3 | PA15   PB3     PB4     PB5  |
 *       +-----------------------------+
 * Any pin can be used for Chip Select (SS_PIN)
 * SPI1 is enabled by default
 */

//
// Touch support
//
#if ENABLED(TOUCH_BUTTONS)
  #define TOUCH_CS_PIN     PA4
  #define TOUCH_INT_PIN    PC4
  #define TOUCH_MISO_PIN   PA6
  #define TOUCH_MOSI_PIN   PA7
  #define TOUCH_SCK_PIN    PA5
#endif

//
// Persistent Storage
// If no option is selected below the SD Card will be used
//
//#define SPI_EEPROM
#define FLASH_EEPROM_EMULATION

#undef E2END
#if ENABLED(SPI_EEPROM)
  // SPI2 EEPROM Winbond W25Q64 (8MB/64Mbits)
  #define ENABLE_SPI2
  #define SPI_CHAN_EEPROM1   2
  #define SPI_EEPROM1_CS     BOARD_SPI2_NSS_PIN   // pin 34
  #define EEPROM_SCK         BOARD_SPI2_SCK_PIN   // PA5 pin 30
  #define EEPROM_MISO        BOARD_SPI2_MISO_PIN  // PA6 pin 31
  #define EEPROM_MOSI        BOARD_SPI2_MOSI_PIN  // PA7 pin 32

  #define EEPROM_PAGE_SIZE   0x1000U              // 4KB (from datasheet)
  #define E2END ((16 * EEPROM_PAGE_SIZE)-1)       // Limit to 64KB for now...
#elif ENABLED(FLASH_EEPROM_EMULATION)
  // SoC Flash (framework-arduinoststm32-maple/STM32F1/libraries/EEPROM/EEPROM.h)
  #define EEPROM_START_ADDRESS (0x8000000UL + (512 * 1024) - 2 * EEPROM_PAGE_SIZE)
  #define EEPROM_PAGE_SIZE     (0x800U)     // 2KB, but will use 2x more (4KB)
  #define E2END (EEPROM_PAGE_SIZE - 1)
#else
  #define E2END (0x7FFU) // On SD, Limit to 2KB, require this amount of RAM
#endif
