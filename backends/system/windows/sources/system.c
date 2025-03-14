#include <kore3/gpu/gpu.h>

#include <kore3/input/gamepad.h>

#include <kore3/backend/microsoft.h>
#include <kore3/backend/windows.h>

#include <kore3/display.h>
#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/input/pen.h>
#include <kore3/input/surface.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/threads/thread.h>
#include <kore3/video.h>
#include <kore3/window.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <oleauto.h>
#include <stdio.h>
#include <wbemidl.h>

#include <XInput.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>

#if defined(KORE_VTUNE)
#include <ittnotify.h>

__itt_domain *kore_itt_domain;
#endif

#ifdef KORE_G4ONG5
#define Graphics Graphics5
#elif KORE_G4
#define Graphics Graphics4
#else
#define Graphics Graphics3
#endif

__declspec(dllexport) unsigned long NvOptimusEnablement        = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
void kore_internal_resize(int window, int width, int height);

typedef BOOL(WINAPI *GetPointerInfoType)(UINT32 pointerId, POINTER_INFO *pointerInfo);
static GetPointerInfoType MyGetPointerInfo = NULL;
typedef BOOL(WINAPI *GetPointerPenInfoType)(UINT32 pointerId, POINTER_PEN_INFO *penInfo);
static GetPointerPenInfoType MyGetPointerPenInfo = NULL;
typedef BOOL(WINAPI *EnableNonClientDpiScalingType)(HWND hwnd);
static EnableNonClientDpiScalingType MyEnableNonClientDpiScaling = NULL;

#define MAX_TOUCH_POINTS 10

#define KORE_DINPUT_MAX_COUNT 8

struct touchpoint {
	int sysID;
	int x;
	int y;
};

static struct touchpoint touchPoints[MAX_TOUCH_POINTS];
static int mouseX, mouseY;
static bool keyPressed[256];
static int keyTranslated[256]; // http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx

static int GetTouchIndex(int dwID) {
	for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
		if (touchPoints[i].sysID == dwID) {
			return i;
		}
	}
	return -1;
}

static int GetAddTouchIndex(int dwID) {
	for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
		if (touchPoints[i].sysID == dwID) {
			return i;
		}
	}
	for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
		if (touchPoints[i].sysID == -1) {
			touchPoints[i].sysID = dwID;
			return i;
		}
	}
	return -1;
}

static void ReleaseTouchIndex(int dwID) {
	for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
		if (touchPoints[i].sysID == dwID) {
			touchPoints[i].sysID = -1;
			touchPoints[i].x     = -1;
			touchPoints[i].y     = -1;
		}
	}
}

