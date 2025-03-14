#include <kore3/backend/x11.h>

#include <stdlib.h>

struct MwmHints {
	// These correspond to XmRInt resources. (VendorSE.c)
	int flags;
	int functions;
	int decorations;
	int input_mode;
	int status;
};
#define MWM_HINTS_DECORATIONS (1L << 1)

void kore_x11_window_set_title(int window_index, const char *title);
void kore_x11_window_change_mode(int window_index, kore_window_mode mode);

int kore_x11_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	int window_index = -1;
	for (int i = 0; i < MAXIMUM_WINDOWS; i++) {
		if (x11_ctx.windows[i].window == None) {
			window_index = i;
			break;
		}
	}

	if (window_index == -1) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Too much windows (maximum is %i)", MAXIMUM_WINDOWS);
		exit(1);
	}

	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	window->window_index           = window_index;
	window->width                  = win->width;
	window->height                 = win->height;

	Visual              *visual             = NULL;
	XSetWindowAttributes set_window_attribs = {0};

	set_window_attribs.border_pixel = 0;
	set_window_attribs.event_mask =
	    KeyPressMask | KeyReleaseMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask | FocusChangeMask;
	int screen                  = DefaultScreen(x11_ctx.display);
	visual                      = DefaultVisual(x11_ctx.display, screen);
	int depth                   = DefaultDepth(x11_ctx.display, screen);
	set_window_attribs.colormap = xlib.XCreateColormap(x11_ctx.display, RootWindow(x11_ctx.display, screen), visual, AllocNone);
	window->window = xlib.XCreateWindow(x11_ctx.display, RootWindow(x11_ctx.display, DefaultScreen(x11_ctx.display)), 0, 0, win->width, win->height, 0, depth,
	                                    InputOutput, visual, CWBorderPixel | CWColormap | CWEventMask, &set_window_attribs);

	static char        nameClass[256];
	static const char *nameClassAddendum = "_KoreApplication";
	strncpy(nameClass, kore_application_name(), sizeof(nameClass) - strlen(nameClassAddendum) - 1);
	strcat(nameClass, nameClassAddendum);
	char resNameBuffer[256];
	strncpy(resNameBuffer, kore_application_name(), 256);
	XClassHint classHint = {.res_name = resNameBuffer, .res_class = nameClass};
	xlib.XSetClassHint(x11_ctx.display, window->window, &classHint);

	xlib.XSetLocaleModifiers("@im=none");
	window->xInputMethod  = xlib.XOpenIM(x11_ctx.display, NULL, NULL, NULL);
	window->xInputContext = xlib.XCreateIC(window->xInputMethod, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window->window, NULL);
	xlib.XSetICFocus(window->xInputContext);

	window->mode = KORE_WINDOW_MODE_WINDOW;
	kore_x11_window_change_mode(window_index, win->mode);

	xlib.XMapWindow(x11_ctx.display, window->window);

	Atom XdndVersion = 5;
	xlib.XChangeProperty(x11_ctx.display, window->window, x11_ctx.atoms.XdndAware, XA_ATOM, 32, PropModeReplace, (unsigned char *)&XdndVersion, 1);
	xlib.XSetWMProtocols(x11_ctx.display, window->window, &x11_ctx.atoms.WM_DELETE_WINDOW, 1);

	kore_x11_window_set_title(window_index, win->title);

	if (x11_ctx.pen.id != -1) {
		xlib.XSelectExtensionEvent(x11_ctx.display, window->window, &x11_ctx.pen.motionClass, 1);
	}

	if (x11_ctx.eraser.id != -1) {
		xlib.XSelectExtensionEvent(x11_ctx.display, window->window, &x11_ctx.eraser.motionClass, 1);
	}

	x11_ctx.num_windows++;
	return window_index;
}

