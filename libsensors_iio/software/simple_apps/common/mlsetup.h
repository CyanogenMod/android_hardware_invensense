/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */
 
/*******************************************************************************
 *
 * $Id: mlsetup.h 6101 2011-09-29 00:30:33Z kkatingari $
 *
 *******************************************************************************/

#ifndef MLSETUP_H
#define MLSETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "linux/mpu.h"
#include "mltypes.h"

    enum mpu_platform_id {
        PLATFORM_ID_INVALID = ID_INVALID, // 0
        PLATFORM_ID_MSB,              // (0x0001) MSB (Multi sensors board)
        PLATFORM_ID_ST_6AXIS,         // (0x0002) 6 Axis with ST accelerometer
        PLATFORM_ID_DONGLE,           // (0x0003) 9 Axis USB dongle with
        PLATFORM_ID_MANTIS_PROTOTYPE, // (0x0004) Mantis prototype board
        PLATFORM_ID_MANTIS_MSB,       // (0x0005) MSB with Mantis
        PLATFORM_ID_MANTIS_USB_DONGLE,// (0x0006) Mantis and AKM on USB dongle.
        PLATFORM_ID_MSB_10AXIS,       // (0x0007) MSB with pressure sensor
        PLATFORM_ID_DRAGON_PROTOTYPE, // (0x0008) Dragon prototype board
        PLATFORM_ID_MSB_V2,           // (0x0009) Version 2 MSB
        PLATFORM_ID_MSB_V2_MANTIS,    // (0x000A) Version 2 MSB with mantis
        PLATFORM_ID_MANTIS_EVB,       // (0x000B) Mantis EVB (shipped to cust.)
        PLATFORM_ID_DRAGON_USB_DONGLE,// (0x000C) Dragon USB Dongle with Mantis Rev C
        PLATFORM_ID_MSB_EVB,          // (0X000D) MSB with 3050. 
        PLATFORM_ID_SPIDER_PROTOTYPE,
        PLATFORM_ID_MSB_V3,

        NUM_PLATFORM_IDS
    };
    // Main entry APIs
inv_error_t SetupPlatform(unsigned short platformId, 
                       unsigned short accelSelection, 
                       unsigned short compassSelection);

#ifdef __cplusplus
}
#endif

#endif /* MLSETUP_H */