static void initKeyTranslation() {
	for (int i = 0; i < 256; ++i)
		keyTranslated[i] = KORE_KEY_UNKNOWN;

	keyTranslated[VK_BACK]    = KORE_KEY_BACKSPACE;
	keyTranslated[VK_TAB]     = KORE_KEY_TAB;
	keyTranslated[VK_CLEAR]   = KORE_KEY_CLEAR;
	keyTranslated[VK_RETURN]  = KORE_KEY_RETURN;
	keyTranslated[VK_SHIFT]   = KORE_KEY_SHIFT;
	keyTranslated[VK_CONTROL] = KORE_KEY_CONTROL;
	keyTranslated[VK_MENU]    = KORE_KEY_ALT;
	keyTranslated[VK_PAUSE]   = KORE_KEY_PAUSE;
	keyTranslated[VK_CAPITAL] = KORE_KEY_CAPS_LOCK;
	keyTranslated[VK_KANA]    = KORE_KEY_KANA;
	// keyTranslated[VK_HANGUEL]
	keyTranslated[VK_HANGUL] = KORE_KEY_HANGUL;
	keyTranslated[VK_JUNJA]  = KORE_KEY_JUNJA;
	keyTranslated[VK_FINAL]  = KORE_KEY_FINAL;
	keyTranslated[VK_HANJA]  = KORE_KEY_HANJA;
	keyTranslated[VK_KANJI]  = KORE_KEY_KANJI;
	keyTranslated[VK_ESCAPE] = KORE_KEY_ESCAPE;
	// keyTranslated[VK_CONVERT]
	// keyTranslated[VK_NONCONVERT
	// keyTranslated[VK_ACCEPT
	// keyTranslated[VK_MODECHANGE
	keyTranslated[VK_SPACE] = KORE_KEY_SPACE;
	keyTranslated[VK_PRIOR] = KORE_KEY_PAGE_UP;
	keyTranslated[VK_NEXT]  = KORE_KEY_PAGE_DOWN;
	keyTranslated[VK_END]   = KORE_KEY_END;
	keyTranslated[VK_HOME]  = KORE_KEY_HOME;
	keyTranslated[VK_LEFT]  = KORE_KEY_LEFT;
	keyTranslated[VK_UP]    = KORE_KEY_UP;
	keyTranslated[VK_RIGHT] = KORE_KEY_RIGHT;
	keyTranslated[VK_DOWN]  = KORE_KEY_DOWN;
	// keyTranslated[VK_SELECT
	keyTranslated[VK_PRINT] = KORE_KEY_PRINT;
	// keyTranslated[VK_EXECUTE
	// keyTranslated[VK_SNAPSHOT
	keyTranslated[VK_INSERT] = KORE_KEY_INSERT;
	keyTranslated[VK_DELETE] = KORE_KEY_DELETE;
	keyTranslated[VK_HELP]   = KORE_KEY_HELP;
	keyTranslated[0x30]      = KORE_KEY_0;
	keyTranslated[0x31]      = KORE_KEY_1;
	keyTranslated[0x32]      = KORE_KEY_2;
	keyTranslated[0x33]      = KORE_KEY_3;
	keyTranslated[0x34]      = KORE_KEY_4;
	keyTranslated[0x35]      = KORE_KEY_5;
	keyTranslated[0x36]      = KORE_KEY_6;
	keyTranslated[0x37]      = KORE_KEY_7;
	keyTranslated[0x38]      = KORE_KEY_8;
	keyTranslated[0x39]      = KORE_KEY_9;
	keyTranslated[0x41]      = KORE_KEY_A;
	keyTranslated[0x42]      = KORE_KEY_B;
	keyTranslated[0x43]      = KORE_KEY_C;
	keyTranslated[0x44]      = KORE_KEY_D;
	keyTranslated[0x45]      = KORE_KEY_E;
	keyTranslated[0x46]      = KORE_KEY_F;
	keyTranslated[0x47]      = KORE_KEY_G;
	keyTranslated[0x48]      = KORE_KEY_H;
	keyTranslated[0x49]      = KORE_KEY_I;
	keyTranslated[0x4A]      = KORE_KEY_J;
	keyTranslated[0x4B]      = KORE_KEY_K;
	keyTranslated[0x4C]      = KORE_KEY_L;
	keyTranslated[0x4D]      = KORE_KEY_M;
	keyTranslated[0x4E]      = KORE_KEY_N;
	keyTranslated[0x4F]      = KORE_KEY_O;
	keyTranslated[0x50]      = KORE_KEY_P;
	keyTranslated[0x51]      = KORE_KEY_Q;
	keyTranslated[0x52]      = KORE_KEY_R;
	keyTranslated[0x53]      = KORE_KEY_S;
	keyTranslated[0x54]      = KORE_KEY_T;
	keyTranslated[0x55]      = KORE_KEY_U;
	keyTranslated[0x56]      = KORE_KEY_V;
	keyTranslated[0x57]      = KORE_KEY_W;
	keyTranslated[0x58]      = KORE_KEY_X;
	keyTranslated[0x59]      = KORE_KEY_Y;
	keyTranslated[0x5A]      = KORE_KEY_Z;
	keyTranslated[VK_LWIN]   = KORE_KEY_WIN;
	keyTranslated[VK_RWIN]   = KORE_KEY_WIN;
	keyTranslated[VK_APPS]   = KORE_KEY_CONTEXT_MENU;
	// keyTranslated[VK_SLEEP
	keyTranslated[VK_NUMPAD0]  = KORE_KEY_NUMPAD_0;
	keyTranslated[VK_NUMPAD1]  = KORE_KEY_NUMPAD_1;
	keyTranslated[VK_NUMPAD2]  = KORE_KEY_NUMPAD_2;
	keyTranslated[VK_NUMPAD3]  = KORE_KEY_NUMPAD_3;
	keyTranslated[VK_NUMPAD4]  = KORE_KEY_NUMPAD_4;
	keyTranslated[VK_NUMPAD5]  = KORE_KEY_NUMPAD_5;
	keyTranslated[VK_NUMPAD6]  = KORE_KEY_NUMPAD_6;
	keyTranslated[VK_NUMPAD7]  = KORE_KEY_NUMPAD_7;
	keyTranslated[VK_NUMPAD8]  = KORE_KEY_NUMPAD_8;
	keyTranslated[VK_NUMPAD9]  = KORE_KEY_NUMPAD_9;
	keyTranslated[VK_MULTIPLY] = KORE_KEY_MULTIPLY;
	keyTranslated[VK_ADD]      = KORE_KEY_ADD;
	// keyTranslated[VK_SEPARATOR
	keyTranslated[VK_SUBTRACT] = KORE_KEY_SUBTRACT;
	keyTranslated[VK_DECIMAL]  = KORE_KEY_DECIMAL;
	keyTranslated[VK_DIVIDE]   = KORE_KEY_DIVIDE;
	keyTranslated[VK_F1]       = KORE_KEY_F1;
	keyTranslated[VK_F2]       = KORE_KEY_F2;
	keyTranslated[VK_F3]       = KORE_KEY_F3;
	keyTranslated[VK_F4]       = KORE_KEY_F4;
	keyTranslated[VK_F5]       = KORE_KEY_F5;
	keyTranslated[VK_F6]       = KORE_KEY_F6;
	keyTranslated[VK_F7]       = KORE_KEY_F7;
	keyTranslated[VK_F8]       = KORE_KEY_F8;
	keyTranslated[VK_F9]       = KORE_KEY_F9;
	keyTranslated[VK_F10]      = KORE_KEY_F10;
	keyTranslated[VK_F11]      = KORE_KEY_F11;
	keyTranslated[VK_F12]      = KORE_KEY_F12;
	keyTranslated[VK_F13]      = KORE_KEY_F13;
	keyTranslated[VK_F14]      = KORE_KEY_F14;
	keyTranslated[VK_F15]      = KORE_KEY_F15;
	keyTranslated[VK_F16]      = KORE_KEY_F16;
	keyTranslated[VK_F17]      = KORE_KEY_F17;
	keyTranslated[VK_F18]      = KORE_KEY_F18;
	keyTranslated[VK_F19]      = KORE_KEY_F19;
	keyTranslated[VK_F20]      = KORE_KEY_F20;
	keyTranslated[VK_F21]      = KORE_KEY_F21;
	keyTranslated[VK_F22]      = KORE_KEY_F22;
	keyTranslated[VK_F23]      = KORE_KEY_F23;
	keyTranslated[VK_F24]      = KORE_KEY_F24;
	keyTranslated[VK_NUMLOCK]  = KORE_KEY_NUM_LOCK;
	keyTranslated[VK_SCROLL]   = KORE_KEY_SCROLL_LOCK;
	// 0x92-96 //OEM specific
	keyTranslated[VK_LSHIFT]   = KORE_KEY_SHIFT;
	keyTranslated[VK_RSHIFT]   = KORE_KEY_SHIFT;
	keyTranslated[VK_LCONTROL] = KORE_KEY_CONTROL;
	keyTranslated[VK_RCONTROL] = KORE_KEY_CONTROL;
	// keyTranslated[VK_LMENU
	// keyTranslated[VK_RMENU
	// keyTranslated[VK_BROWSER_BACK
	// keyTranslated[VK_BROWSER_FORWARD
	// keyTranslated[VK_BROWSER_REFRESH
	// keyTranslated[VK_BROWSER_STOP
	// keyTranslated[VK_BROWSER_SEARCH
	// keyTranslated[VK_BROWSER_FAVORITES
	// keyTranslated[VK_BROWSER_HOME
	// keyTranslated[VK_VOLUME_MUTE
	// keyTranslated[VK_VOLUME_DOWN
	// keyTranslated[VK_VOLUME_UP
	// keyTranslated[VK_MEDIA_NEXT_TRACK
	// keyTranslated[VK_MEDIA_PREV_TRACK
	// keyTranslated[VK_MEDIA_STOP
	// keyTranslated[VK_MEDIA_PLAY_PAUSE
	// keyTranslated[VK_LAUNCH_MAIL
	// keyTranslated[VK_LAUNCH_MEDIA_SELECT
	// keyTranslated[VK_LAUNCH_APP1
	// keyTranslated[VK_LAUNCH_APP2
	keyTranslated[VK_OEM_1]      = KORE_KEY_SEMICOLON; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
	keyTranslated[VK_OEM_PLUS]   = KORE_KEY_PLUS;
	keyTranslated[VK_OEM_COMMA]  = KORE_KEY_COMMA;
	keyTranslated[VK_OEM_MINUS]  = KORE_KEY_HYPHEN_MINUS;
	keyTranslated[VK_OEM_PERIOD] = KORE_KEY_PERIOD;
	keyTranslated[VK_OEM_2]      = KORE_KEY_SLASH;    // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
	keyTranslated[VK_OEM_3] = KORE_KEY_BACK_QUOTE;    // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key
	keyTranslated[VK_OEM_4] = KORE_KEY_OPEN_BRACKET;  // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
	keyTranslated[VK_OEM_5] = KORE_KEY_BACK_SLASH;    // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
	keyTranslated[VK_OEM_6] = KORE_KEY_CLOSE_BRACKET; // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
	keyTranslated[VK_OEM_7] = KORE_KEY_QUOTE;         // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the
	                                          // 'single-quote/double-quote' key keyTranslated[VK_OEM_8 //Used for miscellaneous characters; it can vary by
	                                          // keyboard. keyTranslated[0xE1 //OEM specific keyTranslated[VK_OEM_102 //Either the angle bracket key or the
	                                          // backslash key on the RT 102-key keyboard 0xE3-E4 //OEM specific keyTranslated[VK_PROCESSKEY 0xE6 //OEM specific
	// keyTranslated[VK_PACKET //Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value
	// used for non-keyboard input methods.
	// 0xE9-F5 //OEM specific
	// keyTranslated[VK_ATTN
	// keyTranslated[VK_CRSEL
	// keyTranslated[VK_EXSEL
	// keyTranslated[VK_EREOF
	// keyTranslated[VK_PLAY
	// keyTranslated[VK_ZOOM
	// keyTranslated[VK_NONAME
	// keyTranslated[VK_PA1
	// keyTranslated[PA1 key
	// keyTranslated[VK_OEM_CLEAR
}

