#define EGL_NO_PLATFORM_SPECIFIC_TYPES
#include <EGL/egl.h>
#include <kore3/error.h>
// #include <GLContext.h>
#include <kore3/backend/android.h>
// #include <kore3/graphics4/graphics.h>
#include <kore3/input/gamepad.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
// #include <kore3/input/sensor.h>
#include <android/sensor.h>
#include <android/window.h>
#include <kore3/backend/android_native_app_glue.h>
#include <kore3/input/pen.h>
#include <kore3/input/surface.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/threads/mutex.h>
#include <kore3/video.h>
#include <kore3/window.h>

#include <unistd.h>

#include <stdlib.h>

void pauseAudio();
void resumeAudio();

static struct android_app *app = NULL;
static ANativeActivity *activity = NULL;
static ASensorManager *sensorManager = NULL;
static const ASensor *accelerometerSensor = NULL;
static const ASensor *gyroSensor = NULL;
static ASensorEventQueue *sensorEventQueue = NULL;

static bool started = false;
static bool paused = true;
static bool displayIsInitialized = false;
static bool appIsForeground = false;
static bool activityJustResized = false;

#include <assert.h>

#ifdef KORE_EGL

EGLDisplay kore_egl_get_display() {
	return eglGetDisplay(EGL_DEFAULT_DISPLAY);
}

EGLNativeWindowType kore_egl_get_native_window(EGLDisplay display, EGLConfig config, int window) {
	kore_affirm(window == 0);
	EGLint format;
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
	int e = ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);
	if (e < 0) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Failed to set ANativeWindow buffer geometry.");
	}
	return app->window;
}

#endif

#ifdef KORE_VULKAN

#include <vulkan/vulkan_android.h>
#include <vulkan/vulkan_core.h>

VkResult kore_vulkan_create_surface(VkInstance instance, int window_index, VkSurfaceKHR *surface) {
	assert(app->window != NULL);
	VkAndroidSurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.window = app->window;
	return vkCreateAndroidSurfaceKHR(instance, &createInfo, NULL, surface);
}

void kore_vulkan_get_instance_extensions(const char **names, int *index, int max) {
	assert(*index + 1 < max);
	names[(*index)++] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
}

VkBool32 kore_vulkan_get_physical_device_presentation_support(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
	// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VK_KHR_android_surface.html#_issues
	//
	// 1) Does Android need a way to query for compatibility between a particular physical device (and queue family?)
	// and a specific Android display?
	// RESOLVED: No. Currently on Android, any physical device is expected to be able to present to the system compositor,
	// and all queue families must support the necessary image layout transitions and synchronization operations.
	return true;
}
#endif

#ifndef KORE_VULKAN
void kore_egl_init_window(int window);
void kore_egl_destroy_window(int window);
#endif
#ifdef KORE_VULKAN
void kore_vulkan_init_window(int window);
#endif

static void initDisplay() {
#ifndef KORE_VULKAN
	// kore_egl_init_window(0); // TODO
#endif
#ifdef KORE_VULKAN
	// kore_vulkan_init_window(0); // TODO
#endif
}

static void termDisplay() {
#ifndef KORE_VULKAN
	// kore_egl_destroy_window(0); // TODO
#endif
}

static void updateAppForegroundStatus(bool displayIsInitializedValue, bool appIsForegroundValue) {
	bool oldStatus = displayIsInitialized && appIsForeground;
	displayIsInitialized = displayIsInitializedValue;
	appIsForeground = appIsForegroundValue;
	bool newStatus = displayIsInitialized && appIsForeground;
	if (oldStatus != newStatus) {
		if (newStatus) {
			kore_internal_foreground_callback();
		}
		else {
			kore_internal_background_callback();
		}
	}
}

