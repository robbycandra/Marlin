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

#include "../../inc/MarlinConfig.h"

#if ENABLED(FSMC_GRAPHICAL_TFT)

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
  SERIAL_ECHO("LCD Controller: ");
  SERIAL_ECHO(controller);
  sprintf(controller, " %x",lcd_id); 
  SERIAL_ECHOLN(controller);
}

void GcodeSuite::M3301() {

  uint16_t reg, lcdId = 0;
  volatile uint32_t data;

  //lcdBacklightOff();
  //lcdReset();

  MYSERIAL0.println("Simple TFT LCD Information - MYSERIAL0");
  MYSERIAL1.println("Simple TFT LCD Information - MYSERIAL1");
  UsbSerial.println("Simple TFT LCD Information - UsbSerial");
  SERIAL_ECHOLN("Simple TFT LCD Information - Serial Echo");

  #if ENABLED(LCD_USE_DMA_FSMC)
    dma_init(FSMC_DMA_DEV);
    dma_disable(FSMC_DMA_DEV, FSMC_DMA_CHANNEL);
    dma_set_priority(FSMC_DMA_DEV, FSMC_DMA_CHANNEL, DMA_PRIORITY_MEDIUM);
  #endif

  //lcdBacklightOn();

  char response[50];
  // read 8bit data at 0
  reg = TOUCH_LCD_IO_ReadReg(0x00);
  sprintf(response, "Reg at 0: %x",reg); 
  SERIAL_ECHOLN(response);
  SERIAL_ECHOLN();
  reg &= 0xFF;

  if (reg == 0 || reg == 0xFF) {
    // read ID1 register to get LCD controller ID, MOST of the time located in register 0x04
    data = TOUCH_LCD_IO_ReadData(0x04, 3);
    lcdId = (uint16_t)(data & 0xFFFF);
    sprintf(response, "At pos 0x04 - LCD ID : %x",(uint16_t)((data >> 16) & 0xFF)); 
    SERIAL_ECHOLN(response);
    print_controller(lcdId);
    SERIAL_ECHOLN();

    //If ID1 is 0, it means we need to check alternate registers, like 0xD3 in the case of ILI9341
    data = TOUCH_LCD_IO_ReadData(0xD3, 3);
    lcdId = (uint16_t)(data & 0xFFFF);
    sprintf(response, "At pos 0xD3 - LCD ID : %x",(uint16_t)((data >> 16) & 0xFF)); 
    SERIAL_ECHOLN(response);
    print_controller(lcdId);
    SERIAL_ECHOLN();
  }
}

#endif // FSMC_GRAPHICAL_LCD