static bool detectGamepad = true;
static bool gamepadFound  = false;
static unsigned r         = 0;

static wchar_t toUnicode(WPARAM wParam, LPARAM lParam) {
	wchar_t buffer[11];
	BYTE state[256];
	GetKeyboardState(state);
	ToUnicode((UINT)wParam, (lParam >> 8) & 0xFFFFFF00, state, buffer, 10, 0);
	return buffer[0];
}

#if !defined(KORE_DIRECT3D9) && !defined(KORE_DIRECT3D11) && !defined(KORE_DIRECT3D12)
#define HANDLE_ALT_ENTER
#endif

static bool cursors_initialized = false;
static int cursor               = 0;
#define NUM_CURSORS 14
static HCURSOR cursors[NUM_CURSORS];

void kore_mouse_set_cursor(int set_cursor) {
	cursor = set_cursor >= NUM_CURSORS ? 0 : set_cursor;
	if (cursors_initialized) {
		SetCursor(cursors[cursor]);
	}
}

LRESULT WINAPI KoreWindowsMessageProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int windowId;
	DWORD pointerId;
	POINTER_INFO pointerInfo = {0};
	POINTER_PEN_INFO penInfo = {0};
	static bool controlDown  = false;
#ifdef HANDLE_ALT_ENTER
	static bool altDown = false;
