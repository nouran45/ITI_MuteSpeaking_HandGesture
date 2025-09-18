#ifndef SENSOR_DEBUG_H
#define SENSOR_DEBUG_H

#include "STD_TYPES.h"

/* Debug function declarations */
void debug_sensor_orientation(void);
void debug_finger_bending(u8 finger_index);
void debug_axis_magnitude(void);
void run_sensor_debug_menu(void);

#endif /* SENSOR_DEBUG_H */
