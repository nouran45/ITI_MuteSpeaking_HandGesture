#include "fixed_point.h"

// Define the fixed-point constants needed for the complementary filter
// These are pre-calculated to avoid floating-point math at runtime
const fixed_t FP_ALPHA = 60917; // 0.93 in fixed-point format
const fixed_t FP_ONE_MINUS_ALPHA = 4587; // (1.0 - 0.93) = 0.07 in fixed-point format

// Pre-calculated gyro scale * dt to reduce multiplication operations
// GYRO_SCALE * DT_SEC = (1/131) * 0.04 = 0.000305 in fixed-point format
const fixed_t FP_GYRO_SCALE_DT = 20; // 0.000305 * 65536 rounded

// Square root for fixed point numbers
fixed_t fp_sqrt(fixed_t a) {
    if (a <= 0) return 0;
    
    // Newton-Raphson method for square root
    fixed_t x = a;
    fixed_t x_old;
    
    do {
        x_old = x;
        // x = (x + a/x) / 2 implemented in fixed-point
        x = (x + fp_div(a, x)) >> 1;
    } while (x < x_old);
    
    return x;
}

// Clamp a fixed-point value between min and max
fixed_t fp_clamp(fixed_t value, fixed_t min, fixed_t max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