void kore_x11_window_destroy(int window_index) {
	xlib.XFlush(x11_ctx.display);
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	xlib.XDestroyIC(window->xInputContext);
	xlib.XCloseIM(window->xInputMethod);
	xlib.XDestroyWindow(x11_ctx.display, window->window);
	xlib.XFlush(x11_ctx.display);
	*window = (struct kore_x11_window){0};
	x11_ctx.num_windows--;
}

void kore_x11_window_set_title(int window_index, const char *_title) {
	const char             *title  = _title == NULL ? "" : _title;
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	xlib.XChangeProperty(x11_ctx.display, window->window, x11_ctx.atoms.NET_WM_NAME, x11_ctx.atoms.UTF8_STRING, 8, PropModeReplace, (unsigned char *)title,
	                     strlen(title));

	xlib.XChangeProperty(x11_ctx.display, window->window, x11_ctx.atoms.NET_WM_ICON_NAME, x11_ctx.atoms.UTF8_STRING, 8, PropModeReplace, (unsigned char *)title,
	                     strlen(title));

	xlib.XFlush(x11_ctx.display);
}

int kore_x11_window_x(int window_index) {
	kore_log(KORE_LOG_LEVEL_ERROR, "x11 does not support getting the window position.");
	return 0;
}

int kore_x11_window_y(int window_index) {
	kore_log(KORE_LOG_LEVEL_ERROR, "x11 does not support getting the window position.");
	return 0;
}

void kore_x11_window_move(int window_index, int x, int y) {
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	xlib.XMoveWindow(x11_ctx.display, window->window, x, y);
}

int kore_x11_window_width(int window_index) {
	return x11_ctx.windows[window_index].width;
}

int kore_x11_window_height(int window_index) {
	return x11_ctx.windows[window_index].height;
}

void kore_x11_window_resize(int window_index, int width, int height) {
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	xlib.XResizeWindow(x11_ctx.display, window->window, width, height);
}

void kore_x11_window_show(int window_index) {
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	xlib.XMapWindow(x11_ctx.display, window->window);
}

void kore_x11_window_hide(int window_index) {
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	xlib.XUnmapWindow(x11_ctx.display, window->window);
}

kore_window_mode kore_x11_window_get_mode(int window_index) {
	return x11_ctx.windows[window_index].mode;
}

void kore_x11_window_change_mode(int window_index, kore_window_mode mode) {
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	if (mode == window->mode) {
		return;
	}

	bool fullscreen = false;

	switch (mode) {
	case KORE_WINDOW_MODE_WINDOW:
		if (window->mode == KORE_WINDOW_MODE_FULLSCREEN) {
			window->mode = KORE_WINDOW_MODE_WINDOW;
			fullscreen   = false;
		}
		else {
			return;
		}
		break;
	case KORE_WINDOW_MODE_FULLSCREEN:
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
		if (window->mode == KORE_WINDOW_MODE_WINDOW) {
			window->mode = KORE_WINDOW_MODE_FULLSCREEN;
			fullscreen   = true;
		}
		else {
			return;
		}
		break;
	}

	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.type                 = ClientMessage;
	xev.xclient.window       = window->window;
	xev.xclient.message_type = x11_ctx.atoms.NET_WM_STATE;
	xev.xclient.format       = 32;
	xev.xclient.data.l[0]    = fullscreen ? 1 : 0;
	xev.xclient.data.l[1]    = x11_ctx.atoms.NET_WM_STATE_FULLSCREEN;
	xev.xclient.data.l[2]    = 0;

	xlib.XMapWindow(x11_ctx.display, window->window);

	xlib.XSendEvent(x11_ctx.display, DefaultRootWindow(x11_ctx.display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

	xlib.XFlush(x11_ctx.display);
}

int kore_x11_window_display(int window_index) {
	struct kore_x11_window *window = &x11_ctx.windows[window_index];
	return window->display_index;
}

int kore_x11_count_windows() {
	return x11_ctx.num_windows;
}
