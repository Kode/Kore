#include <kore3/backend/wayland.h>

#include <kore3/image.h>
#include <kore3/log.h>
#include <kore3/window.h>

// for all that shared memory stuff later on
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef KORE_EGL
#define EGL_NO_PLATFORM_SPECIFIC_TYPES
#include <EGL/egl.h>
#endif

static void xdg_surface_handle_configure(void *data, struct xdg_surface *surface, uint32_t serial) {
	xdg_surface_ack_configure(surface, serial);
	struct kore_wl_window *window = data;
	window->configured            = true;
}

void kore_internal_resize(int, int, int);
void kore_wayland_destroy_decoration(struct kore_wl_decoration *);
void kore_wayland_resize_decoration(struct kore_wl_decoration *, int x, int y, int width, int height);

static void xdg_toplevel_handle_configure(void *data, struct xdg_toplevel *toplevel, int32_t width, int32_t height, struct wl_array *states) {
	struct kore_wl_window *window = data;
	if (width > 0 && height > 0) {
		if (window->decorations.server_side) {
			window->surface_width  = width;
			window->surface_height = height;
		}
		else {
			window->surface_width  = width - (KORE_WL_DECORATION_WIDTH * 2);
			window->surface_height = height - KORE_WL_DECORATION_TOP_HEIGHT + KORE_WL_DECORATION_BOTTOM_HEIGHT;
		}
		window->buffer_width  = window->surface_width * window->preferred_scale / 120;
		window->buffer_height = window->surface_height * window->preferred_scale / 120;
	}

	enum xdg_toplevel_state *state;
	wl_array_for_each(state, states) {
		switch (*state) {
		case XDG_TOPLEVEL_STATE_ACTIVATED:
			kore_internal_foreground_callback();
			break;
		case XDG_TOPLEVEL_STATE_RESIZING:
			break;
		case XDG_TOPLEVEL_STATE_MAXIMIZED:
			break;
		default:
			break;
		}
	}

	if (window->surface_width > 0 && window->surface_height > 0) {
		if (window->decorations.server_side) {
			xdg_surface_set_window_geometry(window->xdg_surface, 0, 0, window->surface_width, window->surface_height);
		}
		else {
			xdg_surface_set_window_geometry(window->xdg_surface, KORE_WL_DECORATION_LEFT_X, KORE_WL_DECORATION_TOP_Y,
			                                window->surface_width + (KORE_WL_DECORATION_WIDTH * 2),
			                                window->surface_height + KORE_WL_DECORATION_TOP_HEIGHT + KORE_WL_DECORATION_BOTTOM_HEIGHT);
		}
		if (window->viewport) {
			wp_viewport_set_source(window->viewport, 0, 0, wl_fixed_from_int(window->buffer_width), wl_fixed_from_int(window->buffer_height));
			wp_viewport_set_destination(window->viewport, window->surface_width, window->surface_height);
		}
		else {
			wl_surface_set_buffer_scale(window->surface, window->preferred_scale / 120);
		}

		// kore_internal_resize(window->window_id, window->buffer_width, window->buffer_height); // TODO
		kore_internal_call_resize_callback(window->window_id, window->buffer_width, window->buffer_height);
#ifdef KORE_EGL
		wl_egl_window_resize(window->egl_window, window->buffer_width, window->buffer_height, 0, 0);
#endif

		kore_wayland_resize_decoration(&window->decorations.top, KORE_WL_DECORATION_TOP_X, KORE_WL_DECORATION_TOP_Y, KORE_WL_DECORATION_TOP_WIDTH,
		                               KORE_WL_DECORATION_TOP_HEIGHT);
		kore_wayland_resize_decoration(&window->decorations.left, KORE_WL_DECORATION_LEFT_X, KORE_WL_DECORATION_LEFT_Y, KORE_WL_DECORATION_LEFT_WIDTH,
		                               KORE_WL_DECORATION_LEFT_HEIGHT);
		kore_wayland_resize_decoration(&window->decorations.right, KORE_WL_DECORATION_RIGHT_X, KORE_WL_DECORATION_RIGHT_Y, KORE_WL_DECORATION_RIGHT_WIDTH,
		                               KORE_WL_DECORATION_RIGHT_HEIGHT);
		kore_wayland_resize_decoration(&window->decorations.bottom, KORE_WL_DECORATION_BOTTOM_X, KORE_WL_DECORATION_BOTTOM_Y, KORE_WL_DECORATION_BOTTOM_WIDTH,
		                               KORE_WL_DECORATION_BOTTOM_HEIGHT);
		kore_wayland_resize_decoration(&window->decorations.close, KORE_WL_DECORATION_CLOSE_X, KORE_WL_DECORATION_CLOSE_Y, KORE_WL_DECORATION_CLOSE_WIDTH,
		                               KORE_WL_DECORATION_CLOSE_HEIGHT);
	}
}

