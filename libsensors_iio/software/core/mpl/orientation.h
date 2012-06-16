/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */
#ifndef MLDMP_ORIENTATION_H__
#define MLDMP_ORIENTATION_H__

#include "mltypes.h"
/*******************************************************************************/
/*    Orientations                                                             */
/*******************************************************************************/

#define INV_X_UP                          0x01
#define INV_X_DOWN                        0x02
#define INV_Y_UP                          0x04
#define INV_Y_DOWN                        0x08
#define INV_Z_UP                          0x10
#define INV_Z_DOWN                        0x20
#define INV_ORIENTATION_ALL               0x3F

#ifdef __cplusplus
extern "C" {
#endif

    inv_error_t inv_enable_orientation(void);
    inv_error_t inv_disable_orientation(void);
    inv_error_t inv_set_orientation(int orientation);
    inv_error_t inv_set_orientation_cb(void (*callback)(unsigned short));
    inv_error_t inv_get_orientation(int *orientation);
    inv_error_t inv_get_orientation_state(int * state);
    inv_error_t inv_set_orientation_interrupt(unsigned char on);
    inv_error_t inv_set_orientation_thresh(float angle,
                                       float hysteresis,
                                       unsigned long time, 
                                       unsigned int axis);

#ifdef __cplusplus
}
#endif

#endif // MLDMP_ORIENTATION_H__
