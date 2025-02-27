#ifndef KORE_FREEBSD_GAMEPAD_HEADER
#define KORE_FREEBSD_GAMEPAD_HEADER

namespace Kore {
	void initHIDGamepads();
	void updateHIDGamepads();
	void closeHIDGamepads();
}

#endif
