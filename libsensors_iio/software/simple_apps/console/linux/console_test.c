/*******************************************************************************
 * $Id: $
 ******************************************************************************/
 
/*******************************************************************************
 *
 * Copyright (c) 2011 InvenSense Corporation, All Rights Reserved.
 *
 ******************************************************************************/
//#include <linux/conio.h>
//#include <fcntl.h>
//#include <termios.h>
//#include <unistd.h>

//#if 0
#include <stdio.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <poll.h>
#include <stdlib.h>
#include <linux/input.h>

#include "log.h"
#undef MPL_LOG_TAG
#define MPL_LOG_TAG "console_test"

#include "mlos.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "inv_sysfs_utils.h"
#include "ml_stored_data.h"
#include "ml_math_func.h"
#include "ml_load_dmp.h"
/*#else
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
#include "inv_sysfs_utils.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "ml_stored_data.h"
#include "ml_math_func.h"
#include "ml_load_dmp.h"
#include "log.h"
#endif*/

/* TODO: add devices as needed. */
#define ITG3500         (0)
#define MPU6050         (1)
#define BMA250          (2)
#define NUM_DEVICES     (ITG3500 + MPU6050 + BMA250)

#define DEVICE          MPU6050
#define DMP_IMAGE       dmp_firmware_200_latest
#define SIX_AXES        6
#define NINE_AXES       9

#if 0
struct input_event {
       struct timeval time;
       __u16 type;
       __u16 code;
       __s32 value;
};
#endif

/* TODO: Add paths to other attributes.
 * TODO: Input device paths depend on the module installation order.
 */
const struct inv_sysfs_names_s filenames[NUM_DEVICES] = {
    {        /* ITG3500 */
        .buffer             = "/dev/input/event0",
        .enable             = "/sys/bus/i2c/devices/4-0068/inv_gyro/fifo_enable",
        .raw_data           = "/sys/bus/i2c/devices/4-0068/inv_gyro/raw_gyro",
        .temperature        = "/sys/bus/i2c/devices/4-0068/inv_gyro/temperature",
        .fifo_rate          = "/sys/bus/i2c/devices/4-0068/inv_gyro/fifo_rate",
        .power_state        = "/sys/bus/i2c/devices/4-0068/inv_gyro/power_state",
        .fsr                = "/sys/bus/i2c/devices/4-0068/inv_gyro/FSR",
        .lpf                = "/sys/bus/i2c/devices/4-0068/inv_gyro/lpf",
        .scale              = "/sys/bus/i2c/devices/4-0068/inv_gyro/gyro_scale",
        .temp_scale         = "/sys/bus/i2c/devices/4-0068/inv_gyro/temp_scale",
        .temp_offset        = "/sys/bus/i2c/devices/4-0068/inv_gyro/temp_offset",
        .self_test          = "/sys/bus/i2c/devices/4-0068/inv_gyro/self_test",
        .accel_en           = NULL,
        .accel_fifo_en      = NULL,
        .accel_fs           = NULL,
        .clock_source       = NULL,
        .early_suspend_en   = NULL,
        .firmware_loaded    = NULL,
        .gyro_en            = NULL,
        .gyro_fifo_en       = NULL,
        .key                = NULL,
        .raw_accel          = NULL,
        .reg_dump           = NULL,
        .tap_on             = NULL,
        .dmp_firmware       = NULL
    },

    {        /* MPU6050 */
        .buffer             = "/dev/input/event0",
        .enable             = "/sys/class/invensense/mpu/enable",
        .raw_data           = "/sys/class/invensense/mpu/raw_gyro",
        .temperature        = "/sys/class/invensense/mpu/temperature",
        .fifo_rate          = "/sys/class/invensense/mpu/fifo_rate",
        .power_state        = "/sys/class/invensense/mpu/power_state",
        .fsr                = "/sys/class/invensense/mpu/FSR",
        .lpf                = "/sys/class/invensense/mpu/lpf",
        .scale              = "/sys/class/invensense/mpu/gyro_scale",
        .temp_scale         = "/sys/class/invensense/mpu/temp_scale",
        .temp_offset        = "/sys/class/invensense/mpu/temp_offset",
        .self_test          = "/sys/class/invensense/mpu/self_test",
        .accel_en           = "/sys/class/invensense/mpu/accl_enable",
        .accel_fifo_en      = "/sys/class/invensense/mpu/accl_fifo_enable",
        .accel_fs           = "/sys/class/invensense/mpu/accl_fs",
        .clock_source       = "/sys/class/invensense/mpu/clock_source",
        .early_suspend_en   = "/sys/class/invensense/mpu/early_suspend_enable",
           .firmware_loaded = "/sys/class/invensense/mpu/firmware_loaded",
        .gyro_en            = "/sys/class/invensense/mpu/gyro_enable",
        .gyro_fifo_en       = "/sys/class/invensense/mpu/gyro_fifo_enable",
        .key                = "/sys/class/invensense/mpu/key",
        .raw_accel          = "/sys/class/invensense/mpu/raw_accl",
        .reg_dump           = "/sys/class/invensense/mpu/reg_dump",
        .tap_on             = "/sys/class/invensense/mpu/tap_on",
         .dmp_firmware      = "/sys/class/invensense/mpu/dmp_firmware"
    },

    {        /* BMA250 */
        .buffer             = "/dev/input/input/event1",
        .enable             = "/sys/devices/virtual/input/input1/enable",
        .raw_data           = "/sys/devices/virtual/input/input1/value",
        .temperature        = NULL,
        .fifo_rate          = NULL,
        .power_state        = NULL,
        .fsr                = NULL,
        .lpf                = NULL,
        .scale              = NULL,
        .temp_scale         = NULL,
        .temp_offset        = NULL,
        .self_test          = NULL,
        .accel_en           = NULL,
        .accel_fifo_en      = NULL,
        .accel_fs           = NULL,
        .clock_source       = NULL,
        .early_suspend_en   = NULL,
        .firmware_loaded    = NULL,
        .gyro_en            = NULL,
        .gyro_fifo_en       = NULL,
        .key                = NULL,
        .raw_accel          = NULL,
        .reg_dump           = NULL,
        .tap_on             = NULL,
        .dmp_firmware       = NULL
    }
};