void kore_wayland_window_destroy(int window_index);

static void xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel) {
	struct kore_wl_window *window = data;
	if (kore_internal_call_close_callback(window->window_id)) {
		kore_window_destroy(window->window_id);
		if (wl_ctx.num_windows <= 0) {
			// no windows left, stop
			kore_stop();
		}
	}
}

static int create_shm_fd(off_t size) {
	int fd = -1;
#if defined(__linux__)
#if defined(__GLIBC__) && !__GLIBC_PREREQ(2, 27)
#else
	// memfd_create is available since glibc 2.27 and musl 1.1.20
	// the syscall is available since linux 3.17
	// at the time of writing (04/02/2022) these requirements are fullfilled for the "LTS" versions of the following distributions
	// Ubuntu 18.04
	// Debian Stretch
	// Alpine 3.12
	// Fedora 34

	fd = memfd_create("kore-wayland-shm", MFD_CLOEXEC | MFD_ALLOW_SEALING);
	if (fd >= 0) {
		fcntl(fd, F_ADD_SEALS, F_SEAL_SHRINK | F_SEAL_SEAL);
		int ret = posix_fallocate(fd, 0, size);
		if (ret != 0) {
			close(fd);
			errno = ret;
			return -1;
		}
	}
	else // fall back to a temp file
#endif
#endif
	{

		static const char template[] = "/kore-shared-XXXXXX";

		const char *path = getenv("XDG_RUNTIME_DIR");
		if (!path) {
			errno = ENOENT;
			return -1;
		}

		char *name = malloc(strlen(path) + sizeof(template));
		strcpy(name, path);
		strcat(name, template);

		fd = mkostemp(name, O_CLOEXEC);
		if (fd >= 0)
			unlink(name);

		free(name);
		if (fd < 0)
			return -1;

		int ret = ftruncate(fd, size);
		if (ret != 0) {
			close(fd);
			errno = ret;
			return -1;
		}
	}

	return fd;
}

struct wl_buffer *kore_wayland_create_shm_buffer(const kore_image *image) {
	int stride = image->width * 4;
	int length = image->width * image->height * 4;

	const int fd = create_shm_fd(length);
	if (fd < 0) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Wayland: Creating a buffer file for %d B failed: %s", length, strerror(errno));
		return NULL;
	}

	void *data = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Wayland: mmap failed: %s", strerror(errno));
		close(fd);
		return NULL;
	}

	struct wl_shm_pool *pool = wl_shm_create_pool(wl_ctx.shm, fd, length);

	close(fd);
	memcpy(data, image->data, image->width * image->height * 4);

	struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, image->width, image->height, stride, WL_SHM_FORMAT_ARGB8888);
	munmap(data, length);
	wl_shm_pool_destroy(pool);

	return buffer;
}

static int grey_data[] = {0xFF333333};

static int close_data[] = {
    0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000,
    0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF,
};

// image format is argb32, but Kore does not have that, so let's lie to it

static kore_image grey_image = {
    1, 1, 0, KORE_IMAGE_FORMAT_RGBA32, 0, KORE_IMAGE_COMPRESSION_NONE, grey_data, sizeof(grey_data),
};
static kore_image close_image = {
    9, 9, 0, KORE_IMAGE_FORMAT_RGBA32, 0, KORE_IMAGE_COMPRESSION_NONE, close_data, sizeof(close_data),
};

void kore_wayland_create_decoration(struct kore_wl_decoration *decoration, struct wl_surface *parent, struct wl_buffer *buffer, bool opaque, int x, int y,
                                    int width, int height) {
	decoration->surface    = wl_compositor_create_surface(wl_ctx.compositor);
	decoration->subsurface = wl_subcompositor_get_subsurface(wl_ctx.subcompositor, decoration->surface, parent);
	wl_subsurface_set_position(decoration->subsurface, x, y);
	decoration->viewport = wp_viewporter_get_viewport(wl_ctx.viewporter, decoration->surface);
	wp_viewport_set_destination(decoration->viewport, width, height);
	if (buffer)
		wl_surface_attach(decoration->surface, buffer, 0, 0);

	if (opaque) {
		struct wl_region *region = wl_compositor_create_region(wl_ctx.compositor);
		wl_region_add(region, 0, 0, width, height);
		wl_surface_set_opaque_region(decoration->surface, region);
		wl_surface_commit(decoration->surface);
		wl_region_destroy(region);
	}
	else
		wl_surface_commit(decoration->surface);
}

