/*
 * input interface testing
 */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/config.h>
#include <stdio.h>
#include <stdlib.h>
#include "linux/ml_sysfs_helper.h"
#include "authenticate.h"
#include "ml_load_dmp.h"

#if 0
struct input_event {
       struct timeval time;
       __u16 type;
       __u16 code;
       __s32 value;
};
#endif

void HandleOrient(int orient)
{    
    if (orient & 0x01)
        printf("INV_X_UP\n");
    if (orient & 0x02) 
        printf("INV_X_DOWN\n");
    if (orient & 0x04) 
        printf("INV_Y_UP\n");
    if (orient & 0x08) 
        printf("INV_Y_DOWN\n");
    if (orient & 0x10) 
        printf("INV_Z_UP\n");
    if (orient & 0x20) 
        printf("INV_Z_DOWN\n");
    if (orient & 0x40) 
        printf("INV_ORIENTATION_FLIP\n");
}

void HandleTap(int tap)
{
    int tap_dir = tap/8;
    int tap_num = tap%8 + 1;

    switch (tap_dir) {
        case 1:
            printf("INV_TAP_AXIS_X_POS\n");
            break;
        case 2:
            printf("INV_TAP_AXIS_X_NEG\n");
            break;
        case 3:
            printf("INV_TAP_AXIS_Y_POS\n");
            break;
        case 4:
            printf("INV_TAP_AXIS_Y_NEG\n");
            break;
        case 5:
            printf("INV_TAP_AXIS_Z_POS\n");
            break;
        case 6:
            printf("INV_TAP_AXIS_Z_NEG\n");
            break;
        default:
            break;
    }
    printf("Tap number: %d\n", tap_num);
}

