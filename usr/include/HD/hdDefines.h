/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

    hdDefines.h

Description: 

    Params, definitions, errors, typedefs, return codes.

******************************************************************************/

#ifndef HD_DEFINES_H_DEFINE
#define HD_DEFINES_H_DEFINE

#include <limits.h>

#ifdef __cplusplus
extern "C" 
{
#endif

typedef unsigned int HDuint;
typedef unsigned char HDboolean;
typedef unsigned long HDulong;
typedef unsigned short HDushort;
typedef int HDint;
typedef float HDfloat;
typedef double HDdouble;
typedef long HDlong;
typedef char HDchar;

typedef unsigned int HDerror;
typedef unsigned int HDenum;
typedef const char *HDstring;
typedef unsigned int HHD;

typedef struct
{
    HDerror errorCode; /* The HD_ error code */
    int internalErrorCode; /* The original internal device-generated error */
    HHD hHD; /* The handle of the current device when the error occurred */
} HDErrorInfo;

/* Boolean */
#define HD_TRUE                           1
#define HD_FALSE                          0

/******************************************************************************
 * Version information                                                        */

#define HD_VERSION_MAJOR_NUMBER            3
#define HD_VERSION_MINOR_NUMBER            00
#define HD_VERSION_BUILD_NUMBER            66

/* Version information end                                                    *
*******************************************************************************/


/******************************************************************************
 * Error codes                                                                */

#define HD_SUCCESS                         0x0000

/* Function errors */
#define HD_INVALID_ENUM                    0x0100
#define HD_INVALID_VALUE                   0x0101
#define HD_INVALID_OPERATION               0x0102
#define HD_INVALID_INPUT_TYPE              0x0103
#define HD_BAD_HANDLE                      0x0104

/* Force errors */
#define HD_WARM_MOTORS                     0x0200
#define HD_EXCEEDED_MAX_FORCE              0x0201
#define HD_EXCEEDED_MAX_FORCE_IMPULSE      0x0202
#define HD_EXCEEDED_MAX_VELOCITY           0x0203
#define HD_FORCE_ERROR                     0x0204

/* Device errors */
#define HD_DEVICE_FAULT                    0x0300
#define HD_DEVICE_ALREADY_INITIATED        0x0301
#define HD_COMM_ERROR                      0x0302
#define HD_COMM_CONFIG_ERROR               0x0303
#define HD_TIMER_ERROR                     0x0304

/* Haptic rendering context */
#define HD_ILLEGAL_BEGIN                   0x0400
#define HD_ILLEGAL_END                     0x0401
#define HD_FRAME_ERROR                     0x0402

/* Scheduler errors */
#define HD_INVALID_PRIORITY                0x0500
#define HD_SCHEDULER_FULL                  0x0501

/* Licensing erros */
#define HD_INVALID_LICENSE                 0x0600

/* Error codes end                                                           *
 *****************************************************************************/


/*****************************************************************************
 * Get parameter options                                                     */

/* Raw values */
#define HD_CURRENT_BUTTONS                 0x2000
#define HD_CURRENT_SAFETY_SWITCH           0x2001
#define HD_CURRENT_INKWELL_SWITCH          0x2002
#define HD_CURRENT_ENCODER_VALUES          0x2010
#define HD_CURRENT_PINCH_VALUE             0x2011
#define HD_LAST_PINCH_VALUE                0x2012
/* Cartesian space values */
#define HD_CURRENT_POSITION                0x2050
#define HD_CURRENT_VELOCITY                0x2051
#define HD_CURRENT_TRANSFORM               0x2052
#define HD_CURRENT_ANGULAR_VELOCITY        0x2053
#define HD_CURRENT_JACOBIAN                0x2054
/* Joint space values */
#define HD_CURRENT_JOINT_ANGLES            0x2100
#define HD_CURRENT_GIMBAL_ANGLES           0x2150

#define HD_LAST_BUTTONS                    0x2200
#define HD_LAST_SAFETY_SWITCH              0x2201
#define HD_LAST_INKWELL_SWITCH             0x2202
#define HD_LAST_ENCODER_VALUES             0x2210
#define HD_LAST_POSITION                   0x2250
#define HD_LAST_VELOCITY                   0x2251
#define HD_LAST_TRANSFORM                  0x2252
#define HD_LAST_ANGULAR_VELOCITY           0x2253
#define HD_LAST_JACOBIAN                   0x2254
#define HD_LAST_JOINT_ANGLES               0x2300
#define HD_LAST_GIMBAL_ANGLES              0x2350

/* Identification */
#define HD_VERSION                         0x2500
#define HD_DEVICE_MODEL_TYPE               0x2501
#define HD_DEVICE_DRIVER_VERSION           0x2502
#define HD_DEVICE_VENDOR                   0x2503
#define HD_DEVICE_SERIAL_NUMBER            0x2504
#define HD_DEVICE_FIRMWARE_VERSION         0x2505

/* Device hardware properties */
#define HD_MAX_WORKSPACE_DIMENSIONS        0x2550
#define HD_USABLE_WORKSPACE_DIMENSIONS     0x2551
#define HD_TABLETOP_OFFSET                 0x2552
#define HD_INPUT_DOF                       0x2553
#define HD_OUTPUT_DOF                      0x2554
#define HD_CALIBRATION_STYLE               0x2555

/* Device forces and measurements. */
#define HD_UPDATE_RATE                     0x2600
#define HD_INSTANTANEOUS_UPDATE_RATE       0x2601
#define HD_NOMINAL_MAX_STIFFNESS           0x2602
#define HD_NOMINAL_MAX_DAMPING             0x2609
#define HD_NOMINAL_MAX_FORCE               0x2603
#define HD_NOMINAL_MAX_CONTINUOUS_FORCE    0x2604
#define HD_MOTOR_TEMPERATURE               0x2605
#define HD_SOFTWARE_VELOCITY_LIMIT         0x2606
#define HD_SOFTWARE_FORCE_IMPULSE_LIMIT    0x2607
#define HD_FORCE_RAMPING_RATE              0x2608


#define HD_NOMINAL_MAX_TORQUE_STIFFNESS    0x2620
#define HD_NOMINAL_MAX_TORQUE_DAMPING      0x2621
#define HD_NOMINAL_MAX_TORQUE_FORCE        0x2622
#define HD_NOMINAL_MAX_TORQUE_CONTINUOUS_FORCE      0x2623

/* Cartesian space values */
#define HD_CURRENT_FORCE                   0x2700

/***************HD_CURRENT_TORQUE is deprecated,**************************
Please use HD_CURRENT_JOINT_TORQUE & HD_CURRENT_GIMBAL_TORQUE instead******/
#pragma deprecated (HD_CURRENT_TORQUE)
#define HD_CURRENT_TORQUE                  0x2701

#define HD_JOINT_ANGLE_REFERENCES          0x2702
/* Joint space values */
#define HD_CURRENT_JOINT_TORQUE            0x2703
#define HD_CURRENT_GIMBAL_TORQUE           0x2704

/* Motor space values */
/******HD_CURRENT_MOTOR_DAC_VALUES is deprecated,
Please use HD_CURRENT_JOINT_TORQUE instead******/
#pragma deprecated (HD_CURRENT_MOTOR_DAC_VALUES)
#define HD_CURRENT_MOTOR_DAC_VALUES        0x2750

#define HD_LAST_FORCE                      0x2800

/***************HD_LAST_TORQUE is deprecated,************************
Please use HD_LAST_JOINT_TORQUE & HD_LAST_GIMBAL_TORQUE instead******/
#pragma deprecated (HD_LAST_TORQUE)
#define HD_LAST_TORQUE                     0x2801

#define HD_LAST_JOINT_TORQUE               0x2802
#define HD_LAST_GIMBAL_TORQUE              0x2803

/******HD_LAST_MOTOR_DAC_VALUES is deprecated,
Please use HD_LAST_JOINT_TORQUE instead******/
#pragma deprecated (HD_LAST_MOTOR_DAC_VALUES)
#define HD_LAST_MOTOR_DAC_VALUES           0x2850

/* LED status light */
#define HD_USER_STATUS_LIGHT               0x2900

/* Get parameter options end                                                 *
 *****************************************************************************/


/*****************************************************************************
 * Set parameter options                                                     */

/*      HD_SOFTWARE_VELOCITY_LIMIT */
/*      HD_SOFTWARE_FORCE_IMPULSE_LIMIT */
/*      HD_FORCE_RAMPING_RATE */
/*      HD_CURRENT_FORCE */
/*      HD_CURRENT_TORQUE */   
/*      HD_CURRENT_JOINT_TORQUE */
/*      HD_CURRENT_GIMBAL_TORQUE */
/*      HD_CURRENT_MOTOR_DAC_VALUES */ 

/* Set parameter option end                                                  *
 *****************************************************************************/


/*****************************************************************************
 * Enable/Disable capabilities                                               */

#define HD_FORCE_OUTPUT                    0x4000 
#define HD_MAX_FORCE_CLAMPING              0x4001
#define HD_FORCE_RAMPING                   0x4002
#define HD_SOFTWARE_FORCE_LIMIT            0x4003
/*      HD_SOFTWARE_VELOCITY_LIMIT */
/*      HD_SOFTWARE_FORCE_IMPULSE_LIMIT */
#define HD_ONE_FRAME_LIMIT                 0x4004

/* Enable/Disable capabilities end                                           *
 *****************************************************************************/


/*****************************************************************************
 * Miscellaneous                                                             */

/* Scheduler priority ranges */
#define HD_MAX_SCHEDULER_PRIORITY          USHRT_MAX
#define HD_MIN_SCHEDULER_PRIORITY          0
#define HD_DEFAULT_SCHEDULER_PRIORITY      ((HD_MAX_SCHEDULER_PRIORITY + \
                                             HD_MIN_SCHEDULER_PRIORITY)/2)

