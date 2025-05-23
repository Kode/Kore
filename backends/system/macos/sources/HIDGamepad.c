#include "HIDGamepad.h"
#include "HIDManager.h"

#include <kore3/error.h>
#include <kore3/input/gamepad.h>
#include <kore3/log.h>
#include <kore3/math/core.h>

static void inputValueCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef);
static void valueAvailableCallback(void *inContext, IOReturn inResult, void *inSender);

static void reset(struct HIDGamepad *gamepad);

static void initDeviceElements(struct HIDGamepad *gamepad, CFArrayRef elements);

static void buttonChanged(struct HIDGamepad *gamepad, IOHIDElementRef elementRef, IOHIDValueRef valueRef, int buttonIndex);
static void axisChanged(struct HIDGamepad *gamepad, IOHIDElementRef elementRef, IOHIDValueRef valueRef, int axisIndex);

static bool debugButtonInput = false;

static void logButton(int buttonIndex, bool pressed) {
	switch (buttonIndex) {
	case 0:
		kore_log(KORE_LOG_LEVEL_INFO, "A Pressed %i", pressed);
		break;

	case 1:
		kore_log(KORE_LOG_LEVEL_INFO, "B Pressed %i", pressed);
		break;

	case 2:
		kore_log(KORE_LOG_LEVEL_INFO, "X Pressed %i", pressed);
		break;

	case 3:
		kore_log(KORE_LOG_LEVEL_INFO, "Y Pressed %i", pressed);
		break;

	case 4:
		kore_log(KORE_LOG_LEVEL_INFO, "Lb Pressed %i", pressed);
		break;

	case 5:
		kore_log(KORE_LOG_LEVEL_INFO, "Rb Pressed %i", pressed);
		break;

	case 6:
		kore_log(KORE_LOG_LEVEL_INFO, "Left Stick Pressed %i", pressed);
		break;

	case 7:
		kore_log(KORE_LOG_LEVEL_INFO, "Right Stick Pressed %i", pressed);
		break;

	case 8:
		kore_log(KORE_LOG_LEVEL_INFO, "Start Pressed %i", pressed);
		break;

	case 9:
		kore_log(KORE_LOG_LEVEL_INFO, "Back Pressed %i", pressed);
		break;

	case 10:
		kore_log(KORE_LOG_LEVEL_INFO, "Home Pressed %i", pressed);
		break;

	case 11:
		kore_log(KORE_LOG_LEVEL_INFO, "Up Pressed %i", pressed);
		break;

	case 12:
		kore_log(KORE_LOG_LEVEL_INFO, "Down Pressed %i", pressed);
		break;

	case 13:
		kore_log(KORE_LOG_LEVEL_INFO, "Left Pressed %i", pressed);
		break;

	case 14:
		kore_log(KORE_LOG_LEVEL_INFO, "Right Pressed %i", pressed);
		break;

	default:
		break;
	}
}

static bool debugAxisInput = false;

static void logAxis(int axisIndex) {
	switch (axisIndex) {
	case 0:
		kore_log(KORE_LOG_LEVEL_INFO, "Left stick X");
		break;

	case 1:
		kore_log(KORE_LOG_LEVEL_INFO, "Left stick Y");
		break;

	case 2:
		kore_log(KORE_LOG_LEVEL_INFO, "Right stick X");
		break;

	case 3:
		kore_log(KORE_LOG_LEVEL_INFO, "Right stick Y");
		break;

	case 4:
		kore_log(KORE_LOG_LEVEL_INFO, "Left trigger");
		break;

	case 5:
		kore_log(KORE_LOG_LEVEL_INFO, "Right trigger");
		break;

	default:
		break;
	}
}

// Helper function to copy a CFStringRef to a cstring buffer.
// CFStringRef is converted to UTF8 and as many characters as possible are
// placed into the buffer followed by a null terminator.
// The buffer is set to an empty string if the conversion fails.
static void cstringFromCFStringRef(CFStringRef string, char *cstr, size_t clen) {
	cstr[0] = '\0';
	if (string != NULL) {
		char temp[256];
		if (CFStringGetCString(string, temp, 256, kCFStringEncodingUTF8)) {
			temp[kore_mini(255, (int)(clen - 1))] = '\0';
			strncpy(cstr, temp, clen);
		}
	}
}

