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

#include "../gcode.h"
#include "../../lcd/menu/touch/fsmc.h"

/**
 * M3301: REXYZ hardware report
 */

void print_controller(uint16_t lcd_id) {
  char controller[8];
  switch(lcd_id) {
  case 0x1505: sprintf(controller, "R61505U"); break;
  case 0x8552: sprintf(controller, "ST7789V"); break;
  case 0x8989: sprintf(controller, "SSD1289"); break;
  case 0x9325: sprintf(controller, "ILI9325"); break;
  case 0x9328: sprintf(controller, "ILI9328"); break;
  case 0x9341: sprintf(controller, "ILI9341"); break;
  case 0x8066: sprintf(controller, "ILI9488"); break;
  case 0x9486: sprintf(controller, "ILI9486"); break;
  case 0x0404: sprintf(controller, "NONE");    break;
  default:     sprintf(controller, "Unknown");
  }
  MYSERIAL0.print("LCD Controller: ");
  MYSERIAL0.print(controller);
  MYSERIAL0.print(" ");
  MYSERIAL0.println(lcd_id, HEX);
  MYSERIAL0.flush();
}

void GcodeSuite::M3301() {

  uint16_t reg, lcdId = 0;
  volatile uint32_t data;

  //lcdBacklightOff();
  //lcdReset();

  MYSERIAL0.println("Simple TFT LCD Information");

  #if ENABLED(LCD_USE_DMA_FSMC)
    dma_init(FSMC_DMA_DEV);
    dma_disable(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);
    dma_set_priority(FSMC_DMA_DEV, FSMC_DMA_CHANNEL, DMA_PRIORITY_MEDIUM);
  #endif

  //lcdBacklightOn();

  // read 8bit data at 0
  reg = TOUCH_LCD_IO_ReadReg(0x00);
  MYSERIAL0.print("Reg at 0: ");
  MYSERIAL0.println(reg, HEX);
  reg &= 0xFF;

  if (reg == 0 || reg == 0xFF) {
    // read ID1 register to get LCD controller ID, MOST of the time located in register 0x04
    data = TOUCH_LCD_IO_ReadData(0x04, 3);
    lcdId = (uint16_t)(data & 0xFFFF);
    MYSERIAL0.print("ST5589V LCD ID : ");
    MYSERIAL0.println((data >> 16) & 0xFF, HEX);
    print_controller(lcdId);

    //If ID1 is 0, it means we need to check alternate registers, like 0xD3 in the case of ILI9341
    data = TOUCH_LCD_IO_ReadData(0xD3, 3);
    lcdId = (uint16_t)(data & 0xFFFF);
    MYSERIAL0.print("ILI lcd ID : ");
    MYSERIAL0.println((data >> 16) & 0xFF, HEX);
    print_controller(lcdId);
  }
}

