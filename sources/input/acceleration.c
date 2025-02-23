#include <kore3/input/acceleration.h>

static void (*acceleration_callback)(float /*x*/, float /*y*/, float /*z*/) = NULL;

void kore_acceleration_set_callback(void (*value)(float /*x*/, float /*y*/, float /*z*/)) {
	acceleration_callback = value;
}

void kore_internal_on_acceleration(float x, float y, float z) {
	if (acceleration_callback != NULL) {
		acceleration_callback(x, y, z);
	}
}
