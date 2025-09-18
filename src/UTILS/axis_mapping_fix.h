/*
 * AXIS MAPPING TEST - Quick fix for roll/pitch swapping issue
 * 
 * Instructions:
 * 1. Test current mapping first
 * 2. If roll > pitch when finger is bent, enable SWAP_AXES
 * 3. Recompile and test again
 */

#ifndef AXIS_MAPPING_FIX_H
#define AXIS_MAPPING_FIX_H

#include "STD_TYPES.h"

// ⭐ CHANGE THIS TO FIX AXIS MAPPING ⭐
#define SWAP_AXES 0  // Set to 1 if roll and pitch seem swapped
// Define a special setting for the thumb (finger index 0)
#define SWAP_AXES_THUMB 1  // Special swap for thumb only

/* Fixed compute_angles function with axis swapping option */
void compute_angles_fixed(s16 ax, s16 ay, s16 az,
                         float* out_roll_deg, float* out_pitch_deg,
                         u8 finger_idx);

#endif /* AXIS_MAPPING_FIX_H */
