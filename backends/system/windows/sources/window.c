#include <kore3/display.h>
#include <kore3/gpu/gpu.h>
#include <kore3/window.h>

#include <kore3/backend/windows.h>

#ifdef KORE_VR
#include <kore/vr/vr.h>
#endif

#undef CreateWindow

struct HWND__;
typedef unsigned long DWORD;

typedef struct {
	struct HWND__ *handle;
	int            display_index;
	bool           mouseInside;
	int            index;
	int            x, y, mode, bpp, frequency, features;
	int            manualWidth, manualHeight;
	void (*resizeCallback)(int x, int y, void *data);
	void *resizeCallbackData;
	void (*ppiCallback)(int ppi, void *data);
	void *ppiCallbackData;
	bool (*closeCallback)(void *data);
	void *closeCallbackData;
} WindowData;

LRESULT WINAPI KoreWindowsMessageProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define MAXIMUM_WINDOWS 16
static WindowData windows[MAXIMUM_WINDOWS] = {0};
static int        window_counter           = 0;

#ifdef KORE_OCULUS
const wchar_t *windowClassName = L"ORT";
#else
const wchar_t *windowClassName = L"KoreWindow";
#endif

#ifdef KORE_VULKAN
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

VkResult kore_vulkan_create_surface(VkInstance instance, int window_index, VkSurfaceKHR *surface) {
	VkWin32SurfaceCreateInfoKHR createInfo = {0};
	createInfo.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext                       = NULL;
	createInfo.flags                       = 0;
	createInfo.hinstance                   = GetModuleHandle(NULL);
	createInfo.hwnd                        = windows[window_index].handle;
	return vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, surface);
}

#include <assert.h>