static void (*s_func_cb) (void);

int inv_read_data(char *names, char *data)
{
    char str[8];
    int count;
    short s_data;

    count = inv_sysfs_read((char*)names, sizeof(str), str);
    if (count < 0)
        return count;
    count = sscanf(str, "%hd", &s_data);
    *data = s_data;
    if (count < 1)
        return -EAGAIN;
    return count;

}

void fifoCB(void)
{
    if (1) {
        float gyro[3];
        float accel[3];
        float orient[3];
        float rv[3];

        int8_t accuracy;
        inv_time_t timestamp;

        printf("/*************************************************\n"); 
        inv_get_sensor_type_gyroscope(gyro, &accuracy, &timestamp);
        printf("Gyro %13.6f %13.4f %13.4f %5d %9lld\n", 
                     gyro[0],
                     gyro[1],
                     gyro[2],
                     accuracy,
                     timestamp);

        inv_get_sensor_type_accelerometer(accel, &accuracy, &timestamp);
        printf("Accel %13.6f %13.4f %13.4f %5d %9lld\n", 
                     accel[0],
                     accel[1],
                     accel[2],
                     accuracy,
                     timestamp);

        inv_get_sensor_type_rotation_vector(rv, &accuracy, &timestamp);
        printf("RV %7.3f %7.3f %7.3f %5d %9lld\n",
                     rv[0],rv[1],rv[2],accuracy,timestamp);

        inv_get_sensor_type_orientation(orient, &accuracy, &timestamp);
        printf("Orientation %7.3f %7.3f %7.3f %5d %9lld\n",
                     orient[0],orient[1],orient[2],accuracy,timestamp);
        printf("/*************************************************\n"); 
    }
}

unsigned short orient;
signed char g_gyro_orientation[9] = {1, 0, 0,
                                     0, 1, 0,
                                     0, 0, 1};

signed char g_accel_orientation[9] = {-1, 0, 0,
                                      0, -1, 0,
                                      0, 0, 1};
float scale;
float range;
long sens;



short mTempOffset = 0;
short mTempScale = 0;
bool mFirstRead = 1;

/******************* FUNCTIONS *******************************/
#if 0
static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;                  // error
    return b;
}
#endif 

inv_error_t inv_set_fifo_processed_callback(void (*func_cb)(void))
{
    s_func_cb = func_cb;
    return INV_SUCCESS;
}

/**
 * @brief Keyboard hit function.
 */
int kbhit(void)
{
#if 1
    struct timeval tv;
    fd_set read_fd;

    tv.tv_sec=0;
    tv.tv_usec=0;
    FD_ZERO(&read_fd);
    FD_SET(0,&read_fd);

    if (select(1, &read_fd, NULL, NULL, &tv) == -1)
        return 0;

    if (FD_ISSET(0,&read_fd))
        return 1;

    return 0;
#else
    struct timeval tv;  
    fd_set rdfs;   
    
    tv.tv_sec = 0;  
    tv.tv_usec = 0;   
    
    FD_ZERO(&rdfs);  
    FD_SET (STDIN_FILENO, &rdfs);   
    
    select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);  
    return FD_ISSET(STDIN_FILENO, &rdfs); 