static bool isGamepadEvent(AInputEvent *event) {
	return ((AInputEvent_getSource(event) & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD ||
	        (AInputEvent_getSource(event) & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK ||
	        (AInputEvent_getSource(event) & AINPUT_SOURCE_DPAD) == AINPUT_SOURCE_DPAD);
}

static bool isPenEvent(AInputEvent *event) {
	return (AInputEvent_getSource(event) & AINPUT_SOURCE_STYLUS) == AINPUT_SOURCE_STYLUS;
}

static void touchInput(AInputEvent *event) {
	int action = AMotionEvent_getAction(event);
	int index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
	int id = AMotionEvent_getPointerId(event, index);
	float x = AMotionEvent_getX(event, index);
	float y = AMotionEvent_getY(event, index);
	switch (action & AMOTION_EVENT_ACTION_MASK) {
	case AMOTION_EVENT_ACTION_DOWN:
	case AMOTION_EVENT_ACTION_POINTER_DOWN:
		if (id == 0) {
			kore_internal_mouse_trigger_press(0, 0, x, y);
		}
		if (isPenEvent(event)) {
			kore_internal_pen_trigger_press(0, x, y, AMotionEvent_getPressure(event, index));
		}
		kore_internal_surface_trigger_touch_start(id, x, y);
		break;
	case AMOTION_EVENT_ACTION_MOVE:
	case AMOTION_EVENT_ACTION_HOVER_MOVE: {
		size_t count = AMotionEvent_getPointerCount(event);
		for (int i = 0; i < count; ++i) {
			id = AMotionEvent_getPointerId(event, i);
			x = AMotionEvent_getX(event, i);
			y = AMotionEvent_getY(event, i);
			if (id == 0) {
				kore_internal_mouse_trigger_move(0, x, y);
			}
			if (isPenEvent(event)) {
				kore_internal_pen_trigger_move(0, x, y, AMotionEvent_getPressure(event, index));
			}
			kore_internal_surface_trigger_move(id, x, y);
		}
	} break;
	case AMOTION_EVENT_ACTION_UP:
	case AMOTION_EVENT_ACTION_CANCEL:
	case AMOTION_EVENT_ACTION_POINTER_UP:
		if (id == 0) {
			kore_internal_mouse_trigger_release(0, 0, x, y);
		}
		if (isPenEvent(event)) {
			kore_internal_pen_trigger_release(0, x, y, AMotionEvent_getPressure(event, index));
		}
		kore_internal_surface_trigger_touch_end(id, x, y);
		break;
	case AMOTION_EVENT_ACTION_SCROLL:
		if (id == 0) {
			float scroll = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_VSCROLL, 0);
			kore_internal_mouse_trigger_scroll(0, -(int)scroll);
		}
		break;
	}
}

static float last_x = 0.0f;
static float last_y = 0.0f;
static float last_l = 0.0f;
static float last_r = 0.0f;
static bool last_hat_left = false;
static bool last_hat_right = false;
static bool last_hat_up = false;
static bool last_hat_down = false;

static int32_t input(struct android_app *app, AInputEvent *event) {
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		int source = AInputEvent_getSource(event);
		if (((source & AINPUT_SOURCE_TOUCHSCREEN) == AINPUT_SOURCE_TOUCHSCREEN) || ((source & AINPUT_SOURCE_MOUSE) == AINPUT_SOURCE_MOUSE)) {
			touchInput(event);
			return 1;
		}
		else if ((source & AINPUT_SOURCE_JOYSTICK) == AINPUT_SOURCE_JOYSTICK) {
			// int id = AInputEvent_getDeviceId(event);

			float x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_X, 0);
			if (x != last_x) {
				kore_internal_gamepad_trigger_axis(0, 0, x);
				last_x = x;
			}

			float y = -AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_Y, 0);
			if (y != last_y) {
				kore_internal_gamepad_trigger_axis(0, 1, y);
				last_y = y;
			}

			float l = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_LTRIGGER, 0);
			if (l != last_l) {
				kore_internal_gamepad_trigger_button(0, 6, l);
				last_l = l;
			}

			float r = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_RTRIGGER, 0);
			if (r != last_r) {
				kore_internal_gamepad_trigger_button(0, 7, r);
				last_r = r;
			}

			float hat_x = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_X, 0);

			bool hat_left = false;
			bool hat_right = false;
			if (hat_x < -0.5f) {
				hat_left = true;
			}
			else if (hat_x > 0.5f) {
				hat_right = true;
			}

			float hat_y = AMotionEvent_getAxisValue(event, AMOTION_EVENT_AXIS_HAT_Y, 0);

			bool hat_up = false;
			bool hat_down = false;
			if (hat_y < -0.5f) {
				hat_up = true;
			}
			else if (hat_y > 0.5f) {
				hat_down = true;
			}

			if (hat_left != last_hat_left) {
				kore_internal_gamepad_trigger_button(0, 14, hat_left ? 1.0f : 0.0f);
				last_hat_left = hat_left;
			}

			if (hat_right != last_hat_right) {
				kore_internal_gamepad_trigger_button(0, 15, hat_right ? 1.0f : 0.0f);
				last_hat_right = hat_right;
			}

			if (hat_up != last_hat_up) {
				kore_internal_gamepad_trigger_button(0, 12, hat_up ? 1.0f : 0.0f);
				last_hat_up = hat_up;
			}

			if (hat_down != last_hat_down) {
				kore_internal_gamepad_trigger_button(0, 13, hat_down ? 1.0f : 0.0f);
				last_hat_down = hat_down;
			}

			return 1;
		}
	}
	else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
		int32_t code = AKeyEvent_getKeyCode(event);

		if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN) {
			int shift = AKeyEvent_getMetaState(event) & AMETA_SHIFT_ON;
			if (shift) {
				switch (code) {
				case AKEYCODE_1:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_EXCLAMATION);
					kore_internal_keyboard_trigger_key_press('!');
					return 1;
				case AKEYCODE_4:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_DOLLAR);
					kore_internal_keyboard_trigger_key_press('$');
					return 1;
				case AKEYCODE_5:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_PERCENT);
					kore_internal_keyboard_trigger_key_press('%');
					return 1;
				case AKEYCODE_6:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_CIRCUMFLEX);
					kore_internal_keyboard_trigger_key_press('^');
					return 1;
				case AKEYCODE_7:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_AMPERSAND);
					kore_internal_keyboard_trigger_key_press('&');
					return 1;
				case AKEYCODE_9:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_OPEN_PAREN);
					kore_internal_keyboard_trigger_key_press('(');
					return 1;
				case AKEYCODE_0:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_CLOSE_PAREN);
					kore_internal_keyboard_trigger_key_press(')');
					return 1;
				case AKEYCODE_COMMA:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_LESS_THAN);
					kore_internal_keyboard_trigger_key_press('<');
					return 1;
				case AKEYCODE_PERIOD:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_GREATER_THAN);
					kore_internal_keyboard_trigger_key_press('>');
					return 1;
				case AKEYCODE_MINUS:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_UNDERSCORE);
					kore_internal_keyboard_trigger_key_press('_');
					return 1;
				case AKEYCODE_SLASH:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_QUESTIONMARK);
					kore_internal_keyboard_trigger_key_press('?');
					return 1;
				case AKEYCODE_BACKSLASH:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_PIPE);
					kore_internal_keyboard_trigger_key_press('|');
					return 1;
				case AKEYCODE_LEFT_BRACKET:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_OPEN_CURLY_BRACKET);
					kore_internal_keyboard_trigger_key_press('{');
					return 1;
				case AKEYCODE_RIGHT_BRACKET:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_CLOSE_CURLY_BRACKET);
					kore_internal_keyboard_trigger_key_press('}');
					return 1;
				case AKEYCODE_SEMICOLON:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_COLON);
					kore_internal_keyboard_trigger_key_press(':');
					return 1;
				case AKEYCODE_APOSTROPHE:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_DOUBLE_QUOTE);
					kore_internal_keyboard_trigger_key_press('"');
					return 1;
				case AKEYCODE_GRAVE:
					kore_internal_keyboard_trigger_key_down(KORE_KEY_TILDE);
					kore_internal_keyboard_trigger_key_press('~');
					return 1;
				}
			}
			switch (code) {
			case AKEYCODE_SHIFT_LEFT:
			case AKEYCODE_SHIFT_RIGHT:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_SHIFT);
				return 1;
			case AKEYCODE_DEL:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_BACKSPACE);
				return 1;
			case AKEYCODE_ENTER:
			case AKEYCODE_NUMPAD_ENTER:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_RETURN);
				return 1;
			case AKEYCODE_DPAD_CENTER:
			case AKEYCODE_BUTTON_B:
				kore_internal_gamepad_trigger_button(0, 1, 1);
				return 1;
			case AKEYCODE_BACK:
				if (AKeyEvent_getMetaState(event) & AMETA_ALT_ON) { // Xperia Play
					kore_internal_gamepad_trigger_button(0, 1, 1);
					return 1;
				}
				else {
					kore_internal_keyboard_trigger_key_down(KORE_KEY_BACK);
					return 1;
				}
			case AKEYCODE_BUTTON_A:
				kore_internal_gamepad_trigger_button(0, 0, 1);
				return 1;
			case AKEYCODE_BUTTON_Y:
				kore_internal_gamepad_trigger_button(0, 3, 1);
				return 1;
			case AKEYCODE_BUTTON_X:
				kore_internal_gamepad_trigger_button(0, 2, 1);
				return 1;
			case AKEYCODE_BUTTON_L1:
				kore_internal_gamepad_trigger_button(0, 4, 1);
				return 1;
			case AKEYCODE_BUTTON_R1:
				kore_internal_gamepad_trigger_button(0, 5, 1);
				return 1;
			case AKEYCODE_BUTTON_L2:
				kore_internal_gamepad_trigger_button(0, 6, 1);
				return 1;
			case AKEYCODE_BUTTON_R2:
				kore_internal_gamepad_trigger_button(0, 7, 1);
				return 1;
			case AKEYCODE_BUTTON_SELECT:
				kore_internal_gamepad_trigger_button(0, 8, 1);
				return 1;
			case AKEYCODE_BUTTON_START:
				kore_internal_gamepad_trigger_button(0, 9, 1);
				return 1;
			case AKEYCODE_BUTTON_THUMBL:
				kore_internal_gamepad_trigger_button(0, 10, 1);
				return 1;
			case AKEYCODE_BUTTON_THUMBR:
				kore_internal_gamepad_trigger_button(0, 11, 1);
				return 1;
			case AKEYCODE_DPAD_UP:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 12, 1);
				else
					kore_internal_keyboard_trigger_key_down(KORE_KEY_UP);
				return 1;
			case AKEYCODE_DPAD_DOWN:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 13, 1);
				else
					kore_internal_keyboard_trigger_key_down(KORE_KEY_DOWN);
				return 1;
			case AKEYCODE_DPAD_LEFT:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 14, 1);
				else
					kore_internal_keyboard_trigger_key_down(KORE_KEY_LEFT);
				return 1;
			case AKEYCODE_DPAD_RIGHT:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 15, 1);
				else
					kore_internal_keyboard_trigger_key_down(KORE_KEY_RIGHT);
				return 1;
			case AKEYCODE_BUTTON_MODE:
				kore_internal_gamepad_trigger_button(0, 16, 1);
				return 1;
			case AKEYCODE_STAR:
			case AKEYCODE_NUMPAD_MULTIPLY:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_MULTIPLY);
				kore_internal_keyboard_trigger_key_press('*');
				return 1;
			case AKEYCODE_POUND:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_HASH);
				kore_internal_keyboard_trigger_key_press('#');
				return 1;
			case AKEYCODE_COMMA:
			case AKEYCODE_NUMPAD_COMMA:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_COMMA);
				kore_internal_keyboard_trigger_key_press(',');
				return 1;
			case AKEYCODE_PERIOD:
			case AKEYCODE_NUMPAD_DOT:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_PERIOD);
				kore_internal_keyboard_trigger_key_press('.');
				return 1;
			case AKEYCODE_SPACE:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_SPACE);
				kore_internal_keyboard_trigger_key_press(' ');
				return 1;
			case AKEYCODE_MINUS:
			case AKEYCODE_NUMPAD_SUBTRACT:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_HYPHEN_MINUS);
				kore_internal_keyboard_trigger_key_press('-');
				return 1;
			case AKEYCODE_EQUALS:
			case AKEYCODE_NUMPAD_EQUALS:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_EQUALS);
				kore_internal_keyboard_trigger_key_press('=');
				return 1;
			case AKEYCODE_LEFT_BRACKET:
			case AKEYCODE_NUMPAD_LEFT_PAREN:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_OPEN_BRACKET);
				kore_internal_keyboard_trigger_key_press('[');
				return 1;
			case AKEYCODE_RIGHT_BRACKET:
			case AKEYCODE_NUMPAD_RIGHT_PAREN:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_CLOSE_BRACKET);
				kore_internal_keyboard_trigger_key_press(']');
				return 1;
			case AKEYCODE_BACKSLASH:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_BACK_SLASH);
				kore_internal_keyboard_trigger_key_press('\\');
				return 1;
			case AKEYCODE_SEMICOLON:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_SEMICOLON);
				kore_internal_keyboard_trigger_key_press(';');
				return 1;
			case AKEYCODE_APOSTROPHE:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_QUOTE);
				kore_internal_keyboard_trigger_key_press('\'');
				return 1;
			case AKEYCODE_GRAVE:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_BACK_QUOTE);
				kore_internal_keyboard_trigger_key_press('`');
				return 1;
			case AKEYCODE_SLASH:
			case AKEYCODE_NUMPAD_DIVIDE:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_SLASH);
				kore_internal_keyboard_trigger_key_press('/');
				return 1;
			case AKEYCODE_AT:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_AT);
				kore_internal_keyboard_trigger_key_press('@');
				return 1;
			case AKEYCODE_PLUS:
			case AKEYCODE_NUMPAD_ADD:
				kore_internal_keyboard_trigger_key_down(KORE_KEY_PLUS);
				kore_internal_keyboard_trigger_key_press('+');
				return 1;
			// (DK) Amazon FireTV remote/controller mappings
			// (DK) TODO handle multiple pads (up to 4 possible)
			case AKEYCODE_MENU:
				kore_internal_gamepad_trigger_button(0, 9, 1);
				return 1;
			case AKEYCODE_MEDIA_REWIND:
				kore_internal_gamepad_trigger_button(0, 10, 1);
				return 1;
			case AKEYCODE_MEDIA_FAST_FORWARD:
				kore_internal_gamepad_trigger_button(0, 11, 1);
				return 1;
			case AKEYCODE_MEDIA_PLAY_PAUSE:
				kore_internal_gamepad_trigger_button(0, 12, 1);
				return 1;
			// (DK) /Amazon FireTV remote/controller mappings
			default:
				if (code >= AKEYCODE_NUMPAD_0 && code <= AKEYCODE_NUMPAD_9) {
					kore_internal_keyboard_trigger_key_down(code + KORE_KEY_NUMPAD_0 - AKEYCODE_NUMPAD_0);
					kore_internal_keyboard_trigger_key_press(code + KORE_KEY_NUMPAD_0 - AKEYCODE_NUMPAD_0);
					return 1;
				}
				else if (code >= AKEYCODE_0 && code <= AKEYCODE_9) {
					kore_internal_keyboard_trigger_key_down(code + KORE_KEY_0 - AKEYCODE_0);
					kore_internal_keyboard_trigger_key_press(code + KORE_KEY_0 - AKEYCODE_0);
					return 1;
				}
				else if (code >= AKEYCODE_A && code <= AKEYCODE_Z) {
					kore_internal_keyboard_trigger_key_down(code + KORE_KEY_A - AKEYCODE_A);
					kore_internal_keyboard_trigger_key_press(code + (shift ? 'A' : 'a') - AKEYCODE_A);
					return 1;
				}
			}
		}
		else if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_UP) {
			int shift = AKeyEvent_getMetaState(event) & AMETA_SHIFT_ON;
			if (shift) {
				switch (code) {
				case AKEYCODE_1:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_EXCLAMATION);
					return 1;
				case AKEYCODE_4:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_DOLLAR);
					return 1;
				case AKEYCODE_5:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_PERCENT);
					return 1;
				case AKEYCODE_6:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_CIRCUMFLEX);
					return 1;
				case AKEYCODE_7:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_AMPERSAND);
					return 1;
				case AKEYCODE_9:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_OPEN_PAREN);
					return 1;
				case AKEYCODE_0:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_CLOSE_PAREN);
					return 1;
				case AKEYCODE_COMMA:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_LESS_THAN);
					return 1;
				case AKEYCODE_PERIOD:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_GREATER_THAN);
					return 1;
				case AKEYCODE_MINUS:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_UNDERSCORE);
					return 1;
				case AKEYCODE_SLASH:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_QUESTIONMARK);
					return 1;
				case AKEYCODE_BACKSLASH:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_PIPE);
					return 1;
				case AKEYCODE_LEFT_BRACKET:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_OPEN_CURLY_BRACKET);
					return 1;
				case AKEYCODE_RIGHT_BRACKET:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_CLOSE_CURLY_BRACKET);
					return 1;
				case AKEYCODE_SEMICOLON:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_COLON);
					return 1;
				case AKEYCODE_APOSTROPHE:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_DOUBLE_QUOTE);
					return 1;
				case AKEYCODE_GRAVE:
					kore_internal_keyboard_trigger_key_up(KORE_KEY_TILDE);
					return 1;
				}
			}
			switch (code) {
			case AKEYCODE_SHIFT_LEFT:
			case AKEYCODE_SHIFT_RIGHT:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_SHIFT);
				return 1;
			case AKEYCODE_DEL:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_BACKSPACE);
				return 1;
			case AKEYCODE_ENTER:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_RETURN);
				return 1;
			case AKEYCODE_DPAD_CENTER:
			case AKEYCODE_BUTTON_B:
				kore_internal_gamepad_trigger_button(0, 1, 0);
				return 1;
			case AKEYCODE_BACK:
				if (AKeyEvent_getMetaState(event) & AMETA_ALT_ON) { // Xperia Play
					kore_internal_gamepad_trigger_button(0, 1, 0);
					return 1;
				}
				else {
					kore_internal_keyboard_trigger_key_up(KORE_KEY_BACK);
					return 1;
				}
			case AKEYCODE_BUTTON_A:
				kore_internal_gamepad_trigger_button(0, 0, 0);
				return 1;
			case AKEYCODE_BUTTON_Y:
				kore_internal_gamepad_trigger_button(0, 3, 0);
				return 1;
			case AKEYCODE_BUTTON_X:
				kore_internal_gamepad_trigger_button(0, 2, 0);
				return 1;
			case AKEYCODE_BUTTON_L1:
				kore_internal_gamepad_trigger_button(0, 4, 0);
				return 1;
			case AKEYCODE_BUTTON_R1:
				kore_internal_gamepad_trigger_button(0, 5, 0);
				return 1;
			case AKEYCODE_BUTTON_L2:
				kore_internal_gamepad_trigger_button(0, 6, 0);
				return 1;
			case AKEYCODE_BUTTON_R2:
				kore_internal_gamepad_trigger_button(0, 7, 0);
				return 1;
			case AKEYCODE_BUTTON_SELECT:
				kore_internal_gamepad_trigger_button(0, 8, 0);
				return 1;
			case AKEYCODE_BUTTON_START:
				kore_internal_gamepad_trigger_button(0, 9, 0);
				return 1;
			case AKEYCODE_BUTTON_THUMBL:
				kore_internal_gamepad_trigger_button(0, 10, 0);
				return 1;
			case AKEYCODE_BUTTON_THUMBR:
				kore_internal_gamepad_trigger_button(0, 11, 0);
				return 1;
			case AKEYCODE_DPAD_UP:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 12, 0);
				else
					kore_internal_keyboard_trigger_key_up(KORE_KEY_UP);
				return 1;
			case AKEYCODE_DPAD_DOWN:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 13, 0);
				else
					kore_internal_keyboard_trigger_key_up(KORE_KEY_DOWN);
				return 1;
			case AKEYCODE_DPAD_LEFT:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 14, 0);
				else
					kore_internal_keyboard_trigger_key_up(KORE_KEY_LEFT);
				return 1;
			case AKEYCODE_DPAD_RIGHT:
				if (isGamepadEvent(event))
					kore_internal_gamepad_trigger_button(0, 15, 0);
				else
					kore_internal_keyboard_trigger_key_up(KORE_KEY_RIGHT);
				return 1;
			case AKEYCODE_BUTTON_MODE:
				kore_internal_gamepad_trigger_button(0, 16, 0);
				return 1;
			case AKEYCODE_STAR:
			case AKEYCODE_NUMPAD_MULTIPLY:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_MULTIPLY);
				return 1;
			case AKEYCODE_POUND:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_HASH);
				return 1;
			case AKEYCODE_COMMA:
			case AKEYCODE_NUMPAD_COMMA:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_COMMA);
				return 1;
			case AKEYCODE_PERIOD:
			case AKEYCODE_NUMPAD_DOT:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_PERIOD);
				return 1;
			case AKEYCODE_SPACE:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_SPACE);
				return 1;
			case AKEYCODE_MINUS:
			case AKEYCODE_NUMPAD_SUBTRACT:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_HYPHEN_MINUS);
				return 1;
			case AKEYCODE_EQUALS:
			case AKEYCODE_NUMPAD_EQUALS:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_EQUALS);
				return 1;
			case AKEYCODE_LEFT_BRACKET:
			case AKEYCODE_NUMPAD_LEFT_PAREN:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_OPEN_BRACKET);
				return 1;
			case AKEYCODE_RIGHT_BRACKET:
			case AKEYCODE_NUMPAD_RIGHT_PAREN:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_CLOSE_BRACKET);
				return 1;
			case AKEYCODE_BACKSLASH:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_BACK_SLASH);
				return 1;
			case AKEYCODE_SEMICOLON:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_SEMICOLON);
				return 1;
			case AKEYCODE_APOSTROPHE:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_QUOTE);
				return 1;
			case AKEYCODE_GRAVE:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_BACK_QUOTE);
				return 1;
			case AKEYCODE_SLASH:
			case AKEYCODE_NUMPAD_DIVIDE:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_SLASH);
				return 1;
			case AKEYCODE_AT:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_AT);
				return 1;
			case AKEYCODE_PLUS:
			case AKEYCODE_NUMPAD_ADD:
				kore_internal_keyboard_trigger_key_up(KORE_KEY_PLUS);
				return 1;
			// (DK) Amazon FireTV remote/controller mappings
			// (DK) TODO handle multiple pads (up to 4 possible)
			case AKEYCODE_MENU:
				kore_internal_gamepad_trigger_button(0, 9, 0);
				return 1;
			case AKEYCODE_MEDIA_REWIND:
				kore_internal_gamepad_trigger_button(0, 10, 0);
				return 1;
			case AKEYCODE_MEDIA_FAST_FORWARD:
				kore_internal_gamepad_trigger_button(0, 11, 0);
				return 1;
			case AKEYCODE_MEDIA_PLAY_PAUSE:
				kore_internal_gamepad_trigger_button(0, 12, 0);
				return 1;
			// (DK) /Amazon FireTV remote/controller mappings
			default:
				if (code >= AKEYCODE_NUMPAD_0 && code <= AKEYCODE_NUMPAD_9) {
					kore_internal_keyboard_trigger_key_up(code + KORE_KEY_NUMPAD_0 - AKEYCODE_NUMPAD_0);
					return 1;
				}
				else if (code >= AKEYCODE_0 && code <= AKEYCODE_9) {
					kore_internal_keyboard_trigger_key_up(code + KORE_KEY_0 - AKEYCODE_0);
					return 1;
				}
				else if (code >= AKEYCODE_A && code <= AKEYCODE_Z) {
					kore_internal_keyboard_trigger_key_up(code + KORE_KEY_A - AKEYCODE_A);
					return 1;
				}
			}
		}
	}
	return 0;
}