void kore_wayland_resize_decoration(struct kore_wl_decoration *decoration, int x, int y, int width, int height) {
	if (decoration->surface) {
		wl_subsurface_set_position(decoration->subsurface, x, y);
		wp_viewport_set_destination(decoration->viewport, width, height);
		wl_surface_commit(decoration->surface);
	}
}

void kore_wayland_destroy_decoration(struct kore_wl_decoration *decoration) {
	if (decoration->subsurface)
		wl_subsurface_destroy(decoration->subsurface);
	if (decoration->surface)
		wl_surface_destroy(decoration->surface);
	if (decoration->viewport)
		wp_viewport_destroy(decoration->viewport);
	decoration->surface    = NULL;
	decoration->subsurface = NULL;
	decoration->viewport   = NULL;
}

void kore_wayland_destroy_decorations(struct kore_wl_window *window) {
	kore_wayland_destroy_decoration(&window->decorations.top);
	kore_wayland_destroy_decoration(&window->decorations.left);
	kore_wayland_destroy_decoration(&window->decorations.right);
	kore_wayland_destroy_decoration(&window->decorations.bottom);
	kore_wayland_destroy_decoration(&window->decorations.close);
}

void kore_wayland_create_decorations(struct kore_wl_window *window) {
	if (!window->decorations.dec_buffer) {
		window->decorations.dec_buffer   = kore_wayland_create_shm_buffer(&grey_image);
		window->decorations.close_buffer = kore_wayland_create_shm_buffer(&close_image);
		window->decorations.max_buffer   = kore_wayland_create_shm_buffer(&grey_image);
		window->decorations.min_buffer   = kore_wayland_create_shm_buffer(&grey_image);
	}
	kore_wayland_create_decoration(&window->decorations.top, window->surface, window->decorations.dec_buffer, true, KORE_WL_DECORATION_TOP_X,
	                               KORE_WL_DECORATION_TOP_Y, KORE_WL_DECORATION_TOP_WIDTH, KORE_WL_DECORATION_TOP_HEIGHT);
	kore_wayland_create_decoration(&window->decorations.left, window->surface, window->decorations.dec_buffer, true, KORE_WL_DECORATION_LEFT_X,
	                               KORE_WL_DECORATION_LEFT_Y, KORE_WL_DECORATION_LEFT_WIDTH, KORE_WL_DECORATION_LEFT_HEIGHT);
	kore_wayland_create_decoration(&window->decorations.right, window->surface, window->decorations.dec_buffer, true, KORE_WL_DECORATION_RIGHT_X,
	                               KORE_WL_DECORATION_RIGHT_Y, KORE_WL_DECORATION_RIGHT_WIDTH, KORE_WL_DECORATION_RIGHT_HEIGHT);
	kore_wayland_create_decoration(&window->decorations.bottom, window->surface, window->decorations.dec_buffer, true, KORE_WL_DECORATION_BOTTOM_X,
	                               KORE_WL_DECORATION_BOTTOM_Y, KORE_WL_DECORATION_BOTTOM_WIDTH, KORE_WL_DECORATION_BOTTOM_HEIGHT);
	kore_wayland_create_decoration(&window->decorations.close, window->surface, window->decorations.close_buffer, true, KORE_WL_DECORATION_CLOSE_X,
	                               KORE_WL_DECORATION_CLOSE_Y, KORE_WL_DECORATION_CLOSE_WIDTH, KORE_WL_DECORATION_CLOSE_HEIGHT);
}

void xdg_toplevel_decoration_configure(void *data, struct zxdg_toplevel_decoration_v1 *zxdg_toplevel_decoration_v1, uint32_t mode) {
	struct kore_wl_window *window = data;

	if (mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE) {
		window->decorations.server_side = false;
		if (window->decorations.top.surface) {
			kore_wayland_destroy_decorations(window);
		}
		if (window->mode == KORE_WINDOW_MODE_WINDOW) {
			kore_wayland_create_decorations(window);
		}
	}
	else if (mode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE) {
		window->decorations.server_side = true;
		if (window->decorations.top.surface) {
			kore_wayland_destroy_decorations(window);
		}
	}
}

void wl_surface_handle_enter(void *data, struct wl_surface *wl_surface, struct wl_output *output) {
	struct kore_wl_window  *window  = wl_surface_get_user_data(wl_surface);
	struct kore_wl_display *display = wl_output_get_user_data(output);

	if (display && window) {
		window->display_index = display->index;
	}
}

