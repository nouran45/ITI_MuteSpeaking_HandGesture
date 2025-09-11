#include "STD_TYPES.h"
#include "math_utils.h"
#include <math.h>

f32 math_calculate_mean(const f32* data, u8 size) {
    if (!data || size == 0) {
        return 0.0f;
    }
    
    f32 sum = 0.0f;
    for (u8 i = 0; i < size; i++) {
        sum += data[i];
    }
    
    return sum / size;
}

f32 math_calculate_std(const f32* data, u8 size, f32 mean) {
    if (!data || size == 0) {
        return 0.0f;
    }
    
    f32 sum_squared_diff = 0.0f;
    for (u8 i = 0; i < size; i++) {
        f32 diff = data[i] - mean;
        sum_squared_diff += diff * diff;
    }
    
    return sqrtf(sum_squared_diff / size);
}

f32 math_calculate_correlation(const f32* x_data, const f32* y_data, u8 size) {
    if (!x_data || !y_data || size == 0) {
        return 0.0f;
    }
    
    // Calculate means
    f32 mean_x = math_calculate_mean(x_data, size);
    f32 mean_y = math_calculate_mean(y_data, size);
    
    // Calculate correlation coefficient
    f32 numerator = 0.0f;
    f32 sum_sq_x = 0.0f;
    f32 sum_sq_y = 0.0f;
    
    for (u8 i = 0; i < size; i++) {
        f32 diff_x = x_data[i] - mean_x;
        f32 diff_y = y_data[i] - mean_y;
        
        numerator += diff_x * diff_y;
        sum_sq_x += diff_x * diff_x;
        sum_sq_y += diff_y * diff_y;
    }
    
    f32 denominator = sqrtf(sum_sq_x * sum_sq_y);
    
    if (denominator == 0.0f) {
        return 0.0f;  // No correlation if either variable has no variance
    }
    
    f32 correlation = numerator / denominator;
    
    // Clamp to valid correlation range [-1, 1]
    return math_clamp(correlation, -1.0f, 1.0f);
}

f32 math_calculate_velocity(f32 current, f32 previous, u32 dt) {
    if (dt == 0) {
        return 0.0f;
    }
    
    return (current - previous) / (f32)dt;
}

f32 math_calculate_acceleration(f32 current_vel, f32 previous_vel, u32 dt) {
    if (dt == 0) {
        return 0.0f;
    }
    
    return (current_vel - previous_vel) / (f32)dt;
}

f32 math_find_min(const f32* data, u8 size) {
    if (!data || size == 0) {
        return 0.0f;
    }
    
    f32 min_val = data[0];
    for (u8 i = 1; i < size; i++) {
        if (data[i] < min_val) {
            min_val = data[i];
        }
    }
    
    return min_val;
}

f32 math_find_max(const f32* data, u8 size) {
    if (!data || size == 0) {
        return 0.0f;
    }
    
    f32 max_val = data[0];
    for (u8 i = 1; i < size; i++) {
        if (data[i] > max_val) {
            max_val = data[i];
        }
    }
    
    return max_val;
}

f32 math_calculate_range(const f32* data, u8 size) {
    if (!data || size == 0) {
        return 0.0f;
    }
    
    f32 min_val = math_find_min(data, size);
    f32 max_val = math_find_max(data, size);
    
    return max_val - min_val;
}

u8 math_normalize_to_range(f32* data, u8 size, f32 current_min, f32 current_max, 
                          f32 target_min, f32 target_max) {
    if (!data || size == 0) {
        return 0;
    }
    
    f32 current_range = current_max - current_min;
    f32 target_range = target_max - target_min;
    
    // Check for invalid ranges
    if (current_range == 0.0f || target_range == 0.0f) {
        return 0;
    }
    
    // Normalize each value
    for (u8 i = 0; i < size; i++) {
        // First normalize to [0,1], then scale to target range
        f32 normalized = (data[i] - current_min) / current_range;
        data[i] = target_min + (normalized * target_range);
    }
    
    return 1;
}

u8 math_z_score_normalize(f32* data, u8 size) {
    if (!data || size == 0) {
        return 0;
    }
    
    // Calculate mean and standard deviation
    f32 mean = math_calculate_mean(data, size);
    f32 std = math_calculate_std(data, size, mean);
    
    // Check for zero standard deviation
    if (std == 0.0f) {
        return 0;
    }
    
    // Apply z-score normalization
    for (u8 i = 0; i < size; i++) {
        data[i] = (data[i] - mean) / std;
    }
    
    return 1;
}

u8 math_is_valid_float(f32 value) {
    // Check for NaN and infinite values
    if (isnan(value) || isinf(value)) {
        return 0;
    }
    return 1;
}

u8 math_validate_array(const f32* data, u8 size) {
    if (!data) {
        return 0;
    }
    
    for (u8 i = 0; i < size; i++) {
        if (!math_is_valid_float(data[i])) {
            return 0;
        }
    }
    
    return 1;
}

f32 math_clamp(f32 value, f32 min_val, f32 max_val) {
    if (value < min_val) {
        return min_val;
    } else if (value > max_val) {
        return max_val;
    } else {
        return value;
    }
}

f32 math_lerp(f32 start, f32 end, f32 t) {
    // Clamp t to [0, 1] range
    t = math_clamp(t, 0.0f, 1.0f);
    
    return start + t * (end - start);
}

f32 math_calculate_rms(const f32* data, u8 size) {
    if (!data || size == 0) {
        return 0.0f;
    }
    
    f32 sum_squares = 0.0f;
    for (u8 i = 0; i < size; i++) {
        sum_squares += data[i] * data[i];
    }
    
    return sqrtf(sum_squares / size);
}