static void cmd(struct android_app *app, int32_t cmd) {
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		break;
	case APP_CMD_INIT_WINDOW:
		if (app->window != NULL) {
			if (!started) {
				started = true;
			}
			else {
				initDisplay();
				// kore_g4_swap_buffers(); // TODO
			}

			updateAppForegroundStatus(true, appIsForeground);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		termDisplay();
		updateAppForegroundStatus(false, appIsForeground);
		break;
	case APP_CMD_GAINED_FOCUS:
		if (accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(sensorEventQueue, accelerometerSensor);
			ASensorEventQueue_setEventRate(sensorEventQueue, accelerometerSensor, (1000L / 60) * 1000);
		}
		if (gyroSensor != NULL) {
			ASensorEventQueue_enableSensor(sensorEventQueue, gyroSensor);
			ASensorEventQueue_setEventRate(sensorEventQueue, gyroSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		if (accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(sensorEventQueue, accelerometerSensor);
		}
		if (gyroSensor != NULL) {
			ASensorEventQueue_disableSensor(sensorEventQueue, gyroSensor);
		}
		break;
	case APP_CMD_START:
		updateAppForegroundStatus(displayIsInitialized, true);
		break;
	case APP_CMD_RESUME:
		kore_internal_resume_callback();
		resumeAudio();
		paused = false;
		break;
	case APP_CMD_PAUSE:
		kore_internal_pause_callback();
		pauseAudio();
		paused = true;
		break;
	case APP_CMD_STOP:
		updateAppForegroundStatus(displayIsInitialized, false);
		break;
	case APP_CMD_DESTROY:
		kore_internal_shutdown_callback();
		break;
	case APP_CMD_CONFIG_CHANGED: {

		break;
	}
	}
}

static void resize(ANativeActivity *activity, ANativeWindow *window) {
	activityJustResized = true;
}

ANativeActivity *kore_android_get_activity(void) {
	return activity;
}

AAssetManager *kore_android_get_asset_manager(void) {
	return activity->assetManager;
}

jclass kore_android_find_class(JNIEnv *env, const char *name) {
	jobject nativeActivity = activity->clazz;
	jclass acl = (*env)->GetObjectClass(env, nativeActivity);
	jmethodID getClassLoader = (*env)->GetMethodID(env, acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject cls = (*env)->CallObjectMethod(env, nativeActivity, getClassLoader);
	jclass classLoader = (*env)->FindClass(env, "java/lang/ClassLoader");
	jmethodID findClass = (*env)->GetMethodID(env, classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	jstring strClassName = (*env)->NewStringUTF(env, name);
	jclass clazz = (jclass)((*env)->CallObjectMethod(env, cls, findClass, strClassName));
	(*env)->DeleteLocalRef(env, strClassName);
	return clazz;
}

#define UNICODE_STACK_SIZE 256
static uint16_t unicode_stack[UNICODE_STACK_SIZE];
static int unicode_stack_index = 0;
static kore_mutex unicode_mutex;

JNIEXPORT void JNICALL Java_tech_kore_KoreActivity_nativeKoreKeyPress(JNIEnv *env, jobject jobj, jstring chars) {
	const jchar *text = (*env)->GetStringChars(env, chars, NULL);
	const jsize length = (*env)->GetStringLength(env, chars);

	kore_mutex_lock(&unicode_mutex);
	for (jsize i = 0; i < length && unicode_stack_index < UNICODE_STACK_SIZE; ++i) {
		unicode_stack[unicode_stack_index++] = text[i];
	}
	kore_mutex_unlock(&unicode_mutex);

	(*env)->ReleaseStringChars(env, chars, text);
}

void KoreAndroidKeyboardInit() {
	JNIEnv *env;
	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);

	jclass clazz = kore_android_find_class(env, "tech.kore.KoreActivity");

	// String chars
	JNINativeMethod methodTable[] = {{"nativeKoreKeyPress", "(Ljava/lang/String;)V", (void *)Java_tech_kore_KoreActivity_nativeKoreKeyPress}};

	int methodTableSize = sizeof(methodTable) / sizeof(methodTable[0]);

	int failure = (*env)->RegisterNatives(env, clazz, methodTable, methodTableSize);
	if (failure != 0) {
		kore_log(KORE_LOG_LEVEL_WARNING, "Failed to register KoreActivity.nativeKoreKeyPress");
	}

	(*activity->vm)->DetachCurrentThread(activity->vm);
}

static bool keyboard_active = false;

void kore_keyboard_show() {
	keyboard_active = true;
	JNIEnv *env;
	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);
	jclass koreActivityClass = kore_android_find_class(env, "tech.kore.KoreActivity");
	(*env)->CallStaticVoidMethod(env, koreActivityClass, (*env)->GetStaticMethodID(env, koreActivityClass, "showKeyboard", "()V"));
	(*activity->vm)->DetachCurrentThread(activity->vm);
}

void kore_keyboard_hide() {
	keyboard_active = false;
	JNIEnv *env;
	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);
	jclass koreActivityClass = kore_android_find_class(env, "tech.kore.KoreActivity");
	(*env)->CallStaticVoidMethod(env, koreActivityClass, (*env)->GetStaticMethodID(env, koreActivityClass, "hideKeyboard", "()V"));
	(*activity->vm)->DetachCurrentThread(activity->vm);
}

bool kore_keyboard_active() {
	return keyboard_active;
}

void kore_load_url(const char *url) {
	JNIEnv *env;
	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);
	jclass koreActivityClass = kore_android_find_class(env, "tech.kore.KoreActivity");
	jstring jurl = (*env)->NewStringUTF(env, url);
	(*env)->CallStaticVoidMethod(env, koreActivityClass, (*env)->GetStaticMethodID(env, koreActivityClass, "loadURL", "(Ljava/lang/String;)V"), jurl);
	(*activity->vm)->DetachCurrentThread(activity->vm);
}