void wl_surface_handle_leave(void *data, struct wl_surface *wl_surface, struct wl_output *output) {}

static void wl_surface_handle_preferred_buffer_scale(void *data, struct wl_surface *wl_surface, int32_t scale) {
	struct kore_wl_window *window = wl_surface_get_user_data(wl_surface);
	if (!wl_ctx.fractional_scale_manager) {
		window->preferred_scale = scale * 120;
	}
}
static void wl_surface_handle_preferred_buffer_transform(void *data, struct wl_surface *wl_surface, uint32_t transform) {}

static const struct wl_surface_listener wl_surface_listener = {
    wl_surface_handle_enter,
    wl_surface_handle_leave,
#ifdef WL_SURFACE_PREFERRED_BUFFER_SCALE_SINCE_VERSION
    wl_surface_handle_preferred_buffer_scale,
#endif
#ifdef WL_SURFACE_PREFERRED_BUFFER_TRANSFORM_SINCE_VERSION
    wl_surface_handle_preferred_buffer_transform,
#endif
};

static const struct xdg_surface_listener xdg_surface_listener = {
    xdg_surface_handle_configure,
};

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    xdg_toplevel_handle_configure,
    xdg_toplevel_handle_close,
};

static const struct zxdg_toplevel_decoration_v1_listener xdg_toplevel_decoration_listener = {
    xdg_toplevel_decoration_configure,
};

static void wp_fractional_scale_v1_handle_preferred_scale(void *data, struct wp_fractional_scale_v1 *fractional_scale, uint32_t scale) {
	struct kore_wl_window *window = data;
	window->preferred_scale       = scale;
}

static const struct wp_fractional_scale_v1_listener wp_fractional_scale_v1_listener = {
    wp_fractional_scale_v1_handle_preferred_scale,
};

void kore_wayland_window_set_title(int window_index, const char *title);
void kore_wayland_window_change_mode(int window_index, kore_window_mode mode);

#ifndef KORE_WAYLAND_APP_ID
#define KORE_WAYLAND_APP_ID "_KoreApplication"
#endif

int kore_wayland_window_create(kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	int window_index = -1;
	for (int i = 0; i < MAXIMUM_WINDOWS; i++) {
		if (wl_ctx.windows[i].surface == NULL) {
			window_index = i;
			break;
		}
	}
	if (window_index == -1) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Too many windows (maximum is %i)", MAXIMUM_WINDOWS);
		exit(1);
	}
	struct kore_wl_window *window = &wl_ctx.windows[window_index];
	window->window_id             = window_index;
	window->mode                  = -1;
	window->preferred_scale       = 120;
	window->surface               = wl_compositor_create_surface(wl_ctx.compositor);
	wl_surface_set_user_data(window->surface, window);
	wl_surface_add_listener(window->surface, &wl_surface_listener, NULL);

	if (wl_ctx.viewporter) {
		window->viewport = wp_viewporter_get_viewport(wl_ctx.viewporter, window->surface);
	}

	if (wl_ctx.fractional_scale_manager) {
		if (!window->viewport) {
			kore_log(KORE_LOG_LEVEL_ERROR, "missing wp_viewport wayland extension");
			exit(1);
		}
		window->fractional_scale = wp_fractional_scale_manager_v1_get_fractional_scale(wl_ctx.fractional_scale_manager, window->surface);
		wp_fractional_scale_v1_add_listener(window->fractional_scale, &wp_fractional_scale_v1_listener, window);
	}

	window->xdg_surface = xdg_wm_base_get_xdg_surface(wl_ctx.xdg_wm_base, window->surface);
	xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener, window);

	window->toplevel = xdg_surface_get_toplevel(window->xdg_surface);
	xdg_toplevel_add_listener(window->toplevel, &xdg_toplevel_listener, window);

	if (wl_ctx.decoration_manager) {
		window->xdg_decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(wl_ctx.decoration_manager, window->toplevel);
#ifdef KORE_WAYLAND_FORCE_CSD
		zxdg_toplevel_decoration_v1_set_mode(window->xdg_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
#endif
		zxdg_toplevel_decoration_v1_add_listener(window->xdg_decoration, &xdg_toplevel_decoration_listener, window);
	}
	else {
		window->decorations.server_side = false;
	}

	xdg_toplevel_set_app_id(window->toplevel, KORE_WAYLAND_APP_ID);

	wl_surface_commit(window->surface);
	wl_ctx.num_windows++;

	while (!window->configured) {
		wl_display_roundtrip(wl_ctx.display);
	}

	kore_wayland_window_set_title(window_index, win->title);
	kore_wayland_window_change_mode(window_index, win->mode);
	window->surface_width  = (win->width * 120) / window->preferred_scale;
	window->surface_height = (win->height * 120) / window->preferred_scale;
	window->buffer_width   = win->width;
	window->buffer_height  = win->height;
	if (window->viewport) {
		wp_viewport_set_source(window->viewport, 0, 0, wl_fixed_from_int(window->buffer_width), wl_fixed_from_int(window->buffer_height));
		wp_viewport_set_destination(window->viewport, window->surface_width, window->surface_height);
	}
	else {
		wl_surface_set_buffer_scale(window->surface, window->preferred_scale / 120);
	}
	xdg_surface_set_window_geometry(window->xdg_surface, 0, 0, window->surface_width, window->surface_height);
	if (!window->decorations.server_side) {
		kore_wayland_create_decorations(window);
	}

#ifdef KORE_EGL
	window->egl_window = wl_egl_window_create(window->surface, window->buffer_width, window->buffer_height);
#endif
	wl_surface_commit(window->surface);
	return window_index;
}

