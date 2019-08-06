#ifndef CONDITIONALS_REXYZ
#define CONDITIONALS_REXYZ

/* We define REXYZ_ macros based on which printer or toolhead we are
 * building for, these macros are then placed where appropriate in the
 * Marlin source.
 *
 * Using our own set of macros and limiting changes to Marlin's files
 * to one-liners makes it easier to merge from upstream. All macros are
 * prefixed with REXYZ_ so that it is easy to see what was changed and
 * where. If a default setting is commented out in Marlin, we define
 * REXYZ_{SETTING}_DISABLED here, so we have a record of things that
 * got disabled.
 *
 * Table of Contents:
 * ------------------
 *
 *   1. PRINTER MODEL CHARACTERISTICS
 *   -. Board
 *   2. Thermal Setting
 *   -. LCD & Sound
 *   3. Endstop & Motor Setting
 *   4. Bed Setting
 *   5. Probe Setting
 *
 */

#if ENABLED(REXYZ_MARKING_INIT) || ENABLED(REXYZ_MARKING_UPDATE)
  #define REXYZ_FW_VERSION ".INIT" // Change this with each update
#else
  #define REXYZ_FW_VERSION ".10" // Change this with each update
#endif
#define REXYZ_DEFAULT_MACHINE_UUID "cede2a2f-41a2-4748-9b12-c55c62f367ff"
#define REXYZ_SOURCE_CODE_URL "http://rajawali3d.com"
#define REXYZ_WEBSITE_URL "http://rajawali3d.com"

/*
 #if ( \
    !defined(REXYZ_S22) && \
    !defined(REXYZ_S33) && \
    !defined(REXYZ_N2) && \
    !defined(REXYZ_N3) && \
    !defined(REXYZ_4MAX) \
)
    #error Must specify printer model. Please see "Configuration_Rexyz.h" for directions.
#endif
*/

/**
 * Allow one and only one Printer to be defined
 */
#if 1 != 0 \
  + ENABLED(REXYZ_S22)  \
  + ENABLED(REXYZ_S33)  \
  + ENABLED(REXYZ_4MAX) \
  + ENABLED(REXYZ_N2)   \
  + ENABLED(REXYZ_N3)   \
  + ENABLED(REXYZ_A8P)  \
  + ENABLED(REXYZ_A1)   \
  + ENABLED(REXYZ_A2)
  #error "Please enable one and only one Printer."
#endif

/**
 * Allow one and only one Toolhead to be defined
 */
#if 1 != 0 \
  + ENABLED(REXYZ_NO_ABL) \
  + ENABLED(REXYZ_MK8_MANUAL_PROBE) \
  + ENABLED(REXYZ_MK8_MULTI_FIXPROBE_PROXIMITY) \
  + ENABLED(REXYZ_MK8_MULTI_FIXPROBE_MANUAL) \
  + ENABLED(REXYZ_MK8_MULTI_FIXPROBE_BLTOUCH) \
  + ENABLED(REXYZ_MK8_PROXIMITY_8MM) 
  #error "Please enable one and only one toolhead model."
#endif

//===========================================================================
//============================= PRINTER MODEL Settings ============================
//===========================================================================
#if defined(REXYZ_S22)
    #define REXYZ_MACHINE_FRAME_TYPE "S22"
    #define REXYZ_BOARD_MKSGENL
    #define REXYZ_S_TYPE
    #define REXYZ_LCD2004
#endif
#if defined(REXYZ_S33)
    #define REXYZ_MACHINE_FRAME_TYPE "S33"
    #define REXYZ_BOARD_MKSGENL
    #define REXYZ_S_TYPE
    #define REXYZ_LCD2004
#endif
#if defined(REXYZ_4MAX)
    #define REXYZ_MACHINE_FRAME_TYPE "4MAX"
    #define REXYZ_BOARD_TRIGORILLA14
#endif
#if defined(REXYZ_N2)
    #define REXYZ_MACHINE_FRAME_TYPE "N2"
    #define REXYZ_BOARD_SKR13
    #define REXYZ_N_TYPE
    #define REXYZ_LCD2004