/* Calibration return values */
#define HD_CALIBRATION_OK                  0x5000
#define HD_CALIBRATION_NEEDS_UPDATE        0x5001
#define HD_CALIBRATION_NEEDS_MANUAL_INPUT  0x5002

/* Calibration styles */
#define HD_CALIBRATION_ENCODER_RESET       (1 << 0)
#define HD_CALIBRATION_AUTO                (1 << 1)
#define HD_CALIBRATION_INKWELL             (1 << 2)

/* Button Masks */
#define HD_DEVICE_BUTTON_1                 (1 << 0)
#define HD_DEVICE_BUTTON_2                 (1 << 1)
#define HD_DEVICE_BUTTON_3                 (1 << 2)
#define HD_DEVICE_BUTTON_4                 (1 << 3)

/* Null device handle */
#define HD_INVALID_HANDLE                  0xFFFFFFFF

/* Used by device initialization. */
#define HD_DEFAULT_DEVICE                  NULL

/* LED status light states */
#define LED_MASK                           0x07
#define LED_STATUS_FAST_GRNYEL             0x00
#define LED_STATUS_SLOW_YEL                0x01
#define LED_STATUS_SLOW_GRN                0x02
#define LED_STATUS_FAST_GRN                0x03
#define LED_STATUS_SOLID_GRNYEL            0x04
#define LED_STATUS_SOLID_YEL               0x05
#define LED_STATUS_SOLID_GRN               0x06
#define LED_STATUS_FAST_YEL                0x07    

/* Miscellaneous end                                                         *
 *****************************************************************************/


#ifdef __cplusplus
}
#endif

#endif  /* HD_DEFINES_H_DEFINE */
/******************************************************************************/
