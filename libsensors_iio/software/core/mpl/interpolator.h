/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */
#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#ifdef __cplusplus
extern "C" {
#endif

//#include "mltypes.h"

    /* ------------ */
    /* - Defines. - */
    /* ------------ */

#define MAX_INTERPOLATION (20)

typedef struct  {
    long x[2];
    long y[4];
} tInterpolate2;
typedef struct  {
    long x[2];
    long y[6];
} tInterpolate3;
typedef struct  {
    long x[5];
    long y[10];
    int idx1;
} tInterpolate5;
typedef struct {
    tInterpolate2 state1;
    tInterpolate2 state2;
} tInterpolate4;
typedef struct {
    tInterpolate3 state1;
    tInterpolate2 state2;
} tInterpolate6;
typedef struct {
    tInterpolate2 state1;
    tInterpolate4 state2;
} tInterpolate8;
typedef struct {
    tInterpolate3 state1;
    tInterpolate3 state2;
} tInterpolate9;
typedef struct {
    tInterpolate5 state1;
    tInterpolate2 state2;
} tInterpolate10;
typedef struct {
    tInterpolate4 state1;
    tInterpolate3 state2;
} tInterpolate12;
typedef struct {
    tInterpolate5 state1;
    tInterpolate3 state2;
} tInterpolate15;
typedef struct {
    tInterpolate2 state1;
    tInterpolate8 state2;
} tInterpolate16;
typedef struct {
    tInterpolate9 state1;
    tInterpolate2 state2;
} tInterpolate18;
typedef struct {
    tInterpolate10 state1;
    tInterpolate2 state2;
} tInterpolate20;

typedef  union {
    tInterpolate2 u2;
    tInterpolate3 u3;
    tInterpolate4 u4;
    tInterpolate5 u5;
    tInterpolate6 u6;
    tInterpolate8 u8;
    tInterpolate9 u9;
    tInterpolate10 u10;
    tInterpolate12 u12;
    tInterpolate15 u15;
    tInterpolate16 u16;
    tInterpolate18 u18;
    tInterpolate20 u20;
} tInterpolateState;

    /* --------------------- */
    /* - Function p-types. - */
    /* --------------------- */
int inv_get_interp_amount( int x );
int inv_interpolate( int amount, long input, long *output, tInterpolateState *state );
long inv_fxmult( long x, long y );


#ifdef __cplusplus
}
#endif

#endif /* INTERPOLATOR_H */
