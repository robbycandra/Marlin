#include "../../inc/MarlinConfig.h"

#define LCD_COLUMN      0x2A   /* Colomn address register */
#define LCD_ROW         0x2B   /* Row address register */
#define LCD_WRITE_RAM   0x2C
#define U8G_ESC_DATA(x) (uint8_t)(x >> 8), (uint8_t)(x & 0xFF)

#define COLOR_BLACK       0x0000 //#000000
#define COLOR_WHITE       0xFFFF //#FFFFFF
#define COLOR_SILVER      0xC618 //#C0C0C0
#define COLOR_GREY        0x7BEF //#808080
#define COLOR_DARKGREY    0x4208 //#404040
#define COLOR_DARKGREY2   0x39E7 //#303030

#define COLOR_RED         0xF800 //#FF0000
#define COLOR_LIME        0x7E00 //#00FF00
#define COLOR_BLUE        0x001F //#0000FF
#define COLOR_YELLOW      0xFFE0 //#FFFF00
#define COLOR_MAGENTA     0xF81F //#FF00FF 
#define COLOR_FUCHSIA     0xF81F //#FF00FF 
#define COLOR_CYAN        0x07FF //#00FFFF 
#define COLOR_AQUA        0x07FF //#00FFFF 

#define COLOR_MAROON      0x7800 //#800000
#define COLOR_GREEN       0x03E0 //#008000
#define COLOR_NAVY        0x000F //#000080
#define COLOR_OLIVE       0x8400 //#808000
#define COLOR_PURPLE      0x8010 //#800080
#define COLOR_TEAL        0x0410 //#008080

#define COLOR_ORANGE      0xFA00 //#FF7F00