#endif
#if defined(REXYZ_N3)
    #define REXYZ_MACHINE_FRAME_TYPE "N3"
    #define REXYZ_BOARD_SKR13
    #define REXYZ_N_TYPE
    #define REXYZ_LCD12864
#endif
#if defined(REXYZ_A8P)
    #define REXYZ_MACHINE_FRAME_TYPE "A8P"
    #define REXYZ_BOARD_SKR13
#endif
#if defined(REXYZ_A1)
    #define REXYZ_MACHINE_FRAME_TYPE "A1"
    #define REXYZ_BOARD_ROBINMINI
#endif
#if defined(REXYZ_A2)
    #define REXYZ_MACHINE_FRAME_TYPE "A2"
    #define REXYZ_BOARD_ROBIN
#endif

//===========================================================================
//============================= Board Settings ============================
//===========================================================================

#if defined(REXYZ_BOARD_MKSGENL)
    #define REXYZ_MOTHERBOARD BOARD_RAMPS_14_EFB
    #define ENDSTOP_INTERRUPTS_FEATURE
    #define REXYZ_SERIAL_PORT 0
    #define SD_DETECT_INVERTED
#endif
#if defined(REXYZ_BOARD_TRIGORILLA14)
    #define REXYZ_MOTHERBOARD BOARD_TRIGORILLA_14
    #define ENDSTOP_INTERRUPTS_FEATURE
    #define REXYZ_SERIAL_PORT 0
    #define SD_DETECT_INVERTED
#endif
#if defined(REXYZ_BOARD_SKR13)
    #define REXYZ_MOTHERBOARD BOARD_BIGTREE_SKR_V1_3
    #define REXYZ_SERIAL_PORT -1
    // Use Onboard SD Card.
    #define SDCARD_CONNECTION ONBOARD
    #define SD_DETECT_INVERTED
  #if defined(REXYZ_TOUCH_UI)
      // If using TFT (or not using)
      //#define REXYZ_SERIAL_PORT -1
    #define SERIAL_PORT_2 0
  #endif
#endif
#if defined(REXYZ_BOARD_ROBINMINI)
    #define MOTHERBOARD BOARD_MKS_ROBIN_MINI
    #define REXYZ_SERIAL_PORT 3
    #define SERIAL_PORT_2 1
    #define NUM_SERIAL 2
    #define SDIO_SUPPORT
    //#define SD_DETECT_INVERTED
#endif
#if defined(REXYZ_BOARD_ROBIN)
    #define MOTHERBOARD BOARD_MKS_ROBIN
    #define REXYZ_SERIAL_PORT 3
    #define SERIAL_PORT_2 1
    #define NUM_SERIAL 2
    #define SDIO_SUPPORT
    //#define SD_DETECT_INVERTED
#endif

#if defined(REXYZ_S_TYPE)
    #define REXYZ_FIL_RUNOUT_INVERTING false
    #define FIL_RUNOUT_PIN 14 //Y_MIN_PIN //14

    #define REXYZ_CONTROLLER_FAN_PIN -1    // Set a custom pin for the controller fan
    #define REXYZ_CONTROLLERFAN_SPEED 225        // 255 == full speed
    #define REXYZ_E0_AUTO_FAN_PIN -1
#endif
#if defined(REXYZ_4MAX)
    #define REXYZ_FIL_RUNOUT_INVERTING true

    // Default 4MAX use X_MIN, Y_MIN, and Z_MIN
    // FIL_RUNOUT_PIN use Z_MAZ
    #define FIL_RUNOUT_PIN 15 //Y_MAX_PIN //19

    #define REXYZ_USE_CONTROLLER_FAN 
    #define REXYZ_CONTROLLER_FAN_PIN FAN1_PIN    // Set a custom pin for the controller fan
    #define REXYZ_CONTROLLERFAN_SPEED 127        // 255 == full speed
    #define REXYZ_E0_AUTO_FAN_PIN FAN2_PIN
