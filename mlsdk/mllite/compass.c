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
 * $Id: compass.c 5641 2011-06-14 02:10:02Z mcaramello $
 *
 *******************************************************************************/

/** 
 *  @defgroup COMPASSDL 
 *  @brief  Motion Library - Compass Driver Layer.
 *          Provides the interface to setup and handle an compass
 *          connected to either the primary or the seconday I2C interface 
 *          of the gyroscope.
 *
 *  @{
 *      @file   compass.c
 *      @brief  Compass setup and handling methods.
 */

/* ------------------ */
/* - Include Files. - */
/* ------------------ */

#include <string.h>

#include "compass.h"

#include "ml.h"
#include "mlinclude.h"
#include "dmpKey.h"
#include "mlFIFO.h"
#include "mldl.h"
#include "mldl_cfg.h"
#include "mlMathFunc.h"
#include "mlsl.h"
#include "mlos.h"

#include "log.h"
#undef MPL_LOG_TAG
#define MPL_LOG_TAG "MPL-compass"

#define COMPASS_DEBUG 0

/* --------------------- */
/* - Global Variables. - */
/* --------------------- */

/* --------------------- */
/* - Static Variables. - */
/* --------------------- */

/* --------------- */
/* - Prototypes. - */
/* --------------- */

/* -------------- */
/* - Externs.   - */
/* -------------- */

/* -------------- */
/* - Functions. - */
/* -------------- */

/** 
 *  @brief  Used to determine if a compass is
 *          configured and used by the MPL.
 *  @return INV_SUCCESS if the compass is present.
 */
unsigned char inv_compass_present(void)
{
    INVENSENSE_FUNC_START;
    struct mldl_cfg *mldl_cfg = inv_get_dl_config();
    if (NULL != mldl_cfg->compass &&
        NULL != mldl_cfg->compass->resume &&
        mldl_cfg->requested_sensors & INV_THREE_AXIS_COMPASS)
        return TRUE;
    else
        return FALSE;
}

/**
 *  @brief   Query the compass slave address.
 *  @return  The 7-bit compass slave address.
 */
unsigned char inv_get_compass_slave_addr(void)
{
    INVENSENSE_FUNC_START;
    struct mldl_cfg *mldl_cfg = inv_get_dl_config();
    if (NULL != mldl_cfg->pdata)
        return mldl_cfg->pdata->compass.address;
    else
        return 0;
}

/**
 *  @brief   Get the ID of the compass in use.
 *  @return  ID of the compass in use.
 */
unsigned short inv_get_compass_id(void)
{
    INVENSENSE_FUNC_START;
    struct mldl_cfg *mldl_cfg = inv_get_dl_config();
    if (NULL != mldl_cfg->compass) {
        return mldl_cfg->compass->id;
    }
    return ID_INVALID;
}

/**
 *  @brief  Get a sample of compass data from the device.
 *  @param  data
 *              the buffer to store the compass raw data for
 *              X, Y, and Z axes.
 *  @return INV_SUCCESS or a non-zero error code.
 */
