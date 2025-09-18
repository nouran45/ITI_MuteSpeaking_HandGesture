#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include "STD_TYPES.h"

// Fixed-point math library for improved performance on AVR
// Uses 16.16 format (16 bits integer, 16 bits fraction)

// Define 64-bit integer type for intermediate calculations
typedef long long s64;

// Define the fixed-point type (32 bits total)
typedef s32 fixed_t;

// Constants
#define FP_SHIFT 16
#define FP_ONE (1L << FP_SHIFT)
#define FP_HALF (FP_ONE >> 1)

// Conversion macros
#define FP_FROM_FLOAT(f) ((fixed_t)((f) * FP_ONE + ((f) >= 0 ? 0.5 : -0.5)))
#define FP_TO_FLOAT(fp) ((float)(fp) / FP_ONE)
#define FP_FROM_INT(i) ((fixed_t)((s32)(i) << FP_SHIFT))
#define FP_TO_INT(fp) ((s16)((fp) >> FP_SHIFT))

// Basic fixed-point operations
static inline fixed_t fp_add(fixed_t a, fixed_t b) {
    return a + b;
}

static inline fixed_t fp_sub(fixed_t a, fixed_t b) {
    return a - b;
}

static inline fixed_t fp_mul(fixed_t a, fixed_t b) {
    return ((s64)a * (s64)b) >> FP_SHIFT;
}

static inline fixed_t fp_div(fixed_t a, fixed_t b) {
    // Avoid division by zero
    if (b == 0) return 0;
    return ((s64)a << FP_SHIFT) / b;
}

// Clamping function (bounds checking)
static inline fixed_t fp_clamp(fixed_t value, fixed_t min, fixed_t max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Pre-computed lookup tables for trig functions could be added here
// for even faster performance

#endif // FIXED_POINT_H