#endif
#if defined(REXYZ_N_TYPE)
    #define REXYZ_FIL_RUNOUT_INVERTING false
    #define FIL_RUNOUT_PIN P1_27    // Y_MIN_PIN 

    #define REXYZ_CONTROLLER_FAN_PIN -1   // Set a custom pin for the controller fan
    #define REXYZ_CONTROLLERFAN_SPEED 225 // 255 == full speed
    #define REXYZ_E0_AUTO_FAN_PIN FAN1_PIN
#endif
#if defined(REXYZ_A8P)
    #define REXYZ_FIL_RUNOUT_INVERTING true
    #define FIL_RUNOUT_PIN P1_26    // Y_MAX_PIN 

    #define REXYZ_CONTROLLER_FAN_PIN -1   // Set a custom pin for the controller fan
    #define REXYZ_CONTROLLERFAN_SPEED 225 // 255 == full speed
    #define REXYZ_E0_AUTO_FAN_PIN FAN1_PIN
#endif
#if defined(REXYZ_BOARD_ROBINMINI)
    #define REXYZ_FIL_RUNOUT_INVERTING false
    #define REXYZ_E0_AUTO_FAN_PIN -1
#endif
#if defined(REXYZ_BOARD_ROBIN)
    #define REXYZ_FIL_RUNOUT_INVERTING false
    #define REXYZ_E0_AUTO_FAN_PIN -1
#endif

//===========================================================================
//============================= Thermal Settings ============================
//===========================================================================

    #define REXYZ_TEMP_RESIDENCY_TIME  6 // (seconds) Time to wait for hotend to "settle" in M109
    #define REXYZ_TEMP_RESIDENCY_TIME  6 // (seconds) Time to wait for bed to "settle" in M190
    #define REXYZ_BED_MAXTEMP 130

    #define PID_EDIT_MENU       // Add PID editing to the "Advanced Settings" menu. (~700 bytes of PROGMEM)
    #define PID_AUTOTUNE_MENU   // Add PID auto-tuning to the "Advanced Settings" menu. (~250 bytes of PROGMEM)
    #define REXYZ_PID_FUNCTIONAL_RANGE 20 // If the temperature difference between the target temperature and the actual temperature

    #define REXYZ_THERMAL_PROTECTION_PERIOD 60        // Seconds
    #define REXYZ_WATCH_TEMP_PERIOD 40                // Seconds
    #define REXYZ_THERMAL_PROTECTION_BED_PERIOD 40    // Seconds
    #define REXYZ_WATCH_BED_TEMP_PERIOD 120           // Seconds

#if defined(REXYZ_BOARD_MKSGENL)
    #define REXYZ_EXTRUDER_AUTO_FAN_SPEED 255  // 255 == full speed
#endif
#if defined(REXYZ_BOARD_SKR13)
    #define EXTRUDER_AUTO_FAN_TEMPERATURE_MAX 130
    #define EXTRUDER_AUTO_FAN_SPEED_MAX 191    // 255 == full speed
    #define REXYZ_EXTRUDER_AUTO_FAN_SPEED 63   // 255 == full speed
#endif

#if defined(REXYZ_S_TYPE)
    #define REXYZ_DEFAULT_Kp 20.81
    #define REXYZ_DEFAULT_Ki 1.46
    #define REXYZ_DEFAULT_Kd 74.17

    #define REXYZ_PREHEAT_1_TEMP_BED  60  
    #define REXYZ_PREHEAT_2_TEMP_BED  90
#endif
#if defined(REXYZ_4MAX)
    #define REXYZ_DEFAULT_Kp 16.69
    #define REXYZ_DEFAULT_Ki 0.94
    #define REXYZ_DEFAULT_Kd 73.85

    #define REXYZ_PREHEAT_1_TEMP_BED  60  
    #define REXYZ_PREHEAT_2_TEMP_BED  100
#endif
#if defined(REXYZ_N_TYPE)
    #define REXYZ_DEFAULT_Kp 9.60
    #define REXYZ_DEFAULT_Ki 0.56
    #define REXYZ_DEFAULT_Kd 41.53

    #define REXYZ_PREHEAT_1_TEMP_BED  60  
    #define REXYZ_PREHEAT_2_TEMP_BED  90
