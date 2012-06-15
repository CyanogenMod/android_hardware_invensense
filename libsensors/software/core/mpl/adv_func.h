/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 */

/******************************************************************************
 *
 * $Id$
 *
 *****************************************************************************/

#ifndef MLDMP_ADVFUNC_H__
#define MLDMP_ADVFUNC_H__
#include "mltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

    float inv_compass_angle(const long *compass, const long *grav, const float *quat);
    inv_error_t inv_set_dmp_quaternion(long *q);

#ifdef __cplusplus
}
#endif


#endif // MLDMP_ADVFUNC_H__