#endif
	static int last_window_width  = -1;
	static int last_window_height = -1;
	static int last_window_x      = INT_MIN;
	static int last_window_y      = INT_MIN;

	switch (msg) {
	case WM_NCCREATE:
		if (MyEnableNonClientDpiScaling != NULL) {
			MyEnableNonClientDpiScaling(hWnd);
		}
		break;
	case WM_DPICHANGED: {
		int window = kore_windows_window_index_from_hwnd(hWnd);
		if (window >= 0) {
			kore_internal_call_ppi_changed_callback(window, LOWORD(wParam));
		}
		break;
	}
	case WM_MOVE:
	case WM_MOVING:
	case WM_SIZING:
		// Scheduler::breakTime();
		break;
	case WM_SIZE: {
		int window = kore_windows_window_index_from_hwnd(hWnd);
		if (window >= 0) {
			int width  = LOWORD(lParam);
			int height = HIWORD(lParam);
			// kore_internal_resize(window, width, height); // TODO
			kore_internal_call_resize_callback(window, width, height);
		}
		break;
	}
	case WM_CLOSE: {
		int window_index = kore_windows_window_index_from_hwnd(hWnd);
		if (kore_internal_call_close_callback(window_index)) {
			kore_window_destroy(window_index);
			if (kore_count_windows() <= 0) {
				kore_stop();
				return 0;
			}
		}
		return 0;
	}
	case WM_ERASEBKGND:
		return 1;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE) {
			kore_internal_mouse_window_activated(kore_windows_window_index_from_hwnd(hWnd));
			kore_internal_foreground_callback();
		}
		else {
			kore_internal_mouse_window_deactivated(kore_windows_window_index_from_hwnd(hWnd));
			kore_internal_background_callback();
#ifdef HANDLE_ALT_ENTER
			altDown = false;
#endif
		}
		RegisterTouchWindow(hWnd, 0);
		break;
	case WM_MOUSELEAVE:
		windowId = kore_windows_window_index_from_hwnd(hWnd);
		//**windows[windowId]->isMouseInside = false;
		kore_internal_mouse_trigger_leave_window(windowId);
		break;
	case WM_MOUSEMOVE:
		windowId = kore_windows_window_index_from_hwnd(hWnd);
		/*if (!windows[windowId]->isMouseInside) {
		    windows[windowId]->isMouseInside = true;
		    TRACKMOUSEEVENT tme;
		    tme.cbSize = sizeof(TRACKMOUSEEVENT);
		    tme.dwFlags = TME_LEAVE;
		    tme.hwndTrack = hWnd;
		    TrackMouseEvent(&tme);
		}*/
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_move(windowId, mouseX, mouseY);
		break;
	case WM_CREATE:
		cursors[0]          = LoadCursor(0, IDC_ARROW);
		cursors[1]          = LoadCursor(0, IDC_HAND);
		cursors[2]          = LoadCursor(0, IDC_IBEAM);
		cursors[3]          = LoadCursor(0, IDC_SIZEWE);
		cursors[4]          = LoadCursor(0, IDC_SIZENS);
		cursors[5]          = LoadCursor(0, IDC_SIZENESW);
		cursors[6]          = LoadCursor(0, IDC_SIZENWSE);
		cursors[7]          = LoadCursor(0, IDC_SIZENWSE);
		cursors[8]          = LoadCursor(0, IDC_SIZENESW);
		cursors[9]          = LoadCursor(0, IDC_SIZEALL);
		cursors[10]         = LoadCursor(0, IDC_SIZEALL);
		cursors[11]         = LoadCursor(0, IDC_NO);
		cursors[12]         = LoadCursor(0, IDC_WAIT);
		cursors[13]         = LoadCursor(0, IDC_CROSS);
		cursors_initialized = true;
		if (cursor != 0) {
			SetCursor(cursors[cursor]);
		}
		return TRUE;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT) {
			SetCursor(cursors[cursor]);
			return TRUE;
		}
		break;
	case WM_LBUTTONDOWN:
		if (!kore_mouse_is_locked())
			SetCapture(hWnd);
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_press(kore_windows_window_index_from_hwnd(hWnd), 0, mouseX, mouseY);
		break;
	case WM_LBUTTONUP:
		if (!kore_mouse_is_locked())
			ReleaseCapture();
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_release(kore_windows_window_index_from_hwnd(hWnd), 0, mouseX, mouseY);
		break;
	case WM_RBUTTONDOWN:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_press(kore_windows_window_index_from_hwnd(hWnd), 1, mouseX, mouseY);
		break;
	case WM_RBUTTONUP:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_release(kore_windows_window_index_from_hwnd(hWnd), 1, mouseX, mouseY);
		break;
	case WM_MBUTTONDOWN:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_press(kore_windows_window_index_from_hwnd(hWnd), 2, mouseX, mouseY);
		break;
	case WM_MBUTTONUP:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_release(kore_windows_window_index_from_hwnd(hWnd), 2, mouseX, mouseY);
		break;
	case WM_XBUTTONDOWN:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_press(kore_windows_window_index_from_hwnd(hWnd), HIWORD(wParam) + 2, mouseX, mouseY);
		break;
	case WM_XBUTTONUP:
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		kore_internal_mouse_trigger_release(kore_windows_window_index_from_hwnd(hWnd), HIWORD(wParam) + 2, mouseX, mouseY);
		break;
	case WM_MOUSEWHEEL:
		kore_internal_mouse_trigger_scroll(kore_windows_window_index_from_hwnd(hWnd), GET_WHEEL_DELTA_WPARAM(wParam) / -120);
		break;
	case WM_POINTERDOWN:
		pointerId = GET_POINTERID_WPARAM(wParam);
		MyGetPointerInfo(pointerId, &pointerInfo);
		if (pointerInfo.pointerType == PT_PEN) {
			MyGetPointerPenInfo(pointerId, &penInfo);
			ScreenToClient(hWnd, &pointerInfo.ptPixelLocation);
			kore_internal_pen_trigger_press(kore_windows_window_index_from_hwnd(hWnd), pointerInfo.ptPixelLocation.x, pointerInfo.ptPixelLocation.y,
			                                penInfo.pressure / 1024.0f);
		}
		break;
	case WM_POINTERUP:
		pointerId = GET_POINTERID_WPARAM(wParam);
		MyGetPointerInfo(pointerId, &pointerInfo);
		if (pointerInfo.pointerType == PT_PEN) {
			MyGetPointerPenInfo(pointerId, &penInfo);
			ScreenToClient(hWnd, &pointerInfo.ptPixelLocation);
			kore_internal_pen_trigger_release(kore_windows_window_index_from_hwnd(hWnd), pointerInfo.ptPixelLocation.x, pointerInfo.ptPixelLocation.y,
			                                  penInfo.pressure / 1024.0f);
		}
		break;
	case WM_POINTERUPDATE:
		pointerId = GET_POINTERID_WPARAM(wParam);
		MyGetPointerInfo(pointerId, &pointerInfo);
		if (pointerInfo.pointerType == PT_PEN) {
			MyGetPointerPenInfo(pointerId, &penInfo);
			ScreenToClient(hWnd, &pointerInfo.ptPixelLocation);
			kore_internal_pen_trigger_move(kore_windows_window_index_from_hwnd(hWnd), pointerInfo.ptPixelLocation.x, pointerInfo.ptPixelLocation.y,
			                               penInfo.pressure / 1024.0f);
		}
		break;
	case WM_TOUCH: {
		BOOL bHandled       = FALSE;
		UINT cInputs        = LOWORD(wParam);
		PTOUCHINPUT pInputs = _alloca(cInputs * sizeof(TOUCHINPUT));
		POINT ptInput;
		int tindex;
		if (pInputs) {
			if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
				for (int i = 0; i < (int)cInputs; i++) {
					TOUCHINPUT ti = pInputs[i];
					if (ti.dwID != 0) {
						ptInput.x = TOUCH_COORD_TO_PIXEL(ti.x);
						ptInput.y = TOUCH_COORD_TO_PIXEL(ti.y);
						ScreenToClient(hWnd, &ptInput);

						if (ti.dwFlags & TOUCHEVENTF_UP) {
							tindex = GetTouchIndex(ti.dwID);
							ReleaseTouchIndex(ti.dwID);
							kore_internal_surface_trigger_touch_end(tindex, ptInput.x, ptInput.y);
						}
						else {
							bool touchExisits = GetTouchIndex(ti.dwID) != -1;
							tindex            = GetAddTouchIndex(ti.dwID);
							if (tindex >= 0) {
								if (touchExisits) {
									if (touchPoints[tindex].x != ptInput.x || touchPoints[tindex].y != ptInput.y) {
										touchPoints[tindex].x = ptInput.x;
										touchPoints[tindex].y = ptInput.y;
										kore_internal_surface_trigger_move(tindex, ptInput.x, ptInput.y);
									}
								}
								else {
									touchPoints[tindex].x = ptInput.x;
									touchPoints[tindex].y = ptInput.y;
									kore_internal_surface_trigger_touch_start(tindex, ptInput.x, ptInput.y);
								}
							}
						}
					}
					bHandled = TRUE;

					if (!CloseTouchInputHandle((HTOUCHINPUT)lParam)) {
					}
				}
			}
		}
		if (bHandled)
			CloseTouchInputHandle((HTOUCHINPUT)lParam);
		else
			DefWindowProcW(hWnd, WM_TOUCH, wParam, lParam);

		InvalidateRect(hWnd, NULL, FALSE);
	} break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!keyPressed[wParam]) {
			keyPressed[wParam] = true;

			if (keyTranslated[wParam] == KORE_KEY_CONTROL) {
				controlDown = true;
			}
#ifdef HANDLE_ALT_ENTER
			else if (keyTranslated[wParam] == KORE_KEY_ALT) {
				altDown = true;
			}
#endif
			else {
				if (controlDown && keyTranslated[wParam] == KORE_KEY_X) {
					char *text = kore_internal_cut_callback();
					if (text != NULL) {
						wchar_t wtext[4096];
						MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, 4096);
						OpenClipboard(hWnd);
						EmptyClipboard();
						size_t size   = (wcslen(wtext) + 1) * sizeof(wchar_t);
						HANDLE handle = GlobalAlloc(GMEM_MOVEABLE, size);
						void *data    = GlobalLock(handle);
						memcpy(data, wtext, size);
						GlobalUnlock(handle);
						SetClipboardData(CF_UNICODETEXT, handle);
						CloseClipboard();
					}
				}

				if (controlDown && keyTranslated[wParam] == KORE_KEY_C) {
					char *text = kore_internal_copy_callback();
					if (text != NULL) {
						wchar_t wtext[4096];
						MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, 4096);
						OpenClipboard(hWnd);
						EmptyClipboard();
						size_t size   = (wcslen(wtext) + 1) * sizeof(wchar_t);
						HANDLE handle = GlobalAlloc(GMEM_MOVEABLE, size);
						void *data    = GlobalLock(handle);
						memcpy(data, wtext, size);
						GlobalUnlock(handle);
						SetClipboardData(CF_UNICODETEXT, handle);
						CloseClipboard();
					}
				}

				if (controlDown && keyTranslated[wParam] == KORE_KEY_V) {
					if (IsClipboardFormatAvailable(CF_UNICODETEXT)) {
						OpenClipboard(hWnd);
						HANDLE handle = GetClipboardData(CF_UNICODETEXT);
						if (handle != NULL) {
							wchar_t *wtext = (wchar_t *)GlobalLock(handle);
							if (wtext != NULL) {
								char text[4096];
								WideCharToMultiByte(CP_UTF8, 0, wtext, -1, text, 4096, NULL, NULL);
								kore_internal_paste_callback(text);
								GlobalUnlock(handle);
							}
						}
						CloseClipboard();
					}
				}

#ifdef HANDLE_ALT_ENTER
				if (altDown && keyTranslated[wParam] == KORE_KEY_RETURN) {
					if (kore_window_get_mode(0) == KORE_WINDOW_MODE_WINDOW) {
						last_window_width  = kore_window_width(0);
						last_window_height = kore_window_height(0);
						last_window_x      = kore_window_x(0);
						last_window_y      = kore_window_y(0);
						kore_window_change_mode(0, KORE_WINDOW_MODE_FULLSCREEN);
					}
					else {
						kore_window_change_mode(0, KORE_WINDOW_MODE_WINDOW);
						if (last_window_width > 0 && last_window_height > 0) {
							kore_window_resize(0, last_window_width, last_window_height);
						}
						if (last_window_x > INT_MIN && last_window_y > INT_MIN) {
							kore_window_move(0, last_window_x, last_window_y);
						}
					}
				}