void kore_vibrate(int ms) {
	JNIEnv *env;
	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);
	jclass koreActivityClass = kore_android_find_class(env, "tech.kore.KoreActivity");
	(*env)->CallStaticVoidMethod(env, koreActivityClass, (*env)->GetStaticMethodID(env, koreActivityClass, "vibrate", "(I)V"), ms);
	(*activity->vm)->DetachCurrentThread(activity->vm);
}

const char *kore_language() {
	JNIEnv *env;
	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);
	jclass koreActivityClass = kore_android_find_class(env, "tech.kore.KoreActivity");
	jstring s = (jstring)(*env)->CallStaticObjectMethod(env, koreActivityClass,
	                                                    (*env)->GetStaticMethodID(env, koreActivityClass, "getLanguage", "()Ljava/lang/String;"));
	const char *str = (*env)->GetStringUTFChars(env, s, 0);
	(*activity->vm)->DetachCurrentThread(activity->vm);
	return str;
}

#ifdef KORE_VULKAN
bool kore_vulkan_internal_get_size(int *width, int *height);
#endif

#ifdef KORE_EGL
extern int kore_egl_width(int window);
extern int kore_egl_height(int window);
#endif

int kore_android_width() {
#if defined(KORE_EGL)
	// TODO
	// return kore_egl_width(0);
#elif defined(KORE_VULKAN)
	// TODO
	// int width, height;
	// if (kore_vulkan_internal_get_size(&width, &height)) {
	//	return width;
	//}
	// else
#endif
	{ return ANativeWindow_getWidth(app->window); }
}

