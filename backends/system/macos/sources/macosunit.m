#include <stdbool.h>

struct WindowData {
	id   handle;
	id   view;
	bool fullscreen;
	void (*resizeCallback)(int width, int height, void *data);
	void *resizeCallbackData;
	bool (*closeCallback)(void *data);
	void *closeCallbackData;
};

static struct WindowData windows[10]   = {};
static int               windowCounter = 0;

#include "BasicOpenGLView.m"
#include "HIDGamepad.c"
#include "HIDManager.c"
#include "audio.c"
#include "display.m"
#include "mouse.m"
#include "system.c"
#include "system.m"
#include "window.c"
