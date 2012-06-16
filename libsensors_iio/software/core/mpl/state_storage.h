/*
 $License:
    Copyright (C) 2011-2012 InvenSense Corporation, All Rights Reserved.
    See included License.txt for License information.
 $
 */
#ifndef INV_STATE_STORAGE_H__
#define INV_STATE_STORAGE_H__

#include "mltypes.h"

#ifdef __cplusplus
extern "C" {
#endif

inv_error_t inv_store_data(const void *data, size_t size, unsigned long module,
                           unsigned long version);
inv_error_t inv_load_data(void *data, size_t size, unsigned long module,
                          unsigned long version);

#ifdef __cplusplus
}
#endif

#endif // INV_STATE_STORAGE_H__
