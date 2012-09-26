/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */

/******************************************************************************
 *
 * $Id:$
 *
 *****************************************************************************/

#ifndef MLDMP_PROG_NO_MOTION_H__
#define MLDMP_PROG_NO_MOTION_H__

#include "mltypes.h"

#define PROG_NO_MOTION  1
#define PROG_MOTION     2

#ifdef __cplusplus
extern "C" {
#endif

/* APIs */
inv_error_t inv_enable_prog_no_motion(void);
inv_error_t inv_disable_prog_no_motion(void);

/* internal use */
int inv_get_prog_no_motion_enabled(void);
void inv_get_prog_no_motion_bias_changed(void);
int inv_get_prog_no_motion_state(void);

#ifdef __cplusplus
}
#endif


#endif // MLDMP_PROG_NO_MOTION_H__