#endif
#if defined(REXYZ_A8P)
    #define REXYZ_DEFAULT_Kp 16.69
    #define REXYZ_DEFAULT_Ki 0.94
    #define REXYZ_DEFAULT_Kd 73.85

    #define REXYZ_PREHEAT_1_TEMP_BED  60  
    #define REXYZ_PREHEAT_2_TEMP_BED  80
#endif
#if defined(REXYZ_A1) || defined(REXYZ_A2)
  // Ultimaker
  #define REXYZ_DEFAULT_Kp 22.2
  #define REXYZ_DEFAULT_Ki 1.08
  #define REXYZ_DEFAULT_Kd 114

    #define REXYZ_PREHEAT_1_TEMP_BED  60  
    #define REXYZ_PREHEAT_2_TEMP_BED  80

  #define PIDTEMPBED

  #define DEFAULT_bedKp 10.00
  #define DEFAULT_bedKi .023
  #define DEFAULT_bedKd 305.4
#endif

//===========================================================================
//============================= Endstop & Motor Settings ====================
//===========================================================================

#if defined(REXYZ_S_TYPE) || defined(REXYZ_N_TYPE)
    #define REXYZ_USE_ZMIN_PLUG
    #define REXYZ_USE_XMAX_PLUG
    #define REXYZ_USE_YMAX_PLUG
    #define REXYZ_USE_ZMAX_PLUG

    #define REXYZ_X_MIN_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MIN_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MIN_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_X_MAX_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MAX_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MAX_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.

    #define REXYZ_INVERT_X_DIR false
    #define REXYZ_INVERT_Y_DIR true
    #define REXYZ_INVERT_Z_DIR true
    #define REXYZ_INVERT_E0_DIR false
    #define REXYZ_INVERT_E1_DIR true

    #define REXYZ_X_HOME_DIR 1
    #define REXYZ_Y_HOME_DIR 1
    #define REXYZ_Z_HOME_DIR 1

    #define Z_DUAL_STEPPER_DRIVERS
    #define Z_DUAL_ENDSTOPS
    #define REXYZ_Z2_USE_ENDSTOP _ZMIN_
#endif
#if defined(REXYZ_4MAX)
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_USE_YMIN_PLUG
    #define REXYZ_USE_ZMAX_PLUG

    #define REXYZ_X_MIN_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MIN_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MIN_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_X_MAX_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MAX_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MAX_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.

    #define REXYZ_INVERT_X_DIR false
    #define REXYZ_INVERT_Y_DIR false
    #define REXYZ_INVERT_Z_DIR false
    #define REXYZ_INVERT_E0_DIR false
    #define REXYZ_INVERT_E1_DIR false

    #define REXYZ_X_HOME_DIR -1
    #define REXYZ_Y_HOME_DIR -1
    #define REXYZ_Z_HOME_DIR 1
#endif

#if defined(REXYZ_A8P)
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_USE_YMIN_PLUG
    #define REXYZ_USE_ZMIN_PLUG
    #define REXYZ_USE_ZMAX_PLUG

    #define REXYZ_X_MIN_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MIN_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MIN_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_X_MAX_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MAX_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MAX_ENDSTOP_INVERTING true // set to true to invert the logic of the endstop.

    #define REXYZ_INVERT_X_DIR false
    #define REXYZ_INVERT_Y_DIR false
    #define REXYZ_INVERT_Z_DIR false
    #define REXYZ_INVERT_E0_DIR true
    #define REXYZ_INVERT_E1_DIR false

    #define REXYZ_X_HOME_DIR -1
    #define REXYZ_Y_HOME_DIR -1
    #define REXYZ_Z_HOME_DIR -1

    #define Z_DUAL_STEPPER_DRIVERS
    #define Z_DUAL_ENDSTOPS
    #define REXYZ_Z2_USE_ENDSTOP _ZMAX_

#endif

