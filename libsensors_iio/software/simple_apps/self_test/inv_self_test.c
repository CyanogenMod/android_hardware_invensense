/**
 *  Self Test application for Invensense's MPU6050/MPU9150.
 */

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <features.h>
#include <dirent.h>
#include <string.h>
#include <poll.h>
#include <stddef.h>
#include <linux/input.h>
#include <time.h>
#include <linux/time.h>

#include "invensense.h"
#include "ml_math_func.h"
#include "storage_manager.h"
#include "ml_stored_data.h"

#ifndef ABS
#define ABS(x)(((x) >= 0) ? (x) : -(x))
#endif

/** Change this key if the data being stored by this file changes */
#define INV_DB_SAVE_KEY 53394

#define FALSE   0
#define TRUE    1

#define GYRO_PASS_STATUS_BIT    0x01
#define ACCEL_PASS_STATUS_BIT   0x02
#define COMPASS_PASS_STATUS_BIT 0x04

typedef union {
    long l; 
    int i;
} bias_dtype;

struct inv_sysfs_names_s {
    char *enable;
    char *power_state;
    char *self_test;
    char *temperature;
    char *accl_bias;
};

const struct inv_sysfs_names_s mpu= {
     /* MPU6050 & MPU9150 */
    .enable         = "/sys/class/invensense/mpu/enable",
    .power_state    = "/sys/class/invensense/mpu/power_state",
    .self_test      = "/sys/class/invensense/mpu/self_test",
    .temperature    = "/sys/class/invensense/mpu/temperature",
    .accl_bias      = "/sys/class/invensense/mpu/accl_bias"
};

struct inv_db_save_t {
    /** Compass Bias in Chip Frame in Hardware units scaled by 2^16 */
    long compass_bias[3];
    /** Gyro Bias in Chip Frame in Hardware units scaled by 2^16 */
    long gyro_bias[3];
    /** Temperature when *gyro_bias was stored. */
    long gyro_temp;
    /** Accel Bias in Chip Frame in Hardware units scaled by 2^16 */
    long accel_bias[3];
    /** Temperature when accel bias was stored. */
    long accel_temp;
    long gyro_temp_slope[3];
};

static struct inv_db_save_t save_data;

/** This function receives the data that was stored in non-volatile memory between power off */
static inv_error_t inv_db_load_func(const unsigned char *data)
{
    memcpy(&save_data, data, sizeof(save_data));
    return INV_SUCCESS;
}

/** This function returns the data to be stored in non-volatile memory between power off */
static inv_error_t inv_db_save_func(unsigned char *data)
{
    memcpy(data, &save_data, sizeof(save_data));
    return INV_SUCCESS;
}

int inv_sysfs_write(char *filename, long data)
{
    FILE *fp;
    int count;

    if (!filename)
        return -1;
    fp = fopen(filename, "w");
    if (!fp)
        return -errno;
    count = fprintf(fp, "%ld", data);
    fclose(fp);
    return count;
}

/**
 *  Main Self test 
 */
