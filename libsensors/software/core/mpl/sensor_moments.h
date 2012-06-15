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

#ifndef MLDMP_SENSOR_MOMENTS_H__
#define MLDMP_SENSOR_MOMENTS_H__

#include "mltypes.h"

enum moment_ord {
	SECOND_ORD=0,
	THIRD_ORD,
	FOURTH_ORD,
	MAX_ORD
};

#ifdef __cplusplus
extern "C" {
#endif

	inv_error_t inv_enable_sm(void);
	inv_error_t inv_disable_sm(void);
	inv_error_t inv_sm_record_data(float sample, void *sensor);
	inv_error_t inv_sm_update_evt_act_state(int motion);
	void *inv_init_sm(enum moment_ord);
	float inv_sm_get_filtered_data(void *sensor);

#ifdef __cplusplus
}
#endif

#endif // MLDMP_SENSOR_MOMENTS_H__