#if defined(REXYZ_A1) || defined(REXYZ_A2)
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_USE_YMIN_PLUG
    #define REXYZ_USE_ZMIN_PLUG

    #define REXYZ_X_MIN_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MIN_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MIN_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_X_MAX_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Y_MAX_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.
    #define REXYZ_Z_MAX_ENDSTOP_INVERTING false // set to true to invert the logic of the endstop.

    #define REXYZ_INVERT_X_DIR false
    #define REXYZ_INVERT_Y_DIR true
    #define REXYZ_INVERT_Z_DIR false
    #define REXYZ_INVERT_E0_DIR false
    #define REXYZ_INVERT_E1_DIR false

    #define REXYZ_X_HOME_DIR -1
    #define REXYZ_Y_HOME_DIR -1
    #define REXYZ_Z_HOME_DIR -1

#endif

//===========================================================================
//============================= Bed Settings ====================
//===========================================================================

#define REXYZ_LEVEL_CORNERS_Z_HOP 10.0  // (mm) Move nozzle up before moving between corners
#if defined(REXYZ_S22) || defined(REXYZ_N2)
    #define REXYZ_X_BED_SIZE 220
    #define REXYZ_Y_BED_SIZE 220
    #define REXYZ_X_MIN_POS 0
    #define REXYZ_Y_MIN_POS 0
    #define REXYZ_Z_MIN_POS 0
    #define REXYZ_X_MAX_POS 223
    #define REXYZ_Y_MAX_POS 220
    #define REXYZ_Z_MAX_POS 255

    #define REXYZ_LEVEL_CORNERS_INSET 30    // (mm) An inset for corner leveling

    #define REXYZ_GRID_MAX_POINTS_X 3
    #define REXYZ_GRID_MAX_POINTS_Y 3
#endif
#if defined(REXYZ_4MAX)
    #define REXYZ_X_BED_SIZE 216
    #define REXYZ_Y_BED_SIZE 216
    #define REXYZ_X_MIN_POS -5
    #define REXYZ_Y_MIN_POS 0
    #define REXYZ_Z_MIN_POS 0
    #define REXYZ_X_MAX_POS 216
    #define REXYZ_Y_MAX_POS 216
    #define REXYZ_Z_MAX_POS 296

    #define REXYZ_LEVEL_CORNERS_INSET 20   // (mm) An inset for corner leveling
    #define REXYZ_LEVEL_CENTER_TOO         // Move to the center after the last corner

    #define REXYZ_GRID_MAX_POINTS_X 3
    #define REXYZ_GRID_MAX_POINTS_Y 3
#endif
#if defined(REXYZ_S33) || defined(REXYZ_N3)
    #define REXYZ_X_BED_SIZE 300
    #define REXYZ_Y_BED_SIZE 300
    #define REXYZ_X_MIN_POS 0
    #define REXYZ_Y_MIN_POS 0
    #define REXYZ_Z_MIN_POS 0
    #define REXYZ_X_MAX_POS 300
    #define REXYZ_Y_MAX_POS 300
    #define REXYZ_Z_MAX_POS 365

    #define REXYZ_LEVEL_CORNERS_INSET 30    // (mm) An inset for corner leveling

    #define REXYZ_GRID_MAX_POINTS_X 5
    #define REXYZ_GRID_MAX_POINTS_Y 5
#endif
#if defined(REXYZ_A8P)
    #define REXYZ_X_BED_SIZE 300
    #define REXYZ_Y_BED_SIZE 300
    #define REXYZ_X_MIN_POS -20
    #define REXYZ_Y_MIN_POS -5
    #define REXYZ_Z_MIN_POS 0
    #define REXYZ_X_MAX_POS 300
    #define REXYZ_Y_MAX_POS 300
    #define REXYZ_Z_MAX_POS 350

    #define REXYZ_LEVEL_CORNERS_INSET 30   // (mm) An inset for corner leveling
    //#define REXYZ_LEVEL_CENTER_TOO         // Move to the center after the last corner

    #define REXYZ_GRID_MAX_POINTS_X 4
    #define REXYZ_GRID_MAX_POINTS_Y 4