int kore_android_height() {
#if defined(KORE_EGL)
	// TODO
	// return kore_egl_height(0);
#elif defined(KORE_VULKAN)
	// TODO
	// int width, height;
	// if (kore_vulkan_internal_get_size(&width, &height)) {
	//	return height;
	//}
	// else
#endif
	{ return ANativeWindow_getHeight(app->window); }
}

const char *kore_internal_save_path() {
	return kore_android_get_activity()->internalDataPath;
}

const char *kore_system_id() {
	return "Android";
}

static const char *videoFormats[] = {"ts", NULL};

const char **kore_video_formats() {
	return videoFormats;
}

void kore_set_keep_screen_on(bool on) {
	if (on) {
		ANativeActivity_setWindowFlags(activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);
	}
	else {
		ANativeActivity_setWindowFlags(activity, 0, AWINDOW_FLAG_KEEP_SCREEN_ON);
	}
}

#include <kore3/input/acceleration.h>
#include <kore3/input/rotation.h>
#include <kore3/window.h>
#include <sys/time.h>
#include <time.h>

static __kernel_time_t start_sec = 0;

double kore_frequency() {
	return 1000000.0;
}

kore_ticks kore_timestamp() {
	struct timeval now;
	gettimeofday(&now, NULL);
	return (kore_ticks)(now.tv_sec - start_sec) * 1000000 + (kore_ticks)(now.tv_usec);
}

