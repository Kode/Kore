#pragma once

#include <IOKit/IOKitLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDManager.h>

#define HID_AXIS_COUNT   8
#define HID_BUTTON_COUNT 15

struct HIDGamepad {
	int padIndex;
	IOHIDDeviceRef hidDeviceRef;
	IOHIDQueueRef hidQueueRef;
	int hidDeviceVendorID;
	int hidDeviceProductID;
	char hidDeviceVendor[64];
	char hidDeviceProduct[64];

	IOHIDElementCookie axis[HID_AXIS_COUNT];
	IOHIDElementCookie buttons[HID_BUTTON_COUNT];
	IOHIDElementCookie hatSwitchCookie;
	bool hatUp, hatDown, hatLeft, hatRight;
};

void HIDGamepad_init(struct HIDGamepad *gamepad);
void HIDGamepad_destroy(struct HIDGamepad *gamepad);
void HIDGamepad_bind(struct HIDGamepad *gamepad, IOHIDDeviceRef deviceRef, int padIndex);
void HIDGamepad_unbind(struct HIDGamepad *gamepad);