int main(int argc, char **argv)
{
    FILE *fptr;
    int self_test_status;
    inv_error_t result;
    bias_dtype gyro_bias[3];
    bias_dtype accel_bias[3];
    int axis = 0;
    size_t packet_sz;
    int axis_sign = 1;
    unsigned char *buffer;
    long timestamp;
    int temperature=0;

    // Initialize storage manager
    inv_init_storage_manager();

    // Clear out data.
    memset(&save_data, 0, sizeof(save_data));
    memset(gyro_bias,0, sizeof(gyro_bias));
    memset(accel_bias,0, sizeof(accel_bias));

    // Register packet to be saved.
    result = inv_register_load_store(inv_db_load_func, inv_db_save_func,
                                     sizeof(save_data),
                                     INV_DB_SAVE_KEY);

    // Power ON MPUxxxx chip
    if (inv_sysfs_write(mpu.power_state, 1) <0) {
        printf("ERR- Failed to set power state=1\n");
    } else {
        // Note: Driver turns on power automatically when self-test invoked
    }

    fptr = fopen(mpu.self_test, "r");
    if (fptr != NULL) {
        // Invoke self-test and read gyro bias
        fscanf(fptr, "%d,%d,%d,%d",
               &gyro_bias[0].i, &gyro_bias[1].i, &gyro_bias[2].i, &self_test_status);

        printf("Self-Test:Self test result- Gyro passed= %x, Accel passed= %x, Compass passed= %x\n",
                      (self_test_status & GYRO_PASS_STATUS_BIT), 
                      (self_test_status & ACCEL_PASS_STATUS_BIT) >>1,
                      (self_test_status & COMPASS_PASS_STATUS_BIT) >>2);
        printf("Self-Test:Gyro bias data[0..2] read from Driver= [%d %d %d]\n",gyro_bias[0].i, gyro_bias[1].i, gyro_bias[2].i);
        fclose(fptr);
        
        if (!(self_test_status & GYRO_PASS_STATUS_BIT)) {
            // Reset gyro bias data if gyro self-test failed
            memset(gyro_bias,0, sizeof(gyro_bias));
            printf("Self-Test:Failed Gyro self-test\n");
        }

        if (self_test_status & ACCEL_PASS_STATUS_BIT) {
            // Read Accel Bias
            fptr= fopen(mpu.accl_bias, "r");
            if (fptr != NULL) {
                fscanf(fptr, "%d,%d,%d", &accel_bias[0].i, &accel_bias[1].i, &accel_bias[2].i);
                printf("Self-Test:Accel bias data[0..2] read from Driver= [%d %d %d]\n", accel_bias[0].i, accel_bias[1].i, accel_bias[2].i);
                fclose(fptr);
            } else {
                printf("Self-Test:ERR-Couldn't read accel bias\n");
            }
        } else {
            memset(accel_bias,0, sizeof(accel_bias));
            printf("Self-Test:Failed Accel self-test\n");
        }

        // Read temperature
        if (self_test_status & (GYRO_PASS_STATUS_BIT|ACCEL_PASS_STATUS_BIT))
        {
            fptr= fopen(mpu.temperature, "r");
            if (fptr != NULL) {
                fscanf(fptr,"%d %ld", &temperature, &timestamp);
                fclose(fptr);
            } else {
                printf("Self-Test:ERR-Couldn't read temperature\n");
            }
        }
        
    } else {
        printf("Self-Test:ERR-Couldn't invoke self-test\n");
    }

    // When we read gyro bias, the bias is in raw units scaled by 1000.
    // We store the bias in raw units scaled by 2^16
    save_data.gyro_bias[0] = (long)(gyro_bias[0].l * 65536.f / 8000.f);
    save_data.gyro_bias[1] = (long)(gyro_bias[1].l * 65536.f / 8000.f);
    save_data.gyro_bias[2] = (long)(gyro_bias[2].l * 65536.f / 8000.f);

    // Save temperature @ time stored.  Temperature is in degrees Celsius scaled by 2^16
    save_data.gyro_temp = temperature * (1L << 16);
    save_data.accel_temp = save_data.gyro_temp; 

    // When we read accel bias, the bias is in raw units scaled by 1000.
    // and it contains the gravity vector.
    
    // Find the orientation of the device, by looking for gravity
    if (ABS(accel_bias[1].l) > ABS(accel_bias[0].l)) {
        axis = 1;
    }
    if (ABS(accel_bias[2].l) > ABS(accel_bias[axis].l)) {
        axis = 2;
    }
    if (accel_bias[axis].l < 0) {
        axis_sign = -1;
    }
    
    // Remove gravity, gravity in raw units should be 16384 for a 2g setting.
    // We read data scaled by 1000, so 
    accel_bias[axis].l -= axis_sign * 4096L * 1000L;

    // Convert scaling from raw units scaled by 1000 to raw scaled by 2^16
    save_data.accel_bias[0] = (long)(accel_bias[0].l * 65536.f / 1000.f * 4.f);
    save_data.accel_bias[1] = (long)(accel_bias[1].l * 65536.f / 1000.f * 4.f);
    save_data.accel_bias[2] = (long)(accel_bias[2].l * 65536.f / 1000.f * 4.f);

#if 1
    printf("Self-Test:Saved Accel bias[0..2]= [%ld %ld %ld]\n", save_data.accel_bias[0],
                    save_data.accel_bias[1], save_data.accel_bias[2]);
    printf("Self-Test:Saved Gyro bias[0..2]= [%ld %ld %ld]\n", save_data.gyro_bias[0],
                    save_data.gyro_bias[1], save_data.gyro_bias[2]);
    printf("Self-Test:Gyro temperature @ time stored %ld\n", save_data.gyro_temp);
    printf("Self-Test:Accel temperature @ time stored %ld\n", save_data.accel_temp);
#endif

    // Get size of packet to store.
    inv_get_mpl_state_size(&packet_sz);

    // Create place to store data
    buffer = (unsigned char *)malloc(packet_sz + 10);
    if (buffer == NULL) {
        printf("Self-Test:Can't allocate buffer\n");
        return -1;
    }

    result = inv_save_mpl_states(buffer, packet_sz);
    if (result) {
        result= -1;
    } else {
        fptr= fopen(MLCAL_FILE, "wb+");
        if (fptr != NULL) {
            fwrite(buffer, 1, packet_sz, fptr);
            fclose(fptr);
        } else {
            printf("Self-Test:ERR- Can't open calibration file to write - %s\n", 
                   MLCAL_FILE);
            result= -1;
        }
    }

    free(buffer);
    return result;
}

