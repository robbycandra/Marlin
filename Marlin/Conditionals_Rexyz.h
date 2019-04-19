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
 *   2. GENERAL CONFIGURATION
 *   3. EXPERIMENTAL FEATURES
 *   4. CUSTOMIZED VERSION STRING AND URL
 *   5. MOTHERBOARD AND PIN CONFIGURATION
 *   6. HOMING & AXIS DIRECTIONS
 *   7. STEPPER INACTIVITY TIMEOUT
 *   8. AUTOLEVELING / BED PROBE
 *   9. COMMON TOOLHEADS PARAMETERS
 *  10. MINI TOOLHEADS
 *  11. TAZ 4/5/6 TOOLHEADS
 *  12. UNIVERSAL TOOLHEADS
 *  13. TAZ 7 TOOLHEADS
 *  14. AUTO-CALIBRATION (BACKLASH AND NOZZLE OFFSET)
 *  15. TEMPERATURE SETTINGS
 *  16. HEATING ELEMENTS
 *  17. COOLING FAN CONFIGURATION
 *  18. AXIS TRAVEL LIMITS
 *  19. ENDSTOP CONFIGURATION
 *  20. FILAMENT CONFIGURATION (LIN_ADVANCE/RUNOUT)
 *  21. MOTOR DRIVER TYPE
 *  22. TRINAMIC DRIVER CONFIGURATION
 *  23. TRINAMIC SENSORLESS HOMING
 *  24. ADVANCED PAUSE / FILAMENT CHANGE
 *  25. WIPER PAD
 *  26. REWIPE FUNCTIONALITY
 *  27. PROBE QUALITY CHECK
 *  28. BACKLASH COMPENSATION
 *  29. MOTOR CURRENTS
 *  30. ACCELERATION, FEEDRATES AND XYZ MOTOR STEPS
 *  31. LCD OPTIONS
 *  32. CUSTOM SPLASH SCREEN
 *  33. Z-OFFSET AUTO-SAVE
 *
 */

#define REXYZ_FW_VERSION ".3" // Change this with each update
#define REXYZ_STRING_DISTRIBUTION_DATE "2019-04-19"
#define REXYZ_DEFAULT_MACHINE_UUID "cede2a2f-41a2-4748-9b12-c55c62f367ff"
#define REXYZ_SOURCE_CODE_URL "http://rajawali3d.com"
#define REXYZ_WEBSITE_URL "http://rajawali3d.com"

#if ( \
    !defined(REXYZ_S22) && \
    !defined(REXYZ_S33) && \
    !defined(REXYZ_4MAX) \
)
    #error Must specify printer model. Please see "Configuration_Rexyz.h" for directions.
#endif

#if ( \
    !defined(REXYZ_MK8_MANUAL_PROBE) && \
    !defined(REXYZ_MK8_ALLENKEY_MANUAL) && \
    !defined(REXYZ_MK8_PROXIMITY_8MM) && \
    !defined(REXYZ_MK8_PROXIMITY_4MM) \
)
    #error Must specify toolhead model. Please see "Configuration_REXYZ.h" for directions.
#endif

/*********************** PRINTER MODEL CHARACTERISTICS **************************/

#if defined(REXYZ_S22)
    #define REXYZ_MACHINE_FRAME_TYPE "S22"
    #define REXYZ_X_BED_SIZE 220
    #define REXYZ_Y_BED_SIZE 220
    #define REXYZ_X_MAX_SIZE 223
    #define REXYZ_Y_MAX_SIZE 220
    #define REXYZ_Z_MAX_SIZE 255
#endif
#if defined(REXYZ_S33)
    #define REXYZ_MACHINE_FRAME_TYPE "S33"
    #define REXYZ_X_BED_SIZE 300
    #define REXYZ_Y_BED_SIZE 300
    #define REXYZ_X_MAX_SIZE 300
    #define REXYZ_Y_MAX_SIZE 300
    #define REXYZ_Z_MAX_SIZE 363
#endif
#if defined(REXYZ_4MAX)
#endif

#if defined(REXYZ_MK8_MANUAL_PROBE)
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDM"
    #define REXYZ_PROBE_MANUALLY
    #define REXYZ_MANUAL_PROBE_START_Z 0
    #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 0
    #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER 0 
    #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER 0   
#endif
#if defined(REXYZ_MK8_ALLENKEY_MANUAL)
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDA"
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_Z_MIN_PROBE_PIN X_MIN_PIN
    #define REXYZ_FIX_MOUNTED_PROBE
    #define REXYZ_PAUSE_BEFORE_DEPLOY_STOW
    #define REXYZ_PAUSE_PROBE_DEPLOY_WHEN_TRIGGERED
    #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 23.5
    #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER -20.4 
    #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER -4.2   
#endif
#if defined(REXYZ_MK8_PROXIMITY_8MM)
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDP8"
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_Z_MIN_PROBE_PIN X_MIN_PIN
    #define REXYZ_FIX_MOUNTED_PROBE
    #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 28.5
    #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER -23.5 
    #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER -0.2   
#endif
#if defined(REXYZ_MK8_PROXIMITY_4MM)
    #define REXYZ_MACHINE_TOOLHEAD_TYPE "DDP4"
    #define REXYZ_USE_XMIN_PLUG
    #define REXYZ_Z_MIN_PROBE_PIN X_MIN_PIN
    #define REXYZ_FIX_MOUNTED_PROBE
    #define REXYZ_X_PROBE_OFFSET_FROM_EXTRUDER 29
    #define REXYZ_Y_PROBE_OFFSET_FROM_EXTRUDER -20.13 
    #define REXYZ_Z_PROBE_OFFSET_FROM_EXTRUDER -0.2   
#endif

#define REXYZ_MACHINE_TYPE REXYZ_MACHINE_FRAME_TYPE REXYZ_MACHINE_TOOLHEAD_TYPE

#endif /* CONDITIONALS_REXYZ */