void kore_wayland_window_destroy(int window_index) {
	struct kore_wl_window *window = &wl_ctx.windows[window_index];
#ifdef KORE_EGL
	wl_egl_window_destroy(window->egl_window);
#endif
	if (window->xdg_decoration) {
		zxdg_toplevel_decoration_v1_destroy(window->xdg_decoration);
	}

	xdg_toplevel_destroy(window->toplevel);
	xdg_surface_destroy(window->xdg_surface);
	wl_surface_destroy(window->surface);
	*window = (struct kore_wl_window){0};
	wl_ctx.num_windows--;
}

void kore_wayland_window_set_title(int window_index, const char *title) {
	struct kore_wl_window *window = &wl_ctx.windows[window_index];
	xdg_toplevel_set_title(window->toplevel, title == NULL ? "" : title);
}

int kore_wayland_window_x(int window_index) {
	kore_log(KORE_LOG_LEVEL_ERROR, "Wayland does not support getting the window position.");
	return 0;
}

int kore_wayland_window_y(int window_index) {
	kore_log(KORE_LOG_LEVEL_ERROR, "Wayland does not support getting the window position.");
	return 0;
}

void kore_wayland_window_move(int window_index, int x, int y) {
	kore_log(KORE_LOG_LEVEL_ERROR, "Wayland does not support setting the window position.");
}

int kore_wayland_window_width(int window_index) {
	return wl_ctx.windows[window_index].buffer_width;
}

int kore_wayland_window_height(int window_index) {
	return wl_ctx.windows[window_index].buffer_height;
}

void kore_wayland_window_resize(int window_index, int width, int height) {
	kore_log(KORE_LOG_LEVEL_WARNING, "TODO: resizing windows");
}

void kore_wayland_window_show(int window_index) {
	kore_log(KORE_LOG_LEVEL_ERROR, "Wayland does not support unhiding windows.");
}

void kore_wayland_window_hide(int window_index) {
	kore_log(KORE_LOG_LEVEL_ERROR, "Wayland does not support hiding windows.");
}

kore_window_mode kore_wayland_window_get_mode(int window_index) {
	return wl_ctx.windows[window_index].mode;
}

void kore_wayland_window_change_mode(int window_index, kore_window_mode mode) {
	struct kore_wl_window *window = &wl_ctx.windows[window_index];
	if (mode == window->mode) {
		return;
	}
	switch (mode) {
	case KORE_WINDOW_MODE_WINDOW:
		if (window->mode == KORE_WINDOW_MODE_FULLSCREEN || window->mode == KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
			window->mode = KORE_WINDOW_MODE_WINDOW;
			xdg_toplevel_unset_fullscreen(window->toplevel);
		}
		break;
	case KORE_WINDOW_MODE_FULLSCREEN:
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
		if (window->mode == KORE_WINDOW_MODE_WINDOW) {
			window->mode                    = mode;
			struct kore_wl_display *display = &wl_ctx.displays[window->display_index];
			xdg_toplevel_set_fullscreen(window->toplevel, display->output);
		}
		break;
	}
}

int kore_wayland_window_display(int window_index) {
	struct kore_wl_window *window = &wl_ctx.windows[window_index];
	return window->display_index;
}

int kore_wayland_count_windows() {
	return wl_ctx.num_windows;
}
