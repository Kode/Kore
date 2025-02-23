#include <kore3/input/rotation.h>

static void (*rotation_callback)(float /*x*/, float /*y*/, float /*z*/) = NULL;

void kore_rotation_set_callback(void (*value)(float /*x*/, float /*y*/, float /*z*/)) {
	rotation_callback = value;
}

void kore_internal_on_rotation(float x, float y, float z) {
	if (rotation_callback != NULL) {
		rotation_callback(x, y, z);
	}
}