void HIDGamepad_init(struct HIDGamepad *gamepad) {
	reset(gamepad);
}

void HIDGamepad_destroy(struct HIDGamepad *gamepad) {
	HIDGamepad_unbind(gamepad);
}

void HIDGamepad_bind(struct HIDGamepad *gamepad, IOHIDDeviceRef inDeviceRef, int inPadIndex) {
	kore_affirm(inDeviceRef != NULL);
	kore_affirm(inPadIndex >= 0);
	kore_affirm(gamepad->hidDeviceRef == NULL);
	kore_affirm(gamepad->hidQueueRef == NULL);
	kore_affirm(gamepad->padIndex == -1);

	// Set device and device index
	gamepad->hidDeviceRef = inDeviceRef;
	gamepad->padIndex     = inPadIndex;

	// Initialise HID Device
	// ...open device
	IOHIDDeviceOpen(gamepad->hidDeviceRef, kIOHIDOptionsTypeSeizeDevice);

	// ..register callbacks
	IOHIDDeviceRegisterInputValueCallback(gamepad->hidDeviceRef, inputValueCallback, gamepad);
	IOHIDDeviceScheduleWithRunLoop(gamepad->hidDeviceRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

	// ...create a queue to access element values
	gamepad->hidQueueRef = IOHIDQueueCreate(kCFAllocatorDefault, gamepad->hidDeviceRef, 32, kIOHIDOptionsTypeNone);
	if (CFGetTypeID(gamepad->hidQueueRef) == IOHIDQueueGetTypeID()) {
		IOHIDQueueStart(gamepad->hidQueueRef);
		IOHIDQueueRegisterValueAvailableCallback(gamepad->hidQueueRef, valueAvailableCallback, gamepad);
		IOHIDQueueScheduleWithRunLoop(gamepad->hidQueueRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	}

	// ...get all elements (buttons, axes)
	CFArrayRef elementCFArrayRef = IOHIDDeviceCopyMatchingElements(gamepad->hidDeviceRef, NULL, kIOHIDOptionsTypeNone);
	initDeviceElements(gamepad, elementCFArrayRef);

	// ...get device manufacturer and product details
	{
		CFNumberRef vendorIdRef = (CFNumberRef)IOHIDDeviceGetProperty(gamepad->hidDeviceRef, CFSTR(kIOHIDVendorIDKey));
		CFNumberGetValue(vendorIdRef, kCFNumberIntType, &gamepad->hidDeviceVendorID);

		CFNumberRef productIdRef = (CFNumberRef)IOHIDDeviceGetProperty(gamepad->hidDeviceRef, CFSTR(kIOHIDProductIDKey));
		CFNumberGetValue(productIdRef, kCFNumberIntType, &gamepad->hidDeviceProductID);

		CFStringRef vendorRef = (CFStringRef)IOHIDDeviceGetProperty(gamepad->hidDeviceRef, CFSTR(kIOHIDManufacturerKey));
		cstringFromCFStringRef(vendorRef, gamepad->hidDeviceVendor, sizeof(gamepad->hidDeviceVendor));

		CFStringRef productRef = (CFStringRef)IOHIDDeviceGetProperty(gamepad->hidDeviceRef, CFSTR(kIOHIDProductKey));
		cstringFromCFStringRef(productRef, gamepad->hidDeviceProduct, sizeof(gamepad->hidDeviceProduct));
	}

	// Initialise Kore::Gamepad for this HID Device
	//**
	/*Gamepad *gamepad = Gamepad::get(padIndex);
	gamepad->vendor 	 = hidDeviceVendor;
	gamepad->productName = hidDeviceProduct;*/

	kore_log(KORE_LOG_LEVEL_INFO, "HIDGamepad.bind: <%p> idx:%d [0x%x:0x%x] [%s] [%s]", inDeviceRef, gamepad->padIndex, gamepad->hidDeviceVendorID,
	         gamepad->hidDeviceProductID, gamepad->hidDeviceVendor, gamepad->hidDeviceProduct);
}

static void initDeviceElements(struct HIDGamepad *gamepad, CFArrayRef elements) {
	kore_affirm(elements != NULL);

	for (CFIndex i = 0, count = CFArrayGetCount(elements); i < count; ++i) {
		IOHIDElementRef  elementRef = (IOHIDElementRef)CFArrayGetValueAtIndex(elements, i);
		IOHIDElementType elemType   = IOHIDElementGetType(elementRef);

		IOHIDElementCookie cookie = IOHIDElementGetCookie(elementRef);

		uint32_t usagePage = IOHIDElementGetUsagePage(elementRef);
		uint32_t usage     = IOHIDElementGetUsage(elementRef);

		// Match up items
		switch (usagePage) {
		case kHIDPage_GenericDesktop:
			switch (usage) {
			case kHIDUsage_GD_X: // Left stick X
				// log(Info, "Left stick X axis[0] = %i", cookie);
				gamepad->axis[0] = cookie;
				break;
			case kHIDUsage_GD_Y: // Left stick Y
				// log(Info, "Left stick Y axis[1] = %i", cookie);
				gamepad->axis[1] = cookie;
				break;
			case kHIDUsage_GD_Z: // Left trigger
				// log(Info, "Left trigger axis[4] = %i", cookie);
				gamepad->axis[4] = cookie;
				break;
			case kHIDUsage_GD_Rx: // Right stick X
				// log(Info, "Right stick X axis[2] = %i", cookie);
				gamepad->axis[2] = cookie;
				break;
			case kHIDUsage_GD_Ry: // Right stick Y
				// log(Info, "Right stick Y axis[3] = %i", cookie);
				gamepad->axis[3] = cookie;
				break;
			case kHIDUsage_GD_Rz: // Right trigger
				// log(Info, "Right trigger axis[5] = %i", cookie);
				gamepad->axis[5] = cookie;
				break;
			case kHIDUsage_GD_Hatswitch:
				break;
			default:
				break;
			}
			break;
		case kHIDPage_Button:
			if ((usage >= 1) && (usage <= 15)) {
				// Button 1-11
				gamepad->buttons[usage - 1] = cookie;
				// log(Info, "Button %i = %i", usage-1, cookie);
			}
			break;
		default:
			break;
		}

		if (elemType == kIOHIDElementTypeInput_Misc || elemType == kIOHIDElementTypeInput_Button || elemType == kIOHIDElementTypeInput_Axis) {
			if (!IOHIDQueueContainsElement(gamepad->hidQueueRef, elementRef))
				IOHIDQueueAddElement(gamepad->hidQueueRef, elementRef);
		}
	}
}

void HIDGamepad_unbind(struct HIDGamepad *gamepad) {
	kore_log(KORE_LOG_LEVEL_INFO, "HIDGamepad.unbind: idx:%d [0x%x:0x%x] [%s] [%s]", gamepad->padIndex, gamepad->hidDeviceVendorID, gamepad->hidDeviceProductID,
	         gamepad->hidDeviceVendor, gamepad->hidDeviceProduct);

	if (gamepad->hidQueueRef) {
		IOHIDQueueStop(gamepad->hidQueueRef);
		IOHIDQueueUnscheduleFromRunLoop(gamepad->hidQueueRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	}

	if (gamepad->hidDeviceRef) {
		IOHIDDeviceUnscheduleFromRunLoop(gamepad->hidDeviceRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		IOHIDDeviceClose(gamepad->hidDeviceRef, kIOHIDOptionsTypeSeizeDevice);
	}

	if (gamepad->padIndex >= 0) {
		//**
		/*Gamepad *gamepad = Gamepad::get(padIndex);
		gamepad->vendor 	 = nullptr;
		gamepad->productName = nullptr;*/
	}

	reset(gamepad);
}

static void reset(struct HIDGamepad *gamepad) {
	gamepad->padIndex            = -1;
	gamepad->hidDeviceRef        = NULL;
	gamepad->hidQueueRef         = NULL;
	gamepad->hidDeviceVendor[0]  = '\0';
	gamepad->hidDeviceProduct[0] = '\0';
	gamepad->hidDeviceVendorID   = 0;
	gamepad->hidDeviceProductID  = 0;

	memset(gamepad->axis, 0, sizeof(gamepad->axis));
	memset(gamepad->buttons, 0, sizeof(gamepad->buttons));
}

static void buttonChanged(struct HIDGamepad *gamepad, IOHIDElementRef elementRef, IOHIDValueRef valueRef, int buttonIndex) {
	// double rawValue = IOHIDValueGetIntegerValue(valueRef);
	double rawValue = IOHIDValueGetScaledValue(valueRef, kIOHIDValueScaleTypePhysical);

	// Normalize button value to the range [0.0, 1.0] (0 - release, 1 - pressed)
	double min       = IOHIDElementGetLogicalMin(elementRef);
	double max       = IOHIDElementGetLogicalMax(elementRef);
	double normalize = (rawValue - min) / (max - min);

	// log(Info, "%f %f %f %f", rawValue, min, max, normalize);

	kore_internal_gamepad_trigger_button(gamepad->padIndex, buttonIndex, normalize);

	if (debugButtonInput)
		logButton(buttonIndex, (normalize != 0));
}

static void axisChanged(struct HIDGamepad *gamepad, IOHIDElementRef elementRef, IOHIDValueRef valueRef, int axisIndex) {
	// double rawValue = IOHIDValueGetIntegerValue(valueRef);
	double rawValue = IOHIDValueGetScaledValue(valueRef, kIOHIDValueScaleTypePhysical);

	// Normalize axis value to the range [-1.0, 1.0] (e.g. -1 - left, 0 - release, 1 - right)
	double min       = IOHIDElementGetPhysicalMin(elementRef);
	double max       = IOHIDElementGetPhysicalMax(elementRef);
	double normalize = normalize = (((rawValue - min) / (max - min)) * 2) - 1;

	// Invert Y axis
	if (axisIndex % 2 == 1)
		normalize = -normalize;

	// log(Info, "%f %f %f %f", rawValue, min, max, normalize);

	kore_internal_gamepad_trigger_axis(gamepad->padIndex, axisIndex, normalize);

	if (debugAxisInput)
		logAxis(axisIndex);
}

static void inputValueCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef) {}

static void valueAvailableCallback(void *inContext, IOReturn inResult, void *inSender) {
	struct HIDGamepad *pad = (struct HIDGamepad *)inContext;
	do {
		IOHIDValueRef valueRef = IOHIDQueueCopyNextValueWithTimeout((IOHIDQueueRef)inSender, 0.);
		if (!valueRef)
			break;

		// process the HID value reference
		IOHIDElementRef elementRef = IOHIDValueGetElement(valueRef);
		// IOHIDElementType elemType = IOHIDElementGetType(elementRef);
		// log(Info, "Type %d %d\n", elemType, elementRef);

		IOHIDElementCookie cookie = IOHIDElementGetCookie(elementRef);
		// uint32_t page = IOHIDElementGetUsagePage(elementRef);
		// uint32_t usage = IOHIDElementGetUsage(elementRef);
		// log(Info, "page %i, usage %i cookie %i", page, usage, cookie);

		// Check button
		for (int i = 0, c = sizeof(pad->buttons); i < c; ++i) {
			if (cookie == pad->buttons[i]) {
				buttonChanged(pad, elementRef, valueRef, i);
				break;
			}
		}

		// Check axes
		for (int i = 0, c = sizeof(pad->axis); i < c; ++i) {
			if (cookie == pad->axis[i]) {
				axisChanged(pad, elementRef, valueRef, i);
				break;
			}
		}

		CFRelease(valueRef);
	} while (1);
}

const char *kore_gamepad_vendor(int gamepad) {
	return "unknown";
}

const char *kore_gamepad_product_name(int gamepad) {
	return "unknown";
}