double kore_time() {
	struct timeval now;
	gettimeofday(&now, NULL);
	return (double)(now.tv_sec - start_sec) + (now.tv_usec / 1000000.0);
}

void kore_internal_resize(int window, int width, int height);

bool kore_internal_handle_messages(void) {
	kore_mutex_lock(&unicode_mutex);
	for (int i = 0; i < unicode_stack_index; ++i) {
		kore_internal_keyboard_trigger_key_press(unicode_stack[i]);
	}
	unicode_stack_index = 0;
	kore_mutex_unlock(&unicode_mutex);

	int ident;
	int events;
	struct android_poll_source *source;

	while ((ident = ALooper_pollAll(paused ? -1 : 0, NULL, &events, (void **)&source)) >= 0) {
		if (source != NULL) {
			source->process(app, source);
		}

		if (ident == LOOPER_ID_USER) {
			if (accelerometerSensor != NULL) {
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
					if (event.type == ASENSOR_TYPE_ACCELEROMETER) {
						kore_internal_on_acceleration(event.acceleration.x, event.acceleration.y, event.acceleration.z);
					}
					else if (event.type == ASENSOR_TYPE_GYROSCOPE) {
						kore_internal_on_rotation(event.vector.x, event.vector.y, event.vector.z);
					}
				}
			}
		}

		if (app->destroyRequested != 0) {
			termDisplay();
			kore_stop();
			return true;
		}
	}

	if (activityJustResized && app->window != NULL) {
		activityJustResized = false;
		int32_t width = kore_android_width();
		int32_t height = kore_android_height();
#ifdef KORE_VULKAN
		// kore_internal_resize(0, width, height); // TODO
#endif
		kore_internal_call_resize_callback(0, width, height);
	}

	// Get screen rotation
	/*
	JNIEnv* env;
	(*activity->vm)->AttachCurrentThread(&env, NULL);
	jclass koreActivityClass = KoreAndroid::findClass(env, "tech.kode.kore.KoreActivity");
	jmethodID koreActivityGetRotation = (*env)->GetStaticMethodID(koreActivityClass, "getRotation", "()I");
	screenRotation = (*env)->CallStaticIntMethod(koreActivityClass, koreActivityGetRotation);
	(*activity->vm)->DetachCurrentThread();
	*/

	return true;
}