#endif
#if defined(REXYZ_A1) || defined(REXYZ_A2)
    #define REXYZ_X_BED_SIZE 180
    #define REXYZ_Y_BED_SIZE 180
    #define REXYZ_X_MIN_POS 0
    #define REXYZ_Y_MIN_POS 0
    #define REXYZ_Z_MIN_POS 0
    #define REXYZ_X_MAX_POS 180
    #define REXYZ_Y_MAX_POS 180
    #define REXYZ_Z_MAX_POS 180

    #define REXYZ_LEVEL_CORNERS_INSET 20   // (mm) An inset for corner leveling

    #define REXYZ_GRID_MAX_POINTS_X 3
    #define REXYZ_GRID_MAX_POINTS_Y 3
#endif

//===========================================================================
//============================= LCD & Sound Settings ============================
//===========================================================================

#if defined(REXYZ_LCD12864)
    #define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER    

    #define REXYZ_LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
    #define REXYZ_LCD_FEEDBACK_FREQUENCY_HZ 5000

    #define ENCODER_RATE_MULTIPLIER
    #define REXYZ_ENCODER_10X_STEPS_PER_SEC 30  // (steps/s) Encoder rate for 10x speed
    #define REXYZ_ENCODER_100X_STEPS_PER_SEC 80  // (steps/s) Encoder rate for 100x speed

#endif
#if defined(REXYZ_LCD2004)
    #define REPRAP_DISCOUNT_SMART_CONTROLLER

    #define REXYZ_LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
    #define REXYZ_LCD_FEEDBACK_FREQUENCY_HZ 5000

    #define ENCODER_RATE_MULTIPLIER
    #define REXYZ_ENCODER_10X_STEPS_PER_SEC 30  // (steps/s) Encoder rate for 10x speed
    #define REXYZ_ENCODER_100X_STEPS_PER_SEC 80  // (steps/s) Encoder rate for 100x speed

#endif
#if defined(REXYZ_4MAX)

    #define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER    

    #define REXYZ_LCD_FEEDBACK_FREQUENCY_DURATION_MS 70
    #define REXYZ_LCD_FEEDBACK_FREQUENCY_HZ 4000

    #define ENCODER_RATE_MULTIPLIER
    #define REXYZ_ENCODER_10X_STEPS_PER_SEC 75  // (steps/s) Encoder rate for 10x speed
    #define REXYZ_ENCODER_100X_STEPS_PER_SEC 160  // (steps/s) Encoder rate for 100x speed

#endif
#if defined(REXYZ_A8P)
    #define ANET_FULL_GRAPHICS_LCD    

    #define REXYZ_LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
    #define REXYZ_LCD_FEEDBACK_FREQUENCY_HZ 5000

    #define ENCODER_RATE_MULTIPLIER
    #define REXYZ_ENCODER_10X_STEPS_PER_SEC 30  // (steps/s) Encoder rate for 10x speed
    #define REXYZ_ENCODER_100X_STEPS_PER_SEC 80  // (steps/s) Encoder rate for 100x speed

#endif

#if defined(REXYZ_A1) || defined(REXYZ_A2)
  //#define MKS_ROBIN_TFT
  #define FSMC_GRAPHICAL_TFT
  //#define PRINTER_EVENT_LEDS
  #define TOUCH_BUTTONS
    #define REXYZ_LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
    #define REXYZ_LCD_FEEDBACK_FREQUENCY_HZ 5000
#endif


#if defined(REXYZ_TOUCH_UI)
    //#define EXTENSIBLE_UI
    //#define EXTUI_EXAMPLE
#endif


//===========================================================================
//============================= Feature Settings ============================
//===========================================================================
#if defined(EXTENSIBLE_UI) || defined(REXYZ_A1) || defined(REXYZ_A2)
#else
  #define POWER_LOSS_RECOVERY
#endif

#define FILAMENT_RUNOUT_SENSOR

//===========================================================================
//============================= Probe Settings ==============================
//===========================================================================
//
//  1. D = Direct Extruder
//  2. D = Direct Drive
//  3. M = Manual Probe
//     P8= Proximity 8mm  
//     F = Fixed Probe
//     T = 3D Touch Probe
//  Yg pernah terjual :
//    REXYZ_MK8_MANUAL_PROBE - Yohanes
//    REXYZ_MK8_MULTI_FIXPROBE_PROXYMITY - Panji Enjoy dll
//    REXYZ_MK8_MULTI_FIXPROBE_BLTOUCH
//  
//  Tidak pernah terjual :  
//    REXYZ_MK8_MULTI_FIXPROBE_MANUAL
//    REXYZ_MK8_PROXIMITY_8MM

