/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */

/******************************************************************************
 *
 * $Id$
 *
 *****************************************************************************/

#ifndef MLDMP_IN_USE_AUTO_CALIBRATION_H__
#define MLDMP_IN_USE_AUTO_CALIBRATION_H__

#include "mltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

inv_error_t inv_enable_in_use_auto_calibration(void);
inv_error_t inv_disable_in_use_auto_calibration(void);
inv_error_t inv_in_use_auto_calibration_is_enabled(unsigned char *is_enabled);
inv_error_t inv_init_in_use_auto_calibration(void);

#ifdef __cplusplus
}
#endif


#endif // MLDMP_IN_USE_AUTO_CALIBRATION_H__