bool kore_mouse_can_lock(void) {
	return false;
}

void kore_mouse_show() {}

void kore_mouse_hide() {}

void kore_mouse_set_position(int window, int x, int y) {}

void kore_internal_mouse_lock(int window) {}

void kore_internal_mouse_unlock(void) {}

void kore_mouse_get_position(int window, int *x, int *y) {
	x = 0;
	y = 0;
}

void kore_mouse_set_cursor(int cursor_index) {}

void kore_login() {}

void kore_unlock_achievement(int id) {}

bool kore_gamepad_connected(int num) {
	return num == 0;
}

void kore_gamepad_rumble(int gamepad, float left, float right) {}

void initAndroidFileReader();
void KoreAndroidVideoInit();

void android_main(struct android_app *application) {
	app_dummy();

	struct timeval now;
	gettimeofday(&now, NULL);
	start_sec = now.tv_sec;

	app = application;
	activity = application->activity;
	initAndroidFileReader();
	KoreAndroidVideoInit();
	KoreAndroidKeyboardInit();
	application->onAppCmd = cmd;
	application->onInputEvent = input;
	activity->callbacks->onNativeWindowResized = resize;
	// #ifndef KORE_VULKAN
	// 	glContext = ndk_helper::GLContext::GetInstance();
	// #endif
	sensorManager = ASensorManager_getInstance();
	accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	gyroSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
	sensorEventQueue = ASensorManager_createEventQueue(sensorManager, application->looper, LOOPER_ID_USER, NULL, NULL);

	JNIEnv *env = NULL;
	(*kore_android_get_activity()->vm)->AttachCurrentThread(kore_android_get_activity()->vm, &env, NULL);

	jclass koreMoviePlayerClass = kore_android_find_class(env, "tech.kore.KoreMoviePlayer");
	jmethodID updateAll = (*env)->GetStaticMethodID(env, koreMoviePlayerClass, "updateAll", "()V");

	while (!started) {
		kore_internal_handle_messages();
		(*env)->CallStaticVoidMethod(env, koreMoviePlayerClass, updateAll);
	}
	(*kore_android_get_activity()->vm)->DetachCurrentThread(kore_android_get_activity()->vm);
	kickstart(0, NULL);

	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);
	jclass koreActivityClass = kore_android_find_class(env, "tech.kore.KoreActivity");
	jmethodID FinishHim = (*env)->GetStaticMethodID(env, koreActivityClass, "stop", "()V");
	(*env)->CallStaticVoidMethod(env, koreActivityClass, FinishHim);
	(*activity->vm)->DetachCurrentThread(activity->vm);
}