inv_error_t inv_get_compass_data(long *data)
{
    inv_error_t result;
    int ii;
    struct mldl_cfg *mldl_cfg = inv_get_dl_config();

    unsigned char *tmp = inv_obj.compass_raw_data;

    if (mldl_cfg->compass->read_len > sizeof(inv_obj.compass_raw_data)) {
        LOG_RESULT_LOCATION(INV_ERROR_INVALID_CONFIGURATION);
        return INV_ERROR_INVALID_CONFIGURATION;
    }

    if (mldl_cfg->pdata->compass.bus == EXT_SLAVE_BUS_PRIMARY ||
        !(mldl_cfg->requested_sensors & INV_DMP_PROCESSOR)) {
        /*--- read the compass sensor data.
          The compass read function may return an INV_ERROR_COMPASS_* errors
          when the data is not ready (read/refresh frequency mismatch) or 
          the internal data sampling timing of the device was not respected. 
          Returning the error code will make the sensor fusion supervisor 
          ignore this compass data sample. ---*/
        result = (inv_error_t) inv_mpu_read_compass(mldl_cfg,
                                                    inv_get_serial_handle(),
                                                    inv_get_serial_handle(),
                                                    tmp);
        if (result) {
            if (COMPASS_DEBUG) {
                MPL_LOGV("inv_mpu_read_compass returned %d\n", result);
            }
            return result;
        }
        for (ii = 0; ii < 3; ii++) {
            if (EXT_SLAVE_BIG_ENDIAN == mldl_cfg->compass->endian)
                data[ii] =
                    ((long)((signed char)tmp[2 * ii]) << 8) + tmp[2 * ii + 1];
            else
                data[ii] =
                    ((long)((signed char)tmp[2 * ii + 1]) << 8) + tmp[2 * ii];
        }
    } else {
#if defined CONFIG_MPU_SENSORS_MPU6050A2 ||             \
    defined CONFIG_MPU_SENSORS_MPU6050B1
        result = inv_get_external_sensor_data(data, 3);
        if (result) {
            LOG_RESULT_LOCATION(result);
            return result;
        }
#if defined CONFIG_MPU_SENSORS_MPU6050A2
        {
            static unsigned char first = TRUE;
            // one-off write to AKM
            if (first) {
                unsigned char regs[] = {
                    // beginning Mantis register for one-off slave R/W
                    MPUREG_I2C_SLV4_ADDR,
                    // the slave to write to
                    mldl_cfg->pdata->compass.address,
                    // the register to write to
                    /*mldl_cfg->compass->trigger->reg */ 0x0A,
                    // the value to write
                    /*mldl_cfg->compass->trigger->value */ 0x01,
                    // enable the write
                    0xC0
                };
                result =
                    inv_serial_write(inv_get_serial_handle(), mldl_cfg->addr,
                                     ARRAY_SIZE(regs), regs);
                first = FALSE;
            } else {
                unsigned char regs[] = {
                    MPUREG_I2C_SLV4_CTRL,
                    0xC0
                };
                result =
                    inv_serial_write(inv_get_serial_handle(), mldl_cfg->addr,
                                     ARRAY_SIZE(regs), regs);
            }
        }
#endif
#else
        return INV_ERROR_INVALID_CONFIGURATION;
#endif                          // CONFIG_MPU_SENSORS_xxxx
    }
    data[0] = inv_q30_mult(data[0], inv_obj.compass_asa[0]);
    data[1] = inv_q30_mult(data[1], inv_obj.compass_asa[1]);
    data[2] = inv_q30_mult(data[2], inv_obj.compass_asa[2]);

    return INV_SUCCESS;
}

/** 
 *  @brief  Sets the compass bias.
 *  @param  bias 
 *              Compass bias, length 3. Scale is micro Tesla's * 2^16. 
 *              Frame is mount frame which may be different from body frame.
 *  @return INV_SUCCESS = 0 if successful. A non-zero error code otherwise.
 */
