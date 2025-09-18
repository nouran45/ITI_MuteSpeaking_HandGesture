#include "axis_mapping_fix.h"
#include <math.h>

/* Explicit handling for isnan/isinf if math.h doesn't provide them */
#ifndef isnan
#define isnan(x) ((x) != (x))
#endif
#ifndef isinf
#define isinf(x) (((x) == INFINITY) || ((x) == -INFINITY))
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

void compute_angles_fixed(s16 ax, s16 ay, s16 az,
                         float* out_roll_deg, float* out_pitch_deg,
                         u8 finger_idx)
{
    float fax = (float)ax, fay = (float)ay, faz = (float)az;
    
    // Bounds checking
    if (fax == 0.0f && fay == 0.0f && faz == 0.0f) {
        *out_roll_deg = 0.0f;
        *out_pitch_deg = 0.0f;
        return;
    }
    
#if SWAP_AXES || (SWAP_AXES_THUMB && finger_idx == 0)
    // SWAPPED MAPPING (if original doesn't work correctly)
    // Roll = arctan(ACCx / sqrt(ACCy² + ACCz²)) * 180/π  
    float roll_denominator = sqrtf(fay*fay + faz*faz);
    if (roll_denominator > 0.01f) {
        float roll_rad = atan2f(fax, roll_denominator);
        *out_roll_deg = roll_rad * (180.0f / M_PI);
        
        // Fix for thumb's always negative roll - invert the roll value for thumb
        if (finger_idx == 0) {
            *out_roll_deg = -*out_roll_deg;  // Invert roll for thumb
        }
    } else {
        *out_roll_deg = 0.0f;
    }
    
    // Pitch = arctan(ACCy / sqrt(ACCx² + ACCz²)) * 180/π
    float pitch_denominator = sqrtf(fax*fax + faz*faz);
    if (pitch_denominator > 0.01f) {
        float pitch_rad = atan2f(fay, pitch_denominator);
        *out_pitch_deg = pitch_rad * (180.0f / M_PI);
        
        // Amplify the thumb's pitch for better response
        if (finger_idx == 0) {
            *out_pitch_deg *= 2.0f; // Double pitch amplitude for the thumb to address "much less than predicted" issue
        }
    } else {
        *out_pitch_deg = 0.0f;
    }
#else
    // STANDARDIZED MAPPING - same as gesture_stream_platform.c and gesture_stream.c
    // Roll = arctan(accel_y / sqrt(accel_x² + accel_z²)) * 180/π
    float roll_denominator = sqrtf(fax * fax + faz * faz);
    if (roll_denominator > 0.01f) {
        *out_roll_deg = atan2f(fay, roll_denominator) * (180.0f / M_PI);
    } else {
        *out_roll_deg = 0.0f;
    }
    
    // Pitch = arctan(accel_x / sqrt(accel_y² + accel_z²)) * 180/π
    float pitch_denominator = sqrtf(fay * fay + faz * faz);
    if (pitch_denominator > 0.01f) {
        *out_pitch_deg = atan2f(fax, pitch_denominator) * (180.0f / M_PI);
    } else {
        *out_pitch_deg = 0.0f;
    }
    
    // Check for NaN results
    if (isnan(*out_roll_deg) || isinf(*out_roll_deg)) *out_roll_deg = 0.0f;
    if (isnan(*out_pitch_deg) || isinf(*out_pitch_deg)) *out_pitch_deg = 0.0f;
#endif
    
    // Clamp to reasonable ranges
    if (*out_roll_deg > 180.0f) *out_roll_deg = 180.0f;
    if (*out_roll_deg < -180.0f) *out_roll_deg = -180.0f;
    if (*out_pitch_deg > 90.0f) *out_pitch_deg = 90.0f;
    if (*out_pitch_deg < -90.0f) *out_pitch_deg = -90.0f;
}
