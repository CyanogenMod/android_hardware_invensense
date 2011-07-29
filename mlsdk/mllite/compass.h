/*
 $License:
   Copyright 2011 InvenSense, Inc.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
  $
 */
/*******************************************************************************
 *
 * $Id: compass.h 5629 2011-06-11 03:13:08Z mcaramello $
 *
 *******************************************************************************/

#ifndef COMPASS_H
#define COMPASS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mltypes.h"
#include "mpu.h"
#ifdef INV_INCLUDE_LEGACY_HEADERS
#include "compass_legacy.h"
#endif
    /* ------------ */
    /* - Defines. - */
    /* ------------ */

    /* --------------- */
    /* - Structures. - */
    /* --------------- */

    /* --------------------- */
    /* - Function p-types. - */
    /* --------------------- */

    unsigned char inv_compass_present(void);
    unsigned char inv_get_compass_slave_addr(void);
    inv_error_t inv_get_compass_data(long *data);
    inv_error_t inv_set_compass_bias(long *bias);
    unsigned short inv_get_compass_id(void);
    inv_error_t inv_set_compass_offset(void);
    inv_error_t inv_compass_check_range(void);
    inv_error_t inv_compass_write_reg(unsigned char reg, unsigned char val);
    inv_error_t inv_compass_read_reg(unsigned char reg, unsigned char *val);
    inv_error_t inv_compass_read_scale(long *val);

#ifdef __cplusplus
}
#endif
#endif                          // COMPASS_H
