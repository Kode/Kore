#include <kore3/window.h>

#include <kore3/display.h>

#include <stdlib.h>

void kore_window_options_set_defaults(kore_window_parameters *win) {
	kore_display_init();
	win->title           = NULL;
	win->display_index   = kore_primary_display();
	win->mode            = KORE_WINDOW_MODE_WINDOW;
	win->x               = -1;
	win->y               = -1;
	win->width           = 800;
	win->height          = 600;
	win->visible         = true;
	win->window_features = KORE_WINDOW_FEATURE_RESIZEABLE | KORE_WINDOW_FEATURE_MINIMIZABLE | KORE_WINDOW_FEATURE_MAXIMIZABLE;
}

void kore_framebuffer_options_set_defaults(kore_framebuffer_parameters *frame) {
	frame->frequency         = 60;
	frame->vertical_sync     = true;
	frame->color_bits        = 32;
	frame->depth_bits        = 16;
	frame->stencil_bits      = 8;
	frame->samples_per_pixel = 1;
}
