#include "STD_TYPES.h"
#include "math_utils.h"
#include <math.h>

/* Explicit handling for isnan/isinf if math.h doesn't provide them */
#ifndef isnan
#define isnan(x) ((x) != (x))
#endif
#ifndef isinf
#define isinf(x) (((x) == INFINITY) || ((x) == -INFINITY))
#endif

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
    // Input validation
    if (!x_data || !y_data || size == 0) {
        return 0.0f;
    }
    
    // Validate all values are valid numbers
    for (u8 i = 0; i < size; i++) {
        if (isnan(x_data[i]) || isnan(y_data[i]) || 
            isinf(x_data[i]) || isinf(y_data[i])) {
            return 0.0f; // Invalid input data
        }
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
    
    // Robust division by zero protection with epsilon
    const f32 EPSILON = 1e-10f;
    if (sum_sq_x < EPSILON || sum_sq_y < EPSILON) {
        return 0.0f;  // No correlation if either variable has no variance
    }
    
    f32 denominator = sqrtf(sum_sq_x * sum_sq_y);
    
    // Double-check for potential division by zero
    if (denominator < EPSILON) {
        return 0.0f;
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
    // Input validation
    if (!data || size == 0) {
        return 0;
    }
    
    // Validate input ranges
    if (isnan(current_min) || isnan(current_max) || 
        isnan(target_min) || isnan(target_max) ||
        isinf(current_min) || isinf(current_max) ||
        isinf(target_min) || isinf(target_max)) {
        return 0;
    }
    
    // Ensure ranges are properly ordered
    if (current_min > current_max || target_min > target_max) {
        return 0;
    }
    
    f32 current_range = current_max - current_min;
    f32 target_range = target_max - target_min;
    
    // Check for invalid ranges with epsilon for floating-point comparison
    const f32 EPSILON = 1e-6f;
    if (current_range < EPSILON || target_range < EPSILON) {
        return 0;
    }
    
    // Normalize each value with NaN and infinity checking
    for (u8 i = 0; i < size; i++) {
        // Skip invalid values
        if (isnan(data[i]) || isinf(data[i])) {
            data[i] = target_min; // Default to minimum target value for invalid inputs
            continue;
        }
        
        // First normalize to [0,1], then scale to target range
        f32 normalized = (data[i] - current_min) / current_range;
        
        // Clamp normalized value to [0,1]
        normalized = fmaxf(0.0f, fminf(1.0f, normalized));
        
        data[i] = target_min + (normalized * target_range);
    }
    
    return 1;
}

u8 math_z_score_normalize(f32* data, u8 size) {
    // Input validation
    if (!data || size == 0) {
        return 0;
    }
    
    // Validate input data and replace invalid values with 0
    for (u8 i = 0; i < size; i++) {
        if (isnan(data[i]) || isinf(data[i])) {
            data[i] = 0.0f;
        }
    }
    
    // Calculate mean and standard deviation
    f32 mean = math_calculate_mean(data, size);
    f32 std = math_calculate_std(data, size, mean);
    
    // Check for zero or very small standard deviation (prevent division by zero)
    const f32 EPSILON = 1e-6f;
    if (std < EPSILON) {
        // If std is too small, set all values to 0 (centered)
        for (u8 i = 0; i < size; i++) {
            data[i] = 0.0f;
        }
        return 1;
    }
    
    // Apply z-score normalization with protection against overflow
    for (u8 i = 0; i < size; i++) {
        data[i] = (data[i] - mean) / std;
        
        // Clamp extremely large z-scores to reasonable range (-5 to 5)
        // This is somewhat arbitrary but avoids extreme outliers
        data[i] = fmaxf(-5.0f, fminf(5.0f, data[i]));
        
        // Final NaN check
        if (isnan(data[i]) || isinf(data[i])) {
            data[i] = 0.0f;
        }
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