static void read_compass(int event_number) 
{
    int ev_size, ret_byte, ii;
    int fd = -1;
    struct input_event ev[10];
    char name[64];
    char file_name[64];
    unsigned int RX;
    unsigned long long time0, time1, time2;
    struct timespec tsp;
    ev_size = sizeof(struct input_event);
    sprintf(file_name, "/dev/input/event%d", event_number);
    if ((fd = open(file_name, O_RDONLY)) < 0 ) {
        printf("fail to open compass\n");
        return;
    }

    /* NOTE: Use this to pass device name to HAL. */
    ioctl (fd, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading From : (%s)\n", name);
    while (1) {
    clock_gettime(CLOCK_MONOTONIC, &tsp);
        /*read compass data here */
        if(fd > 0){
            ret_byte = read(fd, ev, ev_size);
        } else {
            ret_byte = -1;
        }
        time0 = tsp.tv_nsec/1000000 + tsp.tv_sec * 1000LL;
        if (ret_byte < 0)
            continue;
        for (ii = 0; ii < ret_byte/ev_size; ii++) {
            if(EV_REL != ev[ii].type) {
                time2 = ev[ii].time.tv_usec/1000 + ev[ii].time.tv_sec * 1000LL;
                printf("mono=%lldms, diff=%d\n", time2, (int)(time1-time0));
                continue;
            }
            switch (ev[ii].code) {
            case REL_X:
                printf("CX:%5d ", ev[ii].value);
                break;
            case REL_Y:
                printf("CY:%5d ", ev[ii].value);
                break;
            case REL_Z:
                printf("CZ:%5d ", ev[ii].value);
                break;
            case REL_MISC:
                RX = ev[ii].value;
                break;
            case REL_WHEEL:
                time1 = ((unsigned long long)(RX)<<32) + (unsigned int)ev[ii].value;
                time1 = time1/1000000;
                printf("time1: %lld ", time1);
                break;
            default:
                printf("GES?:  %5d ", ev[ii].code);
                break;
            }
        }
    }
    close(fd);
}

static void read_gesture(int num)
{
    int ev_size, ret_byte, ii;
    int fd = -1;
    struct input_event ev[10];
    char name[64];
    char file_name[64];
    unsigned long long time;
    struct timespec tsp;
    ev_size = sizeof(struct input_event);
    sprintf(file_name, "/dev/input/event%d", num);
    MPL_LOGI("%s\n", file_name);
    if ((fd = open(file_name, O_RDONLY)) < 0 ) {
        printf("fail to open gusture.\n");
        return;
    }

    /* NOTE: Use this to pass device name to HAL. */
    ioctl (fd, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading From : (%s)\n", name);
    while(1){
        clock_gettime(CLOCK_MONOTONIC, &tsp);
        if(fd > 0){
            ret_byte = read(fd, ev, ev_size);
        } else {
            ret_byte = -1;
        }
        time = tsp.tv_nsec + tsp.tv_sec * 1000000000LL;
        //printf("retbyte=%d, ev3=%d\n", ret_byte, ev_size*3);
        if (ret_byte < 0)
            continue;
        for (ii = 0; ii < ret_byte/ev_size; ii++) {
            if(EV_REL != ev[ii].type) {
                time = ev[ii].time.tv_usec + ev[ii].time.tv_sec * 1000000LL;
                printf("mono=%lld\n", time);
                continue;
            }
            switch (ev[ii].code) {
            case REL_RX:
                printf("GESX:%5x\n", ev[ii].value);
                HandleTap(ev[ii].value);
                break;
            case REL_RY:
                printf("GESY:%5x\n", ev[ii].value);
                HandleOrient(ev[ii].value);
                break;
            case REL_RZ:
                printf("FLICK:%5x\n", ev[ii].value);
                break;
            default:
                printf("?:  %5d ", ev[ii].code);
                break;
            }
        }
        }
}

static void read_gyro_accel(int num)
{
    int ev_size, ret_byte, ii;
    int fd = -1;
    unsigned int RX;
    struct input_event ev[10];
    char name[64];
    char file_name[64];
    unsigned long long time0, time1, time2;
    struct timespec tsp;
    ev_size = sizeof(struct input_event);
    sprintf(file_name, "/dev/input/event%d", num);
    if ((fd = open(file_name, O_RDONLY)) < 0 ) {
        printf("fail to open gyro/accel\n");
        return;
    }    
        
    /* NOTE: Use this to pass device name to HAL. */
    ioctl (fd, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading From : (%s)\n", name);
    while (1){
        //usleep(20000);
        ret_byte = read(fd, ev, ev_size);
        if (ret_byte < 0)
            continue;
        //ret_byte = 0;

        for (ii = 0; ii < ret_byte/ev_size; ii++) {
            if(EV_REL != ev[ii].type) {
                time0 = ev[ii].time.tv_usec/1000 + ev[ii].time.tv_sec * 1000LL;
                printf("T: %lld diff=%d ", time0, (int)(time1 - time0));
                clock_gettime(CLOCK_MONOTONIC, &tsp);
                time2 = tsp.tv_nsec/1000000 + tsp.tv_sec * 1000LL;
                printf("mono=%lld, diff2=%d\n", time2, (int)(time1 - time2));
                continue;
            }
            switch (ev[ii].code) {
            case REL_X:
                printf("GX:%5d ", ev[ii].value);
                break;
            case REL_Y:
                printf("GY:%5d ", ev[ii].value);
                break;
            case REL_Z:
                printf("GZ:%5d ", ev[ii].value);
                break;
            case REL_RX:
                printf("AX:%5d ", ev[ii].value);
                break;
            case REL_RY:
                printf("AY:%5d ", ev[ii].value);
                break;
            case REL_RZ:
                printf("AZ:%5d ", ev[ii].value);
                break;
            case REL_MISC:
                RX = ev[ii].value;
                break;
            case REL_WHEEL:
                time1 = ((unsigned long long)(RX)<<32) + (unsigned int)ev[ii].value;
                time1 = time1/1000000;
                printf("time1: %lld ", time1);
                break;
            default:
                printf("?:  %5d ", ev[ii].code);
                break;
            }
        }
    }
    close(fd);
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
int inv_sysfs_read(char *filename, long num_bytes, char *data)
{
    FILE *fp;
    int count;

    if (!filename)
        return -1;
    fp = fopen(filename, "r");
    if (!fp)
        return -errno;
    count = fread(data, 1, num_bytes, fp);
    fclose(fp);
    return count;
}

void enable_flick(char *p)
{
    char sysfs_file[200];
    printf("flick:%s\n", p);
    sprintf(sysfs_file, "%s/flick_int_on", p);
    inv_sysfs_write(sysfs_file, 1);
    sprintf(sysfs_file, "%s/flick_upper", p);
    inv_sysfs_write(sysfs_file, 3147790);
    sprintf(sysfs_file, "%s/flick_lower", p);
    inv_sysfs_write(sysfs_file, -3147790);
    sprintf(sysfs_file, "%s/flick_counter", p);
    inv_sysfs_write(sysfs_file, 50);
    sprintf(sysfs_file, "%s/flick_message_on", p);
    inv_sysfs_write(sysfs_file, 0);
    sprintf(sysfs_file, "%s/flick_axis", p);
    inv_sysfs_write(sysfs_file, 2);
}

void setup_dmp(char *sysfs_path)
{
    char sysfs_file[200];
    char firmware_loaded[200], dmp_path[200];
    char dd[10];
    
    inv_get_dmpfile(dmp_path);
    sprintf(sysfs_file, "%s/fifo_rate", sysfs_path);
    inv_sysfs_write(sysfs_file, 200);    
    sprintf(sysfs_file, "%s/FSR", sysfs_path);
    inv_sysfs_write(sysfs_file, 2000);    
    sprintf(sysfs_file, "%s/accl_fs", sysfs_path);
    inv_sysfs_write(sysfs_file, 4);
    /*
    sprintf(firmware_loaded, "%s/%s", sysfs_path, "firmware_loaded");
    printf("%s\n", firmware_loaded);
    inv_sysfs_write(firmware_loaded, 0);
    inv_sysfs_read(firmware_loaded, 1, dd);
    printf("beforefirmware_loaded=%c\n", dd[0]);

    if ((fd = open(dmp_path, O_WRONLY)) < 0 ) {
        perror("dmp fail");
    }
    inv_load_dmp(fd);
    close(fd);
    */
    inv_sysfs_read(firmware_loaded, 1, dd);
    printf("firmware_loaded=%c\n", dd[0]);
}
void read_pedometer(char *sysfs_path){
    int steps;
    char sysfs_file[200];
    char dd[4];
    sprintf(sysfs_file, "%s/pedometer_steps", sysfs_path);
    inv_sysfs_read(sysfs_file, 4, dd);
    steps = dd[0] << 8 | dd[1];
    printf("fff=%d\n", steps);
}
/* The running sequence:
    "input_gyro 2 &". 
    This will setup the dmp firmware and let it run on background. 
    tap and flick will work at this time. 
    To see accelerometer data and gyro data.
    type :
    "input_gyro ". 
    This will print out gyro data and accelerometer data
    To see Compass data
    type:
    "input_gyro 1" */

int main(int argc, char *argv[])
{
    unsigned int RX, i, sel;
    unsigned char key[16];
    struct timeval tv;
    struct timespec tsp0, tsp1, tsp2, tsp3;
    int event_num;
    char sysfs_path[200];
    char chip_name[20];
    char sysfs_file[200];
    if (INV_SUCCESS != inv_check_key()) {
        printf("key check fail\n");
        exit(0);
    }else
        printf("key authenticated\n");

    for(i=0;i<16;i++){
        key[i] = 0xff;
    }
    RX = inv_get_sysfs_key(key);
    if(RX == INV_SUCCESS){
        for(i=0;i<16;i++){
            printf("%d, ", key[i]);
        }
        printf("\n");
    }else{
        printf("get key failed\n");
    }
    memset(sysfs_path, 0, 200);
    memset(sysfs_file, 0, 200);
    memset(chip_name, 0, 20);
    inv_get_sysfs_path(sysfs_path);
    inv_get_chip_name(chip_name);
    printf("sysfs path: %s\n", sysfs_path);
    printf("chip name: %s\n", chip_name);
    /*set up driver*/
    sprintf(sysfs_file, "%s/enable", sysfs_path);
    inv_sysfs_write(sysfs_file, 0);
    sprintf(sysfs_file, "%s/power_state", sysfs_path);
    inv_sysfs_write(sysfs_file, 1);    
    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...\n");
    
    if(argc ==2 )
        sel = argv[1][0] - 0x30;
    else
        sel = 0;
    switch(sel){
    case 0:
        printf("-------------------------------\n");
        printf("--- log gyro and accel data ---\n");
        printf("-------------------------------\n");
        sprintf(sysfs_file, "%s/enable", sysfs_path);
        inv_sysfs_write(sysfs_file, 1);
        if(inv_get_handler_number(chip_name, &event_num) < 0)
            printf("mpu not installed\n");
        else
            read_gyro_accel(event_num);
         break;
         
    case 1:
        printf("------------------------\n");
        printf("--- log compass data ---\n");
        printf("------------------------\n");
        sprintf(sysfs_file, "%s/compass_enable", sysfs_path);
        inv_sysfs_write(sysfs_file, 1);
        sprintf(sysfs_file, "%s/enable", sysfs_path);
        inv_sysfs_write(sysfs_file, 1);
        if(inv_get_handler_number("INV_COMPASS", &event_num) < 0)
            printf("compass is not enabled\n");
        else
            read_compass(event_num);
        break;
        
    case 2:
        printf("--------------------\n");
        printf("--- log gestures ---\n");
        printf("--------------------\n");
        setup_dmp(sysfs_path);
        enable_flick(sysfs_path);
        sprintf(sysfs_file, "%s/tap_on", sysfs_path);
        inv_sysfs_write(sysfs_file, 1);
        sprintf(sysfs_file, "%s/enable", sysfs_path);
        inv_sysfs_write(sysfs_file, 1);
        if(inv_get_handler_number("INV_DMP", &event_num) < 0)
            printf("DMP not enabled\n");
        else
            read_gesture(event_num);
        break;
        
    case 3:
        printf("-----------------\n");
        printf("--- pedometer ---\n");
        printf("-----------------\n");
        read_pedometer(sysfs_path);
        break;
        
    default:
        printf("error choice\n");
        break;
    }
    
    gettimeofday(&tv, NULL);
    clock_gettime(CLOCK_MONOTONIC, &tsp1);
    clock_gettime(CLOCK_REALTIME, &tsp0);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tsp2);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tsp3);
    //printf("id=%d, %d, %d, %d\n", CLOCK_MONOTONIC,  CLOCK_REALTIME,
    //    CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID);
    //printf("sec0=%lu , nsec=%ld\n", tsp0.tv_sec, tsp0.tv_nsec);
    //printf("sec1=%lu , nsec=%ld\n", tsp1.tv_sec, tsp1.tv_nsec);
    //printf("sec=%lu , nsec=%ld\n", tsp2.tv_sec, tsp2.tv_nsec);
    //printf("sec=%lu , nsec=%ld\n", tsp3.tv_sec, tsp3.tv_nsec);

    //ioctl (fd, EVIOCGNAME (sizeof (name)), name);
    //printf ("Reading From : %s (%s)\n", argv[1], name);


    return 0;
}

