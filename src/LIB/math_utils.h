// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "STD_TYPES.h"

f32 math_clamp(f32 value, f32 min_val, f32 max_val);
f32 math_find_min(const f32* data, u8 size);
f32 math_find_max(const f32* data, u8 size);
f32 math_calculate_range(const f32* data, u8 size);
u8  math_min_max_normalize(f32* data, u8 size, f32 target_min, f32 target_max);
u8  math_normalize_to_range(f32* data, u8 size, f32 current_min, f32 current_max, 
                          f32 target_min, f32 target_max);
u8  math_z_score_normalize(f32* data, u8 size);
f32 math_calculate_mean(const f32* data, u8 size);
f32 math_calculate_rms(const f32* data, u8 size);
u8  math_is_valid_float(f32 value);
f32 math_calculate_std(const f32* data, u8 size, f32 mean);
f32 math_calculate_correlation(const f32* x_data, const f32* y_data, u8 size);
f32 math_calculate_velocity(f32 current, f32 previous, u32 dt);
f32 math_calculate_acceleration(f32 current_vel, f32 previous_vel, u32 dt);

#endif