inv_error_t inv_set_compass_bias(long *bias)
{
    inv_error_t result = INV_SUCCESS;
    long biasC[3];
    struct mldl_cfg *mldlCfg = inv_get_dl_config();

    inv_obj.compass_bias[0] = bias[0];
    inv_obj.compass_bias[1] = bias[1];
    inv_obj.compass_bias[2] = bias[2];

    // Find Bias in units of the raw data scaled by 2^16 in chip mounting frame
    biasC[0] =
        (long)(bias[0] * (1LL << 30) / inv_obj.compass_sens) +
        inv_obj.init_compass_bias[0] * (1L << 16);
    biasC[1] =
        (long)(bias[1] * (1LL << 30) / inv_obj.compass_sens) +
        inv_obj.init_compass_bias[1] * (1L << 16);
    biasC[2] =
        (long)(bias[2] * (1LL << 30) / inv_obj.compass_sens) +
        inv_obj.init_compass_bias[2] * (1L << 16);

    if (inv_dmpkey_supported(KEY_CPASS_BIAS_X)) {
        unsigned char reg[4];
        long biasB[3];
        signed char *orC = mldlCfg->pdata->compass.orientation;

        // Now transform to body frame
        biasB[0] = biasC[0] * orC[0] + biasC[1] * orC[1] + biasC[2] * orC[2];
        biasB[1] = biasC[0] * orC[3] + biasC[1] * orC[4] + biasC[2] * orC[5];
        biasB[2] = biasC[0] * orC[6] + biasC[1] * orC[7] + biasC[2] * orC[8];

        result =
            inv_set_mpu_memory(KEY_CPASS_BIAS_X, 4,
                               inv_int32_to_big8(biasB[0], reg));
        result =
            inv_set_mpu_memory(KEY_CPASS_BIAS_Y, 4,
                               inv_int32_to_big8(biasB[1], reg));
        result =
            inv_set_mpu_memory(KEY_CPASS_BIAS_Z, 4,
                               inv_int32_to_big8(biasB[2], reg));
    }
    return result;
}

/**
 *  @brief  Write a single register on the compass slave device, regardless
 *          of the bus it is connected to and the MPU's configuration.
 *  @param  reg
 *              the register to write to on the slave compass device.
 *  @param  val
 *              the value to write.
 *  @return INV_SUCCESS = 0 if successful. A non-zero error code otherwise.
 */
inv_error_t inv_compass_write_reg(unsigned char reg, unsigned char val)
{
    struct ext_slave_config config;
    unsigned char data[2];
    inv_error_t result;

    data[0] = reg;
    data[1] = val;

    config.key = MPU_SLAVE_WRITE_REGISTERS;
    config.len = 2;
    config.apply = TRUE;
    config.data = data;

    result = inv_mpu_config_compass(inv_get_dl_config(),
                                    inv_get_serial_handle(),
                                    inv_get_serial_handle(), &config);
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }
    return result;
}

/**
 *  @brief  Read values from the compass slave device registers, regardless
 *          of the bus it is connected to and the MPU's configuration.
 *  @param  reg
 *              the register to read from on the slave compass device.
 *  @param  val
 *              a buffer of 3 elements to store the values read from the 
 *              compass device.
 *  @return INV_SUCCESS = 0 if successful. A non-zero error code otherwise.
 */
inv_error_t inv_compass_read_reg(unsigned char reg, unsigned char *val)
{
    struct ext_slave_config config;
    unsigned char data[2];
    inv_error_t result;

    data[0] = reg;

    config.key = MPU_SLAVE_READ_REGISTERS;
    config.len = 2;
    config.apply = TRUE;
    config.data = data;

    result = inv_mpu_get_compass_config(inv_get_dl_config(),
                                        inv_get_serial_handle(),
                                        inv_get_serial_handle(), &config);
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }
    *val = data[1];
    return result;
}

/**
 *  @brief  Read values from the compass slave device scale registers, regardless
 *          of the bus it is connected to and the MPU's configuration.
 *  @param  reg
 *              the register to read from on the slave compass device.
 *  @param  val
 *              a buffer of 3 elements to store the values read from the 
 *              compass device.
 *  @return INV_SUCCESS = 0 if successful. A non-zero error code otherwise.
 */
inv_error_t inv_compass_read_scale(long *val)
{
    struct ext_slave_config config;
    unsigned char data[3];
    inv_error_t result;

    config.key = MPU_SLAVE_READ_SCALE;
    config.len = 3;
    config.apply = TRUE;
    config.data = data;

    result = inv_mpu_get_compass_config(inv_get_dl_config(),
                                        inv_get_serial_handle(),
                                        inv_get_serial_handle(), &config);
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }
    val[0] = ((data[0] - 128) << 22) + (1L << 30);
    val[1] = ((data[1] - 128) << 22) + (1L << 30);
    val[2] = ((data[2] - 128) << 22) + (1L << 30);
    return result;
}

/**
 * @}
 */