#if defined(REXYZ_NO_ABL)
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "BD"
#endif
#if defined(REXYZ_MK8_MANUAL_PROBE)
    #define AUTO_BED_LEVELING_BILINEAR
    #define LCD_BED_LEVELING
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDM"
    #define REXYZ_PROBE_MANUALLY
    #define REXYZ_MANUAL_PROBE_START_Z 0
    #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 0
    #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER 0 
    #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER 0   
#endif
#if defined(REXYZ_MK8_PROXIMITY_8MM)
    #define AUTO_BED_LEVELING_BILINEAR
    #define LCD_BED_LEVELING
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDP8"
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_Z_MIN_PROBE_PIN X_MIN_PIN
    #define REXYZ_FIX_MOUNTED_PROBE
    #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 28.5
    #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER -23.5 
    #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER -0.5   
#endif
#if defined(REXYZ_MK8_MULTI_FIXPROBE_PROXIMITY) || defined(REXYZ_MK8_MULTI_FIXPROBE_MANUAL) 
    #define AUTO_BED_LEVELING_BILINEAR
    #define LCD_BED_LEVELING
    #ifdef REXYZ_FILAMENT_MOTION_DETECTOR
      #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDFS"
    #else
      #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDF"
    #endif
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_Z_MIN_PROBE_PIN X_MIN_PIN
    #define REXYZ_FIX_MOUNTED_PROBE
    #define REXYZ_PAUSE_BEFORE_DEPLOY_STOW
    #define REXYZ_PAUSE_PROBE_DEPLOY_WHEN_TRIGGERED
    #if defined(REXYZ_MK8_MULTI_FIXPROBE_PROXIMITY) 
      #define REXYZ_DEFAULT_PROBE 1
      #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 29
      #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER -20
      #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER -0.5
    #endif   
    #if defined(REXYZ_MK8_MULTI_FIXPROBE_MANUAL) 
      #define REXYZ_DEFAULT_PROBE 2
      #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 23
      #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER -27 
      #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER -2.7
    #endif   
#endif
#if defined(REXYZ_MK8_MULTI_FIXPROBE_BLTOUCH)  
    #define AUTO_BED_LEVELING_BILINEAR
    #define LCD_BED_LEVELING
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDT"
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_Z_MIN_PROBE_PIN X_MIN_PIN
    #define REXYZ_PAUSE_BEFORE_DEPLOY_STOW
    #define REXYZ_PAUSE_PROBE_DEPLOY_WHEN_TRIGGERED

      #define BLTOUCH
      #define BLTOUCH_FORCE_SW_MODE
      #define BLTOUCH_SET_5V_MODE
      //#define BLTOUCH_LCD_VOLTAGE_MENU
      #define NUM_SERVOS 1
      #define REXYZ_DEFAULT_PROBE 1
      #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 25
      #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER -23
      #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER -3
#endif

//===========================================================================
//============================= Filament Sensor Settings ====================
//===========================================================================

#if defined(REXYZ_FILAMENT_MOTION_DETECTOR)
    #define FILAMENT_RUNOUT_DISTANCE_MM 5
    //#define REXYZ_FILAMENT_RUNOUT_SCRIPT "M25 P\nM24"
    #define REXYZ_FILAMENT_RUNOUT_SCRIPT "M600"
    #define FILAMENT_MOTION_SENSOR
    #define REXYZ_FILAMENT_DISTANCE_PER_MOTION 0.894
#else
    #define REXYZ_FILAMENT_RUNOUT_SCRIPT "M600"
#endif

//===========================================================================
//============================= Rexyz Machine Type ====================
//===========================================================================
// maks 7 char because used for marking.

#define REXYZ_MACHINE_TYPE REXYZ_MACHINE_FRAME_TYPE REXYZ_MACHINE_TOOLHEAD_TYPE

#endif /* CONDITIONALS_REXYZ */