#endif
}

inv_error_t inv_constructor_default_enable()
{
    inv_error_t result;
  
    result = inv_enable_quaternion();
    if (result) {
        if (result == INV_ERROR_NOT_AUTHORIZED) {
            LOGE("Enable Quaternion failed: not authorized");
        }
        LOG_RESULT_LOCATION(result);
        return result;
    }
    result = inv_enable_motion_no_motion();
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }
    result = inv_enable_gyro_tc();
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }
    result = inv_enable_hal_outputs();
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }
    result = inv_enable_9x_sensor_fusion();
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }
    return result;
}

int read_attribute_sensor(int fd, char *data, unsigned int size)
{
    int count = 0;
        if (fd >=0) {
            count = read(fd, data, size);
            if(count < 0) {
                MPL_LOGE("read fails with error code=%d", count);
            }
            close(fd);
        }
        return count;
}

int inv_read_temperature(long long *data)
{
        int count = 0;
        int fd;

        if(mFirstRead) {
            char buf[4];
            fd = open(filenames[ITG3500].temp_scale, O_RDONLY);
            if(fd < 0) {
                MPL_LOGE("errors opening tempscale");
                return -1;
            }

            memset(buf, 0, sizeof(buf));

            count = read_attribute_sensor(fd, buf, sizeof(buf));
            if(count < 0) {
                MPL_LOGE("errors reading temp_scale");
                return -1;
            }

            count = sscanf(buf, "%hd", &mTempScale);
            if(count < 1)
                return -1;
            MPL_LOGI("temp scale = %d", mTempScale);
 
            fd = open(filenames[ITG3500].temp_offset, O_RDONLY);
            if(fd < 0) {
                MPL_LOGE("errors opening tempoffset");
                return -1;
            }

            memset(buf, 0, sizeof(buf));

            count = read_attribute_sensor(fd, buf, sizeof(buf));
            if(count < 0) {
                MPL_LOGE("errors reading temp_offset");
                return -1;
            }

            count = sscanf(buf, "%hd", &mTempOffset);
            if(count < 1)
                return -1;
            MPL_LOGI("temp offset = %d", mTempOffset);

            mFirstRead = false;
        }

        char raw_buf[25];
        short raw;
        long long timestamp;
        fd = open(filenames[ITG3500].temperature, O_RDONLY);
        if(fd < 0) {
            MPL_LOGE("errors opening temperature");
            return -1;
        }

        memset(raw_buf, 0, sizeof(raw_buf));

        count = read_attribute_sensor(fd, raw_buf, sizeof(raw_buf));
        if(count < 0) {
            MPL_LOGE("errors reading temperature");
            return -1;
        }
        count = sscanf(raw_buf, "%hd%lld", &raw, &timestamp);
        if(count < -1)
            return -1;
        MPL_LOGI("temperature raw = %d, timestamp = %lld", raw, timestamp);
        MPL_LOGI("temperature offset = %d", mTempOffset);
        MPL_LOGI("temperature scale = %d", mTempScale);
        int adjuster = 35 + ((raw-mTempOffset)/mTempScale);
        MPL_LOGI("pre-scaled temperature = %d", adjuster);
        MPL_LOGI("adjusted temperature = %d", adjuster*65536);
        //data[0] = adjuster * 65536;
        data[0] = (35 + ((raw - mTempOffset) / mTempScale)) * 65536.f;
        data[1] = timestamp;
        return 0;
}

int self_test(void)
{
        int err = 0;
        char str[50];
        char x[9], y[9], z[9];
        char pass[2];
        int fd;

        fd = open((char*)filenames[DEVICE].self_test, O_RDONLY);
        if(fd < 0) {
            return fd;
        }
        memset(str, 0, sizeof(str));
        err = read_attribute_sensor(fd, str, sizeof(str));
        if(err < 0) {
            return err;
        }
        MPL_LOGI("self_test result: %s", str);
        printf("Self test result: %s ", str);
        err = sscanf(str, "%[^','],%[^','],%[^','],%[^',']", x, y, z, pass);
        if(err < 1) {
            return err;
        }
        MPL_LOGI("Bias   : X:Y:Z (%ld, %ld, %ld)", atol(x), atol(y), atol(z));
        //printf("Bias   : X:Y:Z (%ld, %ld, %ld)", atol(x), atol(y), atol(z));
        if (atoi(pass)) {
            MPL_LOGI("Result : PASS (1)");
            printf("----> PASS (1)\n");
        } else {
            MPL_LOGI("Result : FAIL (0)");
            printf("----> FAIL (0)\n");
        }
        return err;
}

