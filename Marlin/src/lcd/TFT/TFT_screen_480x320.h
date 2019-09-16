#include "../../inc/MarlinConfig.h"
#include "TFT_screen.h"


#if ENABLED(TOUCH_BUTTONS)

  static const uint8_t buttonD_sequence[] = {
    U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), U8G_ESC_DATA(34), U8G_ESC_DATA(97),
    U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), U8G_ESC_DATA(260), U8G_ESC_DATA(299),
    U8G_ESC_ADR(0), LCD_WRITE_RAM, U8G_ESC_ADR(1),
    U8G_ESC_END
  };

  static const uint8_t buttonA_sequence[] = {
    U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), U8G_ESC_DATA(150), U8G_ESC_DATA(213),
    U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), U8G_ESC_DATA(260), U8G_ESC_DATA(299),
    U8G_ESC_ADR(0), LCD_WRITE_RAM, U8G_ESC_ADR(1),
    U8G_ESC_END
  };

  static const uint8_t buttonB_sequence[] = {
    U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), U8G_ESC_DATA(266), U8G_ESC_DATA(329),
    U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), U8G_ESC_DATA(260), U8G_ESC_DATA(299),
    U8G_ESC_ADR(0), LCD_WRITE_RAM, U8G_ESC_ADR(1),
    U8G_ESC_END
  };

  static const uint8_t buttonC_sequence[] = {
    U8G_ESC_ADR(0), LCD_COLUMN, U8G_ESC_ADR(1), U8G_ESC_DATA(382), U8G_ESC_DATA(445),
    U8G_ESC_ADR(0), LCD_ROW,    U8G_ESC_ADR(1), U8G_ESC_DATA(260), U8G_ESC_DATA(299),
    U8G_ESC_ADR(0), LCD_WRITE_RAM, U8G_ESC_ADR(1),
    U8G_ESC_END
  };

#endif