#endif
			}
		}
		kore_internal_keyboard_trigger_key_down(keyTranslated[wParam]);
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyPressed[wParam] = false;

		if (keyTranslated[wParam] == KORE_KEY_CONTROL) {
			controlDown = false;
		}
#ifdef HANDLE_ALT_ENTER
		if (keyTranslated[wParam] == KORE_KEY_ALT) {
			altDown = false;
		}
#endif

		kore_internal_keyboard_trigger_key_up(keyTranslated[wParam]);
		break;
	case WM_CHAR:
		switch (wParam) {
		case 0x1B: // escape
			break;
		default:
			kore_internal_keyboard_trigger_key_press((unsigned)wParam);
			break;
		}
		break;
	case WM_SYSCOMMAND:
		switch (wParam) {
		case SC_KEYMENU:
			return 0; // Prevent from happening
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0; // Prevent from happening

		// Pause game when window is minimized, continue when it's restored or maximized.
		//
		// Unfortunately, if the game would continue to run when minimized, the graphics in
		// the Windows Vista/7 taskbar would not be updated, even when Direct3DDevice::Present()
		// is called without error. I do not know why.
		case SC_MINIMIZE:
			// Scheduler::haltTime(); // haltTime()/unhaltTime() is incremental, meaning that this doesn't interfere with when the game itself calls these
			// functions
			break;
		case SC_RESTORE:
		case SC_MAXIMIZE:
			// Scheduler::unhaltTime();
			break;
		}
		break;
	case WM_DEVICECHANGE:
		detectGamepad = true;
		break;
	case WM_DROPFILES: {
		HDROP hDrop    = (HDROP)wParam;
		unsigned count = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
		for (unsigned i = 0; i < count; ++i) {
			wchar_t filePath[260];
			if (DragQueryFileW(hDrop, i, filePath, 260)) {
				kore_internal_drop_files_callback(filePath);
			}
		}
		DragFinish(hDrop);
		break;
	}
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

static float axes[12 * 6];
static float buttons[12 * 16];

typedef DWORD(WINAPI *XInputGetStateType)(DWORD dwUserIndex, XINPUT_STATE *pState);
typedef DWORD(WINAPI *XInputSetStateType)(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration);
static XInputGetStateType InputGetState = NULL;
static XInputSetStateType InputSetState = NULL;

void loadXInput() {
	HMODULE lib = LoadLibraryA("xinput1_4.dll");
	if (lib == NULL) {
		lib = LoadLibraryA("xinput1_3.dll");
	}
	if (lib == NULL) {
		lib = LoadLibraryA("xinput9_1_0.dll");
	}

	if (lib != NULL) {
		InputGetState = (XInputGetStateType)GetProcAddress(lib, "XInputGetState");
		InputSetState = (XInputSetStateType)GetProcAddress(lib, "XInputSetState");
	}
}

static IDirectInput8W *di_instance = NULL;
static IDirectInputDevice8W *di_pads[KORE_DINPUT_MAX_COUNT];
static DIJOYSTATE2 di_padState[KORE_DINPUT_MAX_COUNT];
static DIJOYSTATE2 di_lastPadState[KORE_DINPUT_MAX_COUNT];
static DIDEVCAPS di_deviceCaps[KORE_DINPUT_MAX_COUNT];
static int padCount = 0;

static void cleanupPad(int padIndex) {
	if (di_pads[padIndex] != NULL) {
		di_pads[padIndex]->lpVtbl->Unacquire(di_pads[padIndex]);
		di_pads[padIndex]->lpVtbl->Release(di_pads[padIndex]);
		di_pads[padIndex] = 0;
	}
}

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)        \
	if (x != NULL) {           \
		x->lpVtbl->Release(x); \
		x = NULL;              \
	}
#endif