/*******************************************************************************
 ******************************* MAIN ******************************************
 ******************************************************************************/

/**
 * @brief Main function
 */
int main(int argc, char *argv[])
{
    int key = 0;
    int ready;
    long accel[3];
    short gyro[3];
    long long timestamp = 0;
    inv_error_t result;

    char data;
    unsigned char i;
    int fd, bytes_read;
    struct pollfd pfd;
    unsigned long long time_stamp;
    unsigned int time_H;
    struct input_event ev[100];
#ifdef INV_PLAYBACK_DBG
    int logging = false;
    FILE *logfile = NULL;
#endif

    result = inv_init_mpl();
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    }

    // Master Enabling.  This also turns on power_state
    if (inv_sysfs_write((char *)filenames[DEVICE].enable, 1) < 0)
       printf("ERR- Failed to enable event generation\n");
    else {
          inv_read_data((char *)filenames[DEVICE].enable, &data);
       printf("Event enable= %d\n", data);
    }

    // Power ON - No need after master enable above but do it anyway
    if (inv_sysfs_write((char *)filenames[DEVICE].power_state, 1) < 0)
       printf("ERR- Failed to set power state=1\n");
    else {
          inv_read_data((char *)filenames[DEVICE].power_state, &data);
       printf("Power state: %d\n", data);
    }
    
    // Turn on tap
    if (inv_sysfs_write((char *)filenames[DEVICE].tap_on, 1) < 0) {
       printf("ERR- Failed to enable Tap On\n");
    } 
    else {
          inv_read_data((char *)filenames[DEVICE].tap_on, &data);
       printf("Tap-on: %d\n", data);
    }

    // Program DMP code.  No longer required to enable tap-on first
    if ((result = 
            inv_sysfs_write((char *)filenames[DEVICE].firmware_loaded, 0)) < 0) {
        printf("ERR- Failed to initiate DMP re-programming %d\n",result);
    } else {
         if ((fd = open(filenames[DEVICE].dmp_firmware, O_WRONLY)) < 0 ) {
            printf("ERR- Failed file open to write DMP\n");
            close(fd);
            exit(0);
        } else {
            // Program 200Hz version
            //result = write(fd, DMP_IMAGE, sizeof(DMP_IMAGE));
            //printf("Downloaded %d byte(s) to DMP\n", result);
            result = inv_load_dmp(fd);
            //LOG_RESULT_LOCATION(result);
            close(fd);
        }
    }

    // Query DMP running.  For now check by 'firmware_loaded' status 
    if (inv_read_data((char *)filenames[DEVICE].firmware_loaded, &data) < 0) {
        printf("ERR- Failed to read 'firmware_loaded'\n");
    } else {
        printf("Firmware Loaded/ DMP running: %d\n", data);
    }

    inv_set_fifo_processed_callback(fifoCB);
    result = inv_constructor_default_enable();
    result = inv_start_mpl();
    if (result) {
        LOG_RESULT_LOCATION(result);
        return result;
    } else {
        printf ("MPL started\n");
    }

    /* Gyro Setup */
    orient = inv_orientation_matrix_to_scalar(g_gyro_orientation);
    inv_set_gyro_orientation_and_scale(orient,2000L<<15);

    /* Accel Setup */
    orient = inv_orientation_matrix_to_scalar(g_accel_orientation);
    /* NOTE: sens expected to be 2 (FSR) * 1L<<15 for 16 bit hardware data.
     * The BMA250 only uses a 10 bit ADC, so we shift the data by 6 bits.
     * 2 * 1L<<15 * 1<<6 ==  1LL<<22
     */
    inv_set_accel_orientation_and_scale(orient, 1LL<<22); 

    // Enable Gyro
    if (inv_sysfs_write((char *)filenames[DEVICE].gyro_en, 1) <0)
        printf("ERR- Failed to enable Gyro\n");
    else {
        inv_read_data((char *)filenames[DEVICE].gyro_en, &data);
        printf("Gyro enable: %d\n", data);
    }

    // Enable Accel
    if (inv_sysfs_write((char *)filenames[DEVICE].accel_en, 1) <0)
        printf("ERR- Failed to enable Accel\n");
    else {
        inv_read_data((char *)filenames[DEVICE].accel_en, &data);
        printf("Accel enable: %d\n", data);
    }

    // polling for data
    fd = open(filenames[DEVICE].buffer, O_RDONLY);
    if(fd < 0) {
         MPL_LOGE("Cannot open device event buffer");
    }
    
    pfd.fd = fd;
    pfd.events = POLLIN;

    while (1) {

       result = kbhit();
        if (result) {
           key = getchar();
        } else {
           key = 0;
        }
        if (key == 'l') {
           MPL_LOGI(" 'l' - load calibration file");
           inv_load_calibration();
        }
        if (key == 't') {
           MPL_LOGI(" 't' - self test");
           self_test();
        }
        if (key == 'q') {
            MPL_LOGI(" 'q' - store calibration file");
            inv_store_calibration();
            break;
        }
#ifdef INV_PLAYBACK_DBG
        if (key == 's') {
            if (!logging) {
                MPL_LOGI(" 's' - toggle logging on");
                logfile = fopen("/data/playback.bin", "wb");
                if (logfile) {
                    inv_turn_on_data_logging(logfile);
                    logging = true;
                } else {
                    MPL_LOGI("Error : "
                             "cannot open log file '/data/playback.bin'");
                }
            } else {
                MPL_LOGI(" 's' - toggle logging off");
                inv_turn_off_data_logging();
                fclose(logfile);
                logging = false;
            }
            break;
        }
#endif

        ready = poll(&pfd, 1, 100);
        if (ready) {
            bytes_read = read_attribute_sensor(fd, (char *)ev,
                                      sizeof(struct input_event) * SIX_AXES);
            //bytes_read= read(fd, &ev, sizeof(struct input_event) * SIX_AXES);
            if (bytes_read > 0) {
                int executed;

                for (i = 0; i < bytes_read / sizeof(struct input_event); i++) {
                    if (ev[i].type == EV_REL) {
                        switch (ev[i].code) {
                        case REL_X:
                            printf("REL_X\n");
                            gyro[0]= ev[i].value;                    //Gyro X
                            printf("Gyro X:%5d ", gyro[0]);        
                            break;
                        case REL_Y:
                            printf("REL_Y\n");
                            gyro[1]= ev[i].value;                    //Gyro Y
                            printf("Gyro Y:%5d ", gyro[1]);    
                            break;
                        case REL_Z:
                            printf("REL_Z\n");
                            gyro[2]= ev[i].value;                    //Gyro Z
                            printf("Gyro Z:%5d ", gyro[2]);        
                            break;
                        case REL_RX:
                            printf("REL_RX\n");
                            accel[0]= ev[i].value;                   //Accel X
                            printf("Accl X:%5ld ", accel[0]);
                            break;
                        case REL_RY:
                            printf("REL_RY\n");
                            accel[1]= ev[i].value;                   //Accel Y
                            printf("Accl Y:%5ld ", accel[1]);    
                            break;
                        case REL_RZ:
                            printf("REL_RZ\n");
                            accel[2]= ev[i].value;                   //Accel Z
                            printf("Accl Z:%5ld ", accel[2]);    
                            break;
                        case REL_MISC:
                            time_H= ev[i].value;
                            break;
                        case REL_WHEEL:
                            time_stamp = ((unsigned long long)(time_H) << 32) + 
                                           (unsigned int)ev[i].value;
                            break;
                        default:
                            printf("ERR- Un-recognized event code:  %5d ", ev[i].code);
                            break;
                        }
                    } else {
#if 0
                        clock_gettime(CLOCK_MONOTONIC, &timer);
                        curr_time= timer.tv_nsec + timer.tv_sec * 1000000000LL;                        
                        printf("Curr time= %lld, Dev time stamp= %lld, Time diff= %d ms\n", curr_time, time_stamp, (curr_time-time_stamp)/1000000LL);
#endif
                    }
                }

                // build & process gyro + accel data
                result = inv_build_gyro(gyro, (inv_time_t)timestamp, &executed);
                if (result) {
                    LOG_RESULT_LOCATION(result);
                } else if ((result = inv_build_accel(accel, 0,
                                                    (inv_time_t)timestamp,
                                                    &executed))) {
                    LOG_RESULT_LOCATION(result);
                } 
                if (executed) {
                    printf("Exec on data Ok\n");
                    s_func_cb();
                }

           } else {
               //printf ("ERR- No data!\n");
           }

        } else { MPL_LOGV("Device not ready"); }
    }
    close(fd);
    
#ifdef INV_PLAYBACK_DBG
    if (logging) {
        inv_turn_off_data_logging();
        fclose(logfile);
    }
#endif

    return 0;
}