int kore_init(const char *name, int width, int height, struct kore_window_parameters *win, struct kore_framebuffer_parameters *frame) {
	kore_mutex_init(&unicode_mutex);

	kore_window_parameters default_win;
	if (win == NULL) {
		kore_window_options_set_defaults(&default_win);
		win = &default_win;
	}
	win->width = width;
	win->height = height;

	kore_framebuffer_parameters default_frame;
	if (frame == NULL) {
		kore_framebuffer_options_set_defaults(&default_frame);
		frame = &default_frame;
	}

	// kore_g4_internal_init();
	// kore_g4_internal_init_window(0, frame->depth_bits, frame->stencil_bits, true);

	kore_internal_gamepad_trigger_connect(0);

	return 0;
}

void kore_internal_shutdown(void) {
	kore_internal_gamepad_trigger_disconnect(0);
}

const char *kore_gamepad_vendor(int gamepad) {
	return "Google";
}

const char *kore_gamepad_product_name(int gamepad) {
	return "gamepad";
}

#include <kore3/io/filereader.h>

#define CLASS_NAME "android/app/NativeActivity"

void initAndroidFileReader(void) {
	if (activity == NULL) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Android activity is NULL");
		return;
	}

	JNIEnv *env;

	(*activity->vm)->AttachCurrentThread(activity->vm, &env, NULL);

	jclass android_app_NativeActivity = (*env)->FindClass(env, CLASS_NAME);
	jmethodID getExternalFilesDir = (*env)->GetMethodID(env, android_app_NativeActivity, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
	jobject file = (*env)->CallObjectMethod(env, activity->clazz, getExternalFilesDir, NULL);
	jclass java_io_File = (*env)->FindClass(env, "java/io/File");
	jmethodID getPath = (*env)->GetMethodID(env, java_io_File, "getPath", "()Ljava/lang/String;");
	jstring jPath = (*env)->CallObjectMethod(env, file, getPath);

	const char *path = (*env)->GetStringUTFChars(env, jPath, NULL);
	char *externalFilesDir = malloc(strlen(path) + 1);
	strcpy(externalFilesDir, path);
	kore_internal_set_files_location(externalFilesDir);

	(*env)->ReleaseStringUTFChars(env, jPath, path);
	(*env)->DeleteLocalRef(env, jPath);
	(*activity->vm)->DetachCurrentThread(activity->vm);
}

static bool kore_aasset_reader_close(kore_file_reader *reader) {
	AAsset_close((struct AAsset *)reader->data);
	return true;
}

static size_t kore_aasset_reader_read(kore_file_reader *reader, void *data, size_t size) {
	return AAsset_read((struct AAsset *)reader->data, data, size);
}

static size_t kore_aasset_reader_pos(kore_file_reader *reader) {
	return (size_t)AAsset_seek((struct AAsset *)reader->data, 0, SEEK_CUR);
}

static bool kore_aasset_reader_seek(kore_file_reader *reader, size_t pos) {
	AAsset_seek((struct AAsset *)reader->data, pos, SEEK_SET);
	return true;
}

static bool kore_aasset_reader_open(kore_file_reader *reader, const char *filename, int type) {
	if (type != KORE_FILE_TYPE_ASSET)
		return false;
	reader->data = AAssetManager_open(kore_android_get_asset_manager(), filename, AASSET_MODE_RANDOM);
	if (reader->data == NULL)
		return false;
	reader->size = AAsset_getLength((struct AAsset *)reader->data);
	reader->close = kore_aasset_reader_close;
	reader->read = kore_aasset_reader_read;
	reader->pos = kore_aasset_reader_pos;
	reader->seek = kore_aasset_reader_seek;
	return true;
}

bool kore_file_reader_open(kore_file_reader *reader, const char *filename, int type) {
	memset(reader, 0, sizeof(*reader));
	return kore_internal_file_reader_callback(reader, filename, type) || kore_internal_file_reader_open(reader, filename, type) ||
	       kore_aasset_reader_open(reader, filename, type);
}

int kore_cpu_cores(void) {
	return kore_hardware_threads();
}

int kore_hardware_threads(void) {
	return sysconf(_SC_NPROCESSORS_ONLN);
}