// From
//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput
//-----------------------------------------------------------------------------
static BOOL IsXInputDevice(const GUID *pGuidProductFromDirectInput) {
	IWbemLocator *pIWbemLocator        = NULL;
	IEnumWbemClassObject *pEnumDevices = NULL;
	IWbemClassObject *pDevices[20]     = {0};
	IWbemServices *pIWbemServices      = NULL;
	BSTR bstrNamespace                 = NULL;
	BSTR bstrDeviceID                  = NULL;
	BSTR bstrClassName                 = NULL;
	DWORD uReturned                    = 0;
	bool bIsXinputDevice               = false;
	UINT iDevice                       = 0;
	VARIANT var;
	HRESULT hr;

	// CoInit if needed
	hr               = CoInitialize(NULL);
	bool bCleanupCOM = SUCCEEDED(hr);

	// Create WMI
	hr = CoCreateInstance(&CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, &IID_IWbemLocator, (LPVOID *)&pIWbemLocator);
	if (FAILED(hr) || pIWbemLocator == NULL)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");
	if (bstrNamespace == NULL)
		goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");
	if (bstrClassName == NULL)
		goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");
	if (bstrDeviceID == NULL)
		goto LCleanup;

	// Connect to WMI
	hr = pIWbemLocator->lpVtbl->ConnectServer(pIWbemLocator, bstrNamespace, NULL, NULL, 0L, 0L, NULL, NULL, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == NULL)
		goto LCleanup;

	// Switch security level to IMPERSONATE.
	CoSetProxyBlanket((IUnknown *)pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
	                  EOAC_NONE);

	hr = pIWbemServices->lpVtbl->CreateInstanceEnum(pIWbemServices, bstrClassName, 0, NULL, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == NULL)
		goto LCleanup;

	// Loop over all devices
	for (;;) {
		// Get 20 at a time
		hr = pEnumDevices->lpVtbl->Next(pEnumDevices, 10000, 20, pDevices, &uReturned);
		if (FAILED(hr))
			goto LCleanup;
		if (uReturned == 0)
			break;

		for (iDevice = 0; iDevice < uReturned; iDevice++) {
			// For each device, get its device ID
			hr = pDevices[iDevice]->lpVtbl->Get(pDevices[iDevice], bstrDeviceID, 0L, &var, NULL, NULL);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL) {
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
				// This information can not be found from DirectInput
				// TODO: Doesn't work with an Xbox Series X|S controller
				if (wcsstr(var.bstrVal, L"IG_")) {
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR *strVid = wcsstr(var.bstrVal, L"VID_");
#ifndef KORE_NO_CLIB
					if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1) {
						dwVid = 0;
					}
					WCHAR *strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1) {
						dwPid = 0;
					}
#endif

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if (dwVidPid == pGuidProductFromDirectInput->Data1) {
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			SAFE_RELEASE(pDevices[iDevice]);
		}
	}

LCleanup:
	if (bstrNamespace)
		SysFreeString(bstrNamespace);
	if (bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if (bstrClassName)
		SysFreeString(bstrClassName);
	for (iDevice = 0; iDevice < 20; iDevice++)
		SAFE_RELEASE(pDevices[iDevice]);
	SAFE_RELEASE(pEnumDevices);
	SAFE_RELEASE(pIWbemLocator);
	SAFE_RELEASE(pIWbemServices);

	if (bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}

// TODO (DK) this should probably be called from somewhere?
static void cleanupDirectInput() {
	for (int padIndex = 0; padIndex < KORE_DINPUT_MAX_COUNT; ++padIndex) {
		cleanupPad(padIndex);
	}

	if (di_instance != NULL) {
		di_instance->lpVtbl->Release(di_instance);
		di_instance = NULL;
	}
}

static BOOL CALLBACK enumerateJoystickAxesCallback(LPCDIDEVICEOBJECTINSTANCEW ddoi, LPVOID context) {
	HWND hwnd = (HWND)context;

	DIPROPRANGE propertyRange;
	propertyRange.diph.dwSize       = sizeof(DIPROPRANGE);
	propertyRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propertyRange.diph.dwHow        = DIPH_BYID;
	propertyRange.diph.dwObj        = ddoi->dwType;
	propertyRange.lMin              = -32768;
	propertyRange.lMax              = 32768;

	HRESULT hr = di_pads[padCount]->lpVtbl->SetProperty(di_pads[padCount], DIPROP_RANGE, &propertyRange.diph);

	if (FAILED(hr)) {
		kore_log(KORE_LOG_LEVEL_WARNING, "DirectInput8 / Pad%i / SetProperty() failed (HRESULT=0x%x)", padCount, hr);

		// TODO (DK) cleanup?
		// cleanupPad(padCount);

		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

static BOOL CALLBACK enumerateJoysticksCallback(LPCDIDEVICEINSTANCEW ddi, LPVOID context) {
	if (padCount < XUSER_MAX_COUNT && IsXInputDevice(&ddi->guidProduct)) {
		++padCount;
		return DIENUM_CONTINUE;
	}

	HRESULT hr = di_instance->lpVtbl->CreateDevice(di_instance, &ddi->guidInstance, &di_pads[padCount], NULL);

	if (SUCCEEDED(hr)) {
		hr = di_pads[padCount]->lpVtbl->SetDataFormat(di_pads[padCount], &c_dfDIJoystick2);

		// TODO (DK) required?
		// hr = di_pads[padCount]->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

		if (SUCCEEDED(hr)) {
			di_deviceCaps[padCount].dwSize = sizeof(DIDEVCAPS);
			hr                             = di_pads[padCount]->lpVtbl->GetCapabilities(di_pads[padCount], &di_deviceCaps[padCount]);

			if (SUCCEEDED(hr)) {
				hr = di_pads[padCount]->lpVtbl->EnumObjects(di_pads[padCount], enumerateJoystickAxesCallback, NULL, DIDFT_AXIS);

				if (SUCCEEDED(hr)) {
					hr = di_pads[padCount]->lpVtbl->Acquire(di_pads[padCount]);

					if (SUCCEEDED(hr)) {
						memset(&di_padState[padCount], 0, sizeof(DIJOYSTATE2));
						hr = di_pads[padCount]->lpVtbl->GetDeviceState(di_pads[padCount], sizeof(DIJOYSTATE2), &di_padState[padCount]);

						if (SUCCEEDED(hr)) {
							kore_log(KORE_LOG_LEVEL_INFO, "DirectInput8 / Pad%i / initialized", padCount);
						}
						else {
							kore_log(KORE_LOG_LEVEL_WARNING, "DirectInput8 / Pad%i / GetDeviceState() failed (HRESULT=0x%x)", padCount, hr);
							// cleanupPad(padCount); // (DK) don't kill it, we try again in handleDirectInputPad()
						}
					}
					else {
						kore_log(KORE_LOG_LEVEL_WARNING, "DirectInput8 / Pad%i / Acquire() failed (HRESULT=0x%x)", padCount, hr);
						cleanupPad(padCount);
					}
				}
				else {
					kore_log(KORE_LOG_LEVEL_WARNING, "DirectInput8 / Pad%i / EnumObjects(DIDFT_AXIS) failed (HRESULT=0x%x)", padCount, hr);
					cleanupPad(padCount);
				}
			}
			else {
				kore_log(KORE_LOG_LEVEL_WARNING, "DirectInput8 / Pad%i / GetCapabilities() failed (HRESULT=0x%x)", padCount, hr);
				cleanupPad(padCount);
			}
		}
		else {
			kore_log(KORE_LOG_LEVEL_WARNING, "DirectInput8 / Pad%i / SetDataFormat() failed (HRESULT=0x%x)", padCount, hr);
			cleanupPad(padCount);
		}

		++padCount;

		if (padCount >= KORE_DINPUT_MAX_COUNT) {
			return DIENUM_STOP;
		}
	}

	return DIENUM_CONTINUE;
}

static void initializeDirectInput() {
	HINSTANCE hinstance = GetModuleHandleW(NULL);

	memset(&di_pads, 0, sizeof(IDirectInputDevice8) * KORE_DINPUT_MAX_COUNT);
	memset(&di_padState, 0, sizeof(DIJOYSTATE2) * KORE_DINPUT_MAX_COUNT);
	memset(&di_lastPadState, 0, sizeof(DIJOYSTATE2) * KORE_DINPUT_MAX_COUNT);
	memset(&di_deviceCaps, 0, sizeof(DIDEVCAPS) * KORE_DINPUT_MAX_COUNT);

	HRESULT hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, &IID_IDirectInput8W, (void **)&di_instance, NULL);

	if (SUCCEEDED(hr)) {
		hr = di_instance->lpVtbl->EnumDevices(di_instance, DI8DEVCLASS_GAMECTRL, enumerateJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);

		if (SUCCEEDED(hr)) {
		}
		else {
			cleanupDirectInput();
		}
	}
	else {
		kore_log(KORE_LOG_LEVEL_WARNING, "DirectInput8Create failed (HRESULT=0x%x)", hr);
	}
}

bool handleDirectInputPad(int padIndex) {
	if (di_pads[padIndex] == NULL) {
		return false;
	}

	HRESULT hr = di_pads[padIndex]->lpVtbl->GetDeviceState(di_pads[padIndex], sizeof(DIJOYSTATE2), &di_padState[padIndex]);

	switch (hr) {
	case S_OK: {
		// TODO (DK) there is a lot more to handle
		for (int axisIndex = 0; axisIndex < 2; ++axisIndex) {
			LONG *now  = NULL;
			LONG *last = NULL;

			switch (axisIndex) {
			case 0: {
				now  = &di_padState[padIndex].lX;
				last = &di_lastPadState[padIndex].lX;
			} break;
			case 1: {
				now  = &di_padState[padIndex].lY;
				last = &di_lastPadState[padIndex].lY;
			} break;
			case 2: {
				now  = &di_padState[padIndex].lZ;
				last = &di_lastPadState[padIndex].lZ;
			} break;
			}

			if (*now != *last) {
				kore_internal_gamepad_trigger_axis(padIndex, axisIndex, *now / 32768.0f);
			}
		}

		for (int buttonIndex = 0; buttonIndex < 128; ++buttonIndex) {
			BYTE *now  = &di_padState[padIndex].rgbButtons[buttonIndex];
			BYTE *last = &di_lastPadState[padIndex].rgbButtons[buttonIndex];

			if (*now != *last) {
				kore_internal_gamepad_trigger_button(padIndex, buttonIndex, *now / 255.0f);
			}
		}

		for (int povIndex = 0; povIndex < 4; ++povIndex) {
			DWORD *now  = &di_padState[padIndex].rgdwPOV[povIndex];
			DWORD *last = &di_lastPadState[padIndex].rgdwPOV[povIndex];

			bool up    = (*now == 0 || *now == 31500 || *now == 4500);
			bool down  = (*now == 18000 || *now == 13500 || *now == 22500);
			bool left  = (*now == 27000 || *now == 22500 || *now == 31500);
			bool right = (*now == 9000 || *now == 4500 || *now == 13500);

			bool lastUp    = (*last == 0 || *last == 31500 || *last == 4500);
			bool lastDown  = (*last == 18000 || *last == 13500 || *last == 22500);
			bool lastLeft  = (*last == 27000 || *last == 22500 || *last == 31500);
			bool lastRight = (*last == 9000 || *last == 4500 || *last == 13500);

			if (up != lastUp) {
				kore_internal_gamepad_trigger_button(padIndex, 12, up ? 1.0f : 0.0f);
			}
			if (down != lastDown) {
				kore_internal_gamepad_trigger_button(padIndex, 13, down ? 1.0f : 0.0f);
			}
			if (left != lastLeft) {
				kore_internal_gamepad_trigger_button(padIndex, 14, left ? 1.0f : 0.0f);
			}
			if (right != lastRight) {
				kore_internal_gamepad_trigger_button(padIndex, 15, right ? 1.0f : 0.0f);
			}
		}

		memcpy(&di_lastPadState[padIndex], &di_padState[padIndex], sizeof(DIJOYSTATE2));
		break;
	}
	case DIERR_INPUTLOST: // fall through
	case DIERR_NOTACQUIRED: {
		hr = di_pads[padIndex]->lpVtbl->Acquire(di_pads[padIndex]);
		break;
	}
	}

	return hr == S_OK;
}

static bool isXInputGamepad(int gamepad) {
	// if gamepad is greater than XInput max, treat it as DINPUT.
	if (gamepad >= XUSER_MAX_COUNT) {
		return false;
	}
	XINPUT_STATE state;
	memset(&state, 0, sizeof(XINPUT_STATE));
	DWORD dwResult = InputGetState(gamepad, &state);
	return dwResult == ERROR_SUCCESS;
}

static bool isDirectInputGamepad(int gamepad) {
	if (di_pads[gamepad] == NULL) {
		return false;
	}
	HRESULT hr = di_pads[gamepad]->lpVtbl->GetDeviceState(di_pads[gamepad], sizeof(DIJOYSTATE2), &di_padState[gamepad]);
	return hr == S_OK;
}

const char *kore_gamepad_vendor(int gamepad) {
	if (isXInputGamepad(gamepad)) {
		return "Microsoft";
	}
	else {
		return "DirectInput8";
	}
}

const char *kore_gamepad_product_name(int gamepad) {
	if (isXInputGamepad(gamepad)) {
		return "Xbox 360 Controller";
	}
	else {
		return "Generic Gamepad";
	}
}

bool kore_internal_handle_messages() {
	MSG message;

	while (PeekMessageW(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	if (InputGetState != NULL && (detectGamepad || gamepadFound)) {
		detectGamepad = false;
		for (DWORD i = 0; i < KORE_DINPUT_MAX_COUNT; ++i) {
			XINPUT_STATE state;
			memset(&state, 0, sizeof(XINPUT_STATE));
			DWORD dwResult = InputGetState(i, &state);

			if (dwResult == ERROR_SUCCESS) {
				gamepadFound = true;

				float newaxes[6];
				newaxes[0] = state.Gamepad.sThumbLX / 32768.0f;
				newaxes[1] = state.Gamepad.sThumbLY / 32768.0f;
				newaxes[2] = state.Gamepad.sThumbRX / 32768.0f;
				newaxes[3] = state.Gamepad.sThumbRY / 32768.0f;
				newaxes[4] = state.Gamepad.bLeftTrigger / 255.0f;
				newaxes[5] = state.Gamepad.bRightTrigger / 255.0f;
				for (int i2 = 0; i2 < 6; ++i2) {
					if (axes[i * 6 + i2] != newaxes[i2]) {
						kore_internal_gamepad_trigger_axis(i, i2, newaxes[i2]);
						axes[i * 6 + i2] = newaxes[i2];
					}
				}
				float newbuttons[16];
				newbuttons[0]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 1.0f : 0.0f;
				newbuttons[1]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 1.0f : 0.0f;
				newbuttons[2]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 1.0f : 0.0f;
				newbuttons[3]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 1.0f : 0.0f;
				newbuttons[4]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 1.0f : 0.0f;
				newbuttons[5]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 1.0f : 0.0f;
				newbuttons[6]  = state.Gamepad.bLeftTrigger / 255.0f;
				newbuttons[7]  = state.Gamepad.bRightTrigger / 255.0f;
				newbuttons[8]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 1.0f : 0.0f;
				newbuttons[9]  = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 1.0f : 0.0f;
				newbuttons[10] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 1.0f : 0.0f;
				newbuttons[11] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 1.0f : 0.0f;
				newbuttons[12] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 1.0f : 0.0f;
				newbuttons[13] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 1.0f : 0.0f;
				newbuttons[14] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 1.0f : 0.0f;
				newbuttons[15] = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 1.0f : 0.0f;
				for (int i2 = 0; i2 < 16; ++i2) {
					if (buttons[i * 16 + i2] != newbuttons[i2]) {
						kore_internal_gamepad_trigger_button(i, i2, newbuttons[i2]);
						buttons[i * 16 + i2] = newbuttons[i2];
					}
				}
			}
			else {
				if (handleDirectInputPad(i)) {
					gamepadFound = true;
				}
			}
		}
	}

	return true;
}

//**vec2i Kore::System::mousePos() {
//**	return vec2i(mouseX, mouseY);
//**}

static bool keyboardshown = false;
static char language[3]   = {0};

void kore_keyboard_show() {
	keyboardshown = true;
}

void kore_keyboard_hide() {
	keyboardshown = false;
}

bool kore_keyboard_active() {
	return keyboardshown;
}

void kore_load_url(const char *url) {
#define WURL_SIZE 1024
#define HTTP      "http://"
#define HTTPS     "https://"
	if (strncmp(url, HTTP, sizeof(HTTP) - 1) == 0 || strncmp(url, HTTPS, sizeof(HTTPS) - 1) == 0) {
		wchar_t wurl[WURL_SIZE];
		MultiByteToWideChar(CP_UTF8, 0, url, -1, wurl, WURL_SIZE);
		INT_PTR ret = (INT_PTR)ShellExecuteW(NULL, L"open", wurl, NULL, NULL, SW_SHOWNORMAL);
		// According to ShellExecuteW's documentation return values > 32 indicate a success.
		if (ret <= 32) {
			kore_log(KORE_LOG_LEVEL_WARNING, "Error opening url %s", url);
		}
	}
#undef HTTPS
#undef HTTP
#undef WURL_SIZE
}

void kore_set_keep_screen_on(bool on) {}
void kore_vibrate(int ms) {}

const char *kore_language() {
	wchar_t wlanguage[3] = {0};

	if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SISO639LANGNAME, wlanguage, 3)) {
		WideCharToMultiByte(CP_UTF8, 0, wlanguage, -1, language, 3, NULL, NULL);
		return language;
	}
	return "en";
}

const char *kore_system_id() {
	return "Windows";
}

static bool co_initialized = false;

void kore_windows_co_initialize(void) {
	if (!co_initialized) {
		kore_microsoft_affirm(CoInitializeEx(0, COINIT_MULTITHREADED));
		co_initialized = true;
	}
}

static wchar_t savePathw[2048] = {0};
static char savePath[2048]     = {0};

static void findSavePath() {
	kore_windows_co_initialize();
	IKnownFolderManager *folders = NULL;
	CoCreateInstance(&CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, &IID_IKnownFolderManager, (LPVOID *)&folders);
	IKnownFolder *folder = NULL;
	folders->lpVtbl->GetFolder(folders, &FOLDERID_SavedGames, &folder);

	LPWSTR path;
	folder->lpVtbl->GetPath(folder, 0, &path);

	wcscpy(savePathw, path);
	wcscat(savePathw, L"\\");
	wchar_t name[1024];
	MultiByteToWideChar(CP_UTF8, 0, kore_application_name(), -1, name, 1024);
	wcscat(savePathw, name);
	wcscat(savePathw, L"\\");

	SHCreateDirectoryExW(NULL, savePathw, NULL);
	WideCharToMultiByte(CP_UTF8, 0, savePathw, -1, savePath, 1024, NULL, NULL);

	CoTaskMemFree(path);
	folder->lpVtbl->Release(folder);
	folders->lpVtbl->Release(folders);
	// CoUninitialize();
}

const char *kore_internal_save_path() {
	if (savePath[0] == 0)
		findSavePath();
	return savePath;
}

static const char *videoFormats[] = {"ogv", NULL};
static LARGE_INTEGER frequency;
static LARGE_INTEGER startCount;

const char **kore_video_formats() {
	return videoFormats;
}

void kore_login(void) {}

void kore_unlock_achievement(int id) {}

bool kore_gamepad_connected(int num) {
	return isXInputGamepad(num) || isDirectInputGamepad(num);
}

void kore_gamepad_rumble(int gamepad, float left, float right) {
	if (isXInputGamepad(gamepad)) {
		XINPUT_VIBRATION vibration;
		memset(&vibration, 0, sizeof(XINPUT_VIBRATION));
		vibration.wLeftMotorSpeed  = (WORD)(65535.f * left);
		vibration.wRightMotorSpeed = (WORD)(65535.f * right);
		InputSetState(gamepad, &vibration);
	}
}

double kore_frequency() {
	return (double)frequency.QuadPart;
}

kore_ticks kore_timestamp(void) {
	LARGE_INTEGER stamp;
	QueryPerformanceCounter(&stamp);
	return stamp.QuadPart - startCount.QuadPart;
}

double kore_time(void) {
	LARGE_INTEGER stamp;
	QueryPerformanceCounter(&stamp);
	return (double)(stamp.QuadPart - startCount.QuadPart) / (double)frequency.QuadPart;
}

#if !defined(KORE_NO_MAIN) && !defined(KORE_NO_CLIB)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	int ret = kickstart(__argc, __argv);
	if (ret != 0) {
#ifdef NDEBUG
		MessageBox(0, L"Unknown Error", L"Error", MB_OK);
#else
		__debugbreak();
#endif
	}
	return ret;
}
#endif

typedef BOOL(__stdcall *MiniDumpWriteDumpType)(IN HANDLE hProcess, IN DWORD ProcessId, IN HANDLE hFile, IN MINIDUMP_TYPE DumpType,
                                               IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                               OPTIONAL IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                               OPTIONAL IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL);

static MiniDumpWriteDumpType MyMiniDumpWriteDump;

static LONG __stdcall MyCrashHandlerExceptionFilter(EXCEPTION_POINTERS *pEx) {
	HANDLE file = CreateFileA("kore.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE) {
		MINIDUMP_EXCEPTION_INFORMATION stMDEI;
		stMDEI.ThreadId          = GetCurrentThreadId();
		stMDEI.ExceptionPointers = pEx;
		stMDEI.ClientPointers    = TRUE;
		MyMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpNormal, &stMDEI, NULL, NULL);
		CloseHandle(file);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

static void init_crash_handler() {
	HMODULE dbghelp = LoadLibraryA("dbghelp.dll");
	if (dbghelp != NULL) {
		MyMiniDumpWriteDump = (MiniDumpWriteDumpType)GetProcAddress(dbghelp, "MiniDumpWriteDump");
		SetUnhandledExceptionFilter(MyCrashHandlerExceptionFilter);
	}
}

int kore_init(const char *name, int width, int height, kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	init_crash_handler();

#if defined(KORE_VTUNE)
	kore_itt_domain = __itt_domain_create(name);
#endif

	// Pen functions are only in Windows 8 and later, so load them dynamically
	HMODULE user32              = LoadLibraryA("user32.dll");
	MyGetPointerInfo            = (GetPointerInfoType)GetProcAddress(user32, "GetPointerInfo");
	MyGetPointerPenInfo         = (GetPointerPenInfoType)GetProcAddress(user32, "GetPointerPenInfo");
	MyEnableNonClientDpiScaling = (EnableNonClientDpiScalingType)GetProcAddress(user32, "EnableNonClientDpiScaling");
	initKeyTranslation();
	for (int i = 0; i < 256; ++i) {
		keyPressed[i] = false;
	}

	for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
		touchPoints[i].sysID = -1;
		touchPoints[i].x     = -1;
		touchPoints[i].y     = -1;
	}

	kore_display_init();

	QueryPerformanceCounter(&startCount);
	QueryPerformanceFrequency(&frequency);

	for (int i = 0; i < 256; ++i) {
		keyPressed[i] = false;
	}

	kore_set_application_name(name);
	kore_window_parameters defaultWin;
	if (win == NULL) {
		kore_window_options_set_defaults(&defaultWin);
		win = &defaultWin;
	}
	win->width  = width;
	win->height = height;
	if (win->title == NULL) {
		win->title = name;
	}

	// kore_g4_internal_init(); // TODO

	int window = kore_window_create(win, frame);
	loadXInput();
	initializeDirectInput();

	return window;
}

void kore_internal_shutdown() {
	kore_windows_hide_windows();
	kore_internal_shutdown_callback();
	kore_windows_destroy_windows();
	// kore_g4_internal_destroy(); // TODO
	kore_windows_restore_displays();
}

void kore_copy_to_clipboard(const char *text) {
	wchar_t wtext[4096];
	MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, 4096);
	OpenClipboard(kore_windows_window_handle(0));
	EmptyClipboard();
	size_t size   = (wcslen(wtext) + 1) * sizeof(wchar_t);
	HANDLE handle = GlobalAlloc(GMEM_MOVEABLE, size);
	void *data    = GlobalLock(handle);
	memcpy(data, wtext, size);
	GlobalUnlock(handle);
	SetClipboardData(CF_UNICODETEXT, handle);
	CloseClipboard();
}

int kore_cpu_cores(void) {
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION info[1024];
	DWORD returnLength = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) * 1024;
	BOOL success       = GetLogicalProcessorInformation(&info[0], &returnLength);

	int proper_cpu_count = 0;

	if (success) {
		DWORD byteOffset                          = 0;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = &info[0];
		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) {
			if (ptr->Relationship == RelationProcessorCore) {
				++proper_cpu_count;
			}

			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			++ptr;
		}
	}
	else {
		proper_cpu_count = 1;
	}

	return proper_cpu_count;
}

int kore_hardware_threads(void) {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}