void kore_vulkan_get_instance_extensions(const char **names, int *index, int max) {
	assert(*index + 1 < max);
	names[(*index)++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

VkBool32 kore_vulkan_get_physical_device_presentation_support(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
	return vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
}
#endif

static void RegisterWindowClass(HINSTANCE hInstance, const wchar_t *className) {
	WNDCLASSEXW wc = {sizeof(WNDCLASSEXA),
	                  CS_OWNDC /*CS_CLASSDC*/,
	                  KoreWindowsMessageProcedure,
	                  0L,
	                  0L,
	                  hInstance,
	                  LoadIconW(hInstance, MAKEINTRESOURCEW(107)),
	                  LoadCursor(NULL, IDC_ARROW),
	                  0,
	                  0,
	                  className,
	                  0};
	RegisterClassExW(&wc);
}

static DWORD getStyle(int features) {
	DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

	if ((features & KORE_WINDOW_FEATURE_RESIZEABLE) && ((features & KORE_WINDOW_FEATURE_BORDERLESS) == 0)) {
		style |= WS_SIZEBOX;
	}

	if (features & KORE_WINDOW_FEATURE_MAXIMIZABLE) {
		style |= WS_MAXIMIZEBOX;
	}

	if (features & KORE_WINDOW_FEATURE_MINIMIZABLE) {
		style |= WS_MINIMIZEBOX;
	}

	if ((features & KORE_WINDOW_FEATURE_BORDERLESS) == 0) {
		style |= WS_CAPTION | WS_SYSMENU;
	}

	return style;
}

static DWORD getExStyle(int features) {
	DWORD exStyle = WS_EX_APPWINDOW;

	if ((features & KORE_WINDOW_FEATURE_BORDERLESS) == 0) {
		exStyle |= WS_EX_WINDOWEDGE;
	}

	if (features & KORE_WINDOW_FEATURE_ON_TOP) {
		exStyle |= WS_EX_TOPMOST;
	}

	return exStyle;
}

int kore_windows_window_index_from_hwnd(struct HWND__ *handle) {
	for (int i = 0; i < MAXIMUM_WINDOWS; ++i) {
		if (windows[i].handle == handle) {
			return i;
		}
	}
	return -1;
}

int kore_count_windows() {
	return window_counter;
}

int kore_window_x(int window_index) {
	RECT rect;
	GetWindowRect(windows[window_index].handle, &rect);
	windows[window_index].x = rect.left;
	return windows[window_index].x;
}

int kore_window_y(int window_index) {
	RECT rect;
	GetWindowRect(windows[window_index].handle, &rect);
	windows[window_index].y = rect.top;
	return windows[window_index].y;
}

int kore_window_width(int window_index) {
	RECT rect;
	GetClientRect(windows[window_index].handle, &rect);
	return rect.right;
}

int kore_window_height(int window_index) {
	RECT rect;
	GetClientRect(windows[window_index].handle, &rect);
	return rect.bottom;
}

static DWORD getDwStyle(kore_window_mode mode, int features) {
	switch (mode) {
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN: {
		return WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
	}
	case KORE_WINDOW_MODE_FULLSCREEN:
		return WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
	case KORE_WINDOW_MODE_WINDOW:
	default:
		return getStyle(features);
	}
}

static DWORD getDwExStyle(kore_window_mode mode, int features) {
	switch (mode) {
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN: {
		return WS_EX_APPWINDOW;
	}
	case KORE_WINDOW_MODE_FULLSCREEN:
		return WS_EX_APPWINDOW;
	case KORE_WINDOW_MODE_WINDOW:
	default:
		return getExStyle(features);
	}
}

static int createWindow(const wchar_t *title, int x, int y, int width, int height, int bpp, int frequency, int features, kore_window_mode windowMode,
                        int target_display_index) {
	HINSTANCE inst = GetModuleHandleW(NULL);

	if (window_counter == 0) {
		RegisterWindowClass(inst, windowClassName);
	}

	int display_index = target_display_index == -1 ? kore_primary_display() : target_display_index;

#ifdef KORE_VR
	int dstx = 0;
	int dsty = 0;

	char titleutf8[1024];
	char classNameutf8[1024];
	WideCharToMultiByte(CP_UTF8, 0, title, -1, titleutf8, 1024 - 1, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, windowClassName, -1, classNameutf8, 1024 - 1, NULL, NULL);

	HWND hwnd = (HWND)kore_vr_interface_init(inst, titleutf8, classNameutf8);
#else
	RECT WindowRect;
	WindowRect.left   = 0;
	WindowRect.right  = width;
	WindowRect.top    = 0;
	WindowRect.bottom = height;

	if (windowMode == KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
		kore_windows_set_display_mode(display_index, width, height, bpp, frequency);
	}

	AdjustWindowRectEx(&WindowRect, getDwStyle(windowMode, features), FALSE, getDwExStyle(windowMode, features));

	kore_display_mode display_mode = kore_display_current_mode(display_index);

	int dstx = display_mode.x;
	int dsty = display_mode.y;
	int dstw = width;
	int dsth = height;

	switch (windowMode) {
	case KORE_WINDOW_MODE_WINDOW:
		dstx += x < 0 ? (display_mode.width - width) / 2 : x;
		dsty += y < 0 ? (display_mode.height - height) / 2 : y;
		dstw = WindowRect.right - WindowRect.left;
		dsth = WindowRect.bottom - WindowRect.top;
		break;
	case KORE_WINDOW_MODE_FULLSCREEN:
		dstw = display_mode.width;
		dsth = display_mode.height;
		break;
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
		break;
	}

	HWND hwnd = CreateWindowExW(getDwExStyle(windowMode, features), windowClassName, title, getDwStyle(windowMode, features), dstx, dsty, dstw, dsth, NULL,
	                            NULL, inst, NULL);
#endif

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	DragAcceptFiles(hwnd, true);

	windows[window_counter].handle        = hwnd;
	windows[window_counter].x             = dstx;
	windows[window_counter].y             = dsty;
	windows[window_counter].mode          = windowMode;
	windows[window_counter].display_index = display_index;
	windows[window_counter].bpp           = bpp;
	windows[window_counter].frequency     = frequency;
	windows[window_counter].features      = features;
	windows[window_counter].manualWidth   = width;
	windows[window_counter].manualHeight  = height;
	windows[window_counter].index         = window_counter;

	return window_counter++;
}

void kore_window_resize(int window_index, int width, int height) {
	WindowData *win   = &windows[window_index];
	win->manualWidth  = width;
	win->manualHeight = height;
	switch (win->mode) {
	case KORE_WINDOW_MODE_WINDOW: {
		RECT rect;
		rect.left   = 0;
		rect.top    = 0;
		rect.right  = width;
		rect.bottom = height;
		AdjustWindowRectEx(&rect, getDwStyle((kore_window_mode)win->mode, win->features), FALSE, getDwExStyle((kore_window_mode)win->mode, win->features));
		SetWindowPos(win->handle, NULL, kore_window_x(window_index), kore_window_y(window_index), rect.right - rect.left, rect.bottom - rect.top, 0);
		break;
	}
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN: {
		int               display_index = kore_window_display(window_index);
		kore_display_mode display_mode  = kore_display_current_mode(display_index);
		kore_windows_set_display_mode(display_index, width, height, win->bpp, win->frequency);
		SetWindowPos(win->handle, NULL, display_mode.x, display_mode.y, display_mode.width, display_mode.height, 0);
		break;
	}
	}
}

void kore_window_move(int window_index, int x, int y) {
	WindowData *win = &windows[window_index];

	if (win->mode != 0) {
		return;
	}

	win->x = x;
	win->y = y;

	RECT rect;
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = kore_window_width(window_index);
	rect.bottom = kore_window_height(window_index);
	AdjustWindowRectEx(&rect, getDwStyle((kore_window_mode)win->mode, win->features), FALSE, getDwExStyle((kore_window_mode)win->mode, win->features));

	SetWindowPos(win->handle, NULL, x, y, rect.right - rect.left, rect.bottom - rect.top, 0);
}

void kore_internal_change_framebuffer(int window, struct kore_framebuffer_options *frame);

void kore_window_change_framebuffer(int window, kore_framebuffer_parameters *frame) {
	// kore_internal_change_framebuffer(window, frame); // TODO
}

void kore_window_change_features(int window_index, int features) {
	WindowData *win = &windows[window_index];
	win->features   = features;
	SetWindowLongW(win->handle, GWL_STYLE, getStyle(features));
	SetWindowLongW(win->handle, GWL_EXSTYLE, getExStyle(features));

	HWND on_top = (features & KORE_WINDOW_FEATURE_ON_TOP) ? HWND_TOPMOST : HWND_NOTOPMOST;
	SetWindowPos(win->handle, on_top, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

	kore_window_show(window_index);
}

void kore_window_change_mode(int window_index, kore_window_mode mode) {
	WindowData       *win           = &windows[window_index];
	int               display_index = kore_window_display(window_index);
	kore_display_mode display_mode  = kore_display_current_mode(display_index);
	switch (mode) {
	case KORE_WINDOW_MODE_WINDOW:
		kore_windows_restore_display(display_index);
		kore_window_change_features(window_index, win->features);
		kore_window_show(window_index);
		break;
	case KORE_WINDOW_MODE_FULLSCREEN: {
		kore_windows_restore_display(display_index);
		SetWindowLongW(win->handle, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
		SetWindowLongW(win->handle, GWL_EXSTYLE, WS_EX_APPWINDOW);
		SetWindowPos(win->handle, NULL, display_mode.x, display_mode.y, display_mode.width, display_mode.height, 0);
		kore_window_show(window_index);
		break;
	}
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
		kore_windows_set_display_mode(display_index, win->manualWidth, win->manualHeight, win->bpp, win->frequency);
		SetWindowLongW(win->handle, GWL_STYLE, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP);
		SetWindowLongW(win->handle, GWL_EXSTYLE, WS_EX_APPWINDOW);
		SetWindowPos(win->handle, NULL, display_mode.x, display_mode.y, display_mode.width, display_mode.height, 0);
		kore_window_show(window_index);
		break;
	}
	win->mode = mode;
}

kore_window_mode kore_window_get_mode(int window_index) {
	return (kore_window_mode)windows[window_index].mode;
}

void kore_window_destroy(int window_index) {
	WindowData *win = &windows[window_index];
	if (win->handle != NULL) {
		// kore_g4_internal_destroy_window(window_index); // TODO
		DestroyWindow(win->handle);
		win->handle = NULL;
		--window_counter;
	}
}

void kore_windows_hide_windows(void) {
	for (int i = 0; i < MAXIMUM_WINDOWS; ++i) {
		if (windows[i].handle != NULL) {
			ShowWindow(windows[i].handle, SW_HIDE);
			UpdateWindow(windows[i].handle);
		}
	}
}

void kore_windows_destroy_windows(void) {
	for (int i = 0; i < MAXIMUM_WINDOWS; ++i) {
		kore_window_destroy(i);
	}
	UnregisterClassW(windowClassName, GetModuleHandleW(NULL));
}

void kore_window_show(int window_index) {
	ShowWindow(windows[window_index].handle, SW_SHOWDEFAULT);
	UpdateWindow(windows[window_index].handle);
}

void kore_window_hide(int window_index) {
	ShowWindow(windows[window_index].handle, SW_HIDE);
	UpdateWindow(windows[window_index].handle);
}

void kore_window_set_title(int window_index, const char *title) {
	wchar_t buffer[1024];
	MultiByteToWideChar(CP_UTF8, 0, title, -1, buffer, 1024);
	SetWindowTextW(windows[window_index].handle, buffer);
}

int kore_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	kore_window_parameters      defaultWin;
	kore_framebuffer_parameters defaultFrame;

	if (win == NULL) {
		kore_window_options_set_defaults(&defaultWin);
		win = &defaultWin;
	}

	if (frame == NULL) {
		kore_framebuffer_options_set_defaults(&defaultFrame);
		frame = &defaultFrame;
	}

	if (win->title == NULL) {
		win->title = "";
	}

	wchar_t wbuffer[1024];
	MultiByteToWideChar(CP_UTF8, 0, win->title, -1, wbuffer, 1024);

	int windowId = createWindow(wbuffer, win->x, win->y, win->width, win->height, frame->color_bits, frame->frequency, win->window_features, win->mode,
	                            win->display_index);

	// kore_g4_set_antialiasing_samples(frame->samples_per_pixel); // TODO
	bool vsync = frame->vertical_sync;
#ifdef KORE_OCULUS
	vsync = false;
#endif

	// kore_g4_internal_init_window(windowId, frame->depth_bits, frame->stencil_bits, vsync); // TODO

	if (win->visible) {
		kore_window_show(windowId);
	}

	return windowId;
}

void kore_window_set_resize_callback(int window_index, void (*callback)(int x, int y, void *data), void *data) {
	windows[window_index].resizeCallback     = callback;
	windows[window_index].resizeCallbackData = data;
}

void kore_window_set_ppi_changed_callback(int window_index, void (*callback)(int ppi, void *data), void *data) {
	windows[window_index].ppiCallback     = callback;
	windows[window_index].ppiCallbackData = data;
}

void kore_window_set_close_callback(int window_index, bool (*callback)(void *data), void *data) {
	windows[window_index].closeCallback     = callback;
	windows[window_index].closeCallbackData = data;
}

int kore_window_display(int window_index) {
	return kore_windows_get_display_for_monitor(MonitorFromWindow(windows[window_index].handle, MONITOR_DEFAULTTOPRIMARY));
}

struct HWND__ *kore_windows_window_handle(int window_index) {
	return windows[window_index].handle;
}

int kore_windows_manual_width(int window) {
	return windows[window].manualWidth;
}

int kore_windows_manual_height(int window) {
	return windows[window].manualHeight;
}

void kore_internal_call_resize_callback(int window_index, int width, int height) {
	if (windows[window_index].resizeCallback != NULL) {
		windows[window_index].resizeCallback(width, height, windows[window_index].resizeCallbackData);
	}
}

void kore_internal_call_ppi_changed_callback(int window_index, int ppi) {
	if (windows[window_index].ppiCallback != NULL) {
		windows[window_index].ppiCallback(ppi, windows[window_index].ppiCallbackData);
	}
}

bool kore_internal_call_close_callback(int window_index) {
	if (windows[window_index].closeCallback != NULL) {
		return windows[window_index].closeCallback(windows[window_index].closeCallbackData);
	}
	return true;
}
