#import "BasicOpenGLView.h"

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>

#include <kore3/backend/HIDManager.h>
#include <kore3/input/keyboard.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/window.h>

#ifdef KORE_METAL
#include <kore3/threads/event.h>
#endif

bool withAutoreleasepool(bool (*f)(void)) {
	@autoreleasepool {
		return f();
	}
}

extern const char *macgetresourcepath(void);

const char *macgetresourcepath(void) {
	return [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSUTF8StringEncoding];
}

@interface KoreApplication : NSApplication {
}
- (void)terminate:(id)sender;
@end

@interface KoreAppDelegate : NSObject <NSWindowDelegate> {
}
- (void)windowWillClose:(NSNotification *)notification;
- (void)windowDidResize:(NSNotification *)notification;
- (void)windowWillMiniaturize:(NSNotification *)notification;
- (void)windowDidDeminiaturize:(NSNotification *)notification;
- (void)windowDidResignMain:(NSNotification *)notification;
- (void)windowDidBecomeMain:(NSNotification *)notification;
@end

static NSApplication     *myapp;
static NSWindow          *window;
static BasicOpenGLView   *view;
static KoreAppDelegate   *delegate;
static struct HIDManager *hidManager;

#ifdef KORE_METAL
static kore_event       displayLinkEvent;
static CVDisplayLinkRef displayLink;
#endif

/*struct KoreWindow : public KoreWindowBase {
    NSWindow* handle;
    BasicOpenGLView* view;

    KoreWindow(NSWindow* handle, BasicOpenGLView* view, int x, int y, int width, int height)
        : KoreWindowBase(x, y, width, height), handle(handle), view(view) {
        ::view = view;
    }
};*/

#ifdef KORE_METAL
CAMetalLayer *getMetalLayer(void) {
	return [view metalLayer];
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *now, const CVTimeStamp *outputTime, CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut, void *displayLinkContext) {

	kore_event_signal(&displayLinkEvent);
	return kCVReturnSuccess;
}
#endif

bool kore_internal_handle_messages(void) {
	NSEvent *event = [myapp nextEventMatchingMask:NSEventMaskAny
	                                    untilDate:[NSDate distantPast]
	                                       inMode:NSDefaultRunLoopMode
	                                      dequeue:YES]; // distantPast: non-blocking
	if (event != nil) {
		[myapp sendEvent:event];
		[myapp updateWindows];
	}

	// Sleep for a frame to limit the calls when the window is not visible.
	// if (!window.visible) {
	// 	[NSThread sleepForTimeInterval:1.0 / 60];
	// }
	return true;
}

void swapBuffersMac(int windowId) {
#ifndef KORE_METAL
	[windows[windowId].view switchBuffers];
#else
	kore_event_wait(&displayLinkEvent);
#endif
}

static int createWindow(kore_window_parameters *parameters) {
	int width     = parameters->width / [[NSScreen mainScreen] backingScaleFactor];
	int height    = parameters->height / [[NSScreen mainScreen] backingScaleFactor];
	int styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
	if ((parameters->window_features & KORE_WINDOW_FEATURE_RESIZEABLE) || (parameters->window_features & KORE_WINDOW_FEATURE_MAXIMIZABLE)) {
		styleMask |= NSWindowStyleMaskResizable;
	}
	if (parameters->window_features & KORE_WINDOW_FEATURE_MINIMIZABLE) {
		styleMask |= NSWindowStyleMaskMiniaturizable;
	}

	view = [[BasicOpenGLView alloc] initWithFrame:NSMakeRect(0, 0, width, height)];
	[view registerForDraggedTypes:[NSArray arrayWithObjects:NSPasteboardTypeURL, nil]];
	window   = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, width, height) styleMask:styleMask backing:NSBackingStoreBuffered defer:TRUE];
	delegate = [KoreAppDelegate alloc];
	[window setDelegate:delegate];
	[window setTitle:[NSString stringWithCString:parameters->title encoding:NSUTF8StringEncoding]];
	[window setAcceptsMouseMovedEvents:YES];
	[[window contentView] addSubview:view];
	[window center];

	windows[windowCounter].handle = window;
	windows[windowCounter].view   = view;

	[window makeKeyAndOrderFront:nil];

	if (parameters->mode == KORE_WINDOW_MODE_FULLSCREEN || parameters->mode == KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
		[window toggleFullScreen:nil];
		windows[windowCounter].fullscreen = true;
	}

	return windowCounter++;
}

int kore_count_windows(void) {
	return windowCounter;
}

void kore_window_change_window_mode(int window_index, kore_window_mode mode) {
	switch (mode) {
	case KORE_WINDOW_MODE_WINDOW:
		if (windows[window_index].fullscreen) {
			[window toggleFullScreen:nil];
			windows[window_index].fullscreen = false;
		}
		break;
	case KORE_WINDOW_MODE_FULLSCREEN:
	case KORE_WINDOW_MODE_EXCLUSIVE_FULLSCREEN:
		if (!windows[window_index].fullscreen) {
			[window toggleFullScreen:nil];
			windows[window_index].fullscreen = true;
		}
		break;
	}
}

void kore_window_set_close_callback(int window, bool (*callback)(void *), void *data) {
	windows[window].closeCallback     = callback;
	windows[window].closeCallbackData = data;
}

static void addMenubar(void) {
	NSString *appName = [[NSProcessInfo processInfo] processName];

	NSMenu     *appMenu      = [NSMenu new];
	NSString   *quitTitle    = [@"Quit " stringByAppendingString:appName];
	NSMenuItem *quitMenuItem = [[NSMenuItem alloc] initWithTitle:quitTitle action:@selector(terminate:) keyEquivalent:@"q"];
	[appMenu addItem:quitMenuItem];

	NSMenuItem *appMenuItem = [NSMenuItem new];
	[appMenuItem setSubmenu:appMenu];

	NSMenu *menubar = [NSMenu new];
	[menubar addItem:appMenuItem];
	[NSApp setMainMenu:menubar];
}

int kore_init(const char *name, int width, int height, kore_window_parameters *win, kore_framebuffer_parameters *frame) {
	@autoreleasepool {
		myapp = [KoreApplication sharedApplication];
		[myapp finishLaunching];
		[[NSRunningApplication currentApplication] activateWithOptions:(NSApplicationActivateAllWindows | NSApplicationActivateIgnoringOtherApps)];
		NSApp.activationPolicy = NSApplicationActivationPolicyRegular;

		hidManager = (struct HIDManager *)malloc(sizeof(struct HIDManager));
		HIDManager_init(hidManager);
		addMenubar();
	}

	// System::_init(name, width, height, &win, &frame);
	kore_window_parameters defaultWindowOptions;
	if (win == NULL) {
		kore_window_options_set_defaults(&defaultWindowOptions);
		win = &defaultWindowOptions;
	}

	kore_framebuffer_parameters defaultFramebufferOptions;
	if (frame == NULL) {
		kore_framebuffer_options_set_defaults(&defaultFramebufferOptions);
		frame = &defaultFramebufferOptions;
	}

	win->width  = width;
	win->height = height;
	if (win->title == NULL) {
		win->title = name;
	}

	/*int windowId = */ createWindow(win);
	// kore_g4_internal_init(); // TODO
	// kore_g4_internal_init_window(windowId, frame->depth_bits, frame->stencil_bits, true); // TODO

#ifdef KORE_METAL
	kore_event_init(&displayLinkEvent, false);
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, NULL);
	CVDisplayLinkStart(displayLink);
#endif

	return 0;
}

int kore_window_width(int window_index) {
	NSWindow *window = windows[window_index].handle;
	float     scale  = [window backingScaleFactor];
	return [[window contentView] frame].size.width * scale;
}

int kore_window_height(int window_index) {
	NSWindow *window = windows[window_index].handle;
	float     scale  = [window backingScaleFactor];
	return [[window contentView] frame].size.height * scale;
}

NSWindow *kore_get_mac_window_handle(int window_index) {
	return windows[window_index].handle;
}

void kore_load_url(const char *url) {
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url]]];
}

static char language[3];

const char *kore_language(void) {
	NSString   *nsstr = [[NSLocale preferredLanguages] objectAtIndex:0];
	const char *lang  = [nsstr UTF8String];
	language[0]       = lang[0];
	language[1]       = lang[1];
	language[2]       = 0;
	return language;
}

void kore_internal_shutdown(void) {
#ifdef KORE_METAL
	if (displayLink) {
		CVDisplayLinkStop(displayLink);
		CVDisplayLinkRelease(displayLink);
		displayLink = NULL;
	}
	kore_event_destroy(&displayLinkEvent);
#endif
}

static const char *getSavePath(void) {
	NSArray  *paths        = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString *resolvedPath = [paths objectAtIndex:0];
	NSString *appName      = [NSString stringWithUTF8String:kore_application_name()];
	resolvedPath           = [resolvedPath stringByAppendingPathComponent:appName];

	NSFileManager *fileMgr = [[NSFileManager alloc] init];

	NSError *error;
	[fileMgr createDirectoryAtPath:resolvedPath withIntermediateDirectories:YES attributes:nil error:&error];

	resolvedPath = [resolvedPath stringByAppendingString:@"/"];
	return [resolvedPath cStringUsingEncoding:NSUTF8StringEncoding];
}

const char *kore_internal_save_path(void) {
	return getSavePath();
}

#ifndef KORE_NO_MAIN
int main(int argc, char **argv) {
	return kickstart(argc, argv);
}
#endif

@implementation KoreApplication

- (void)terminate:(id)sender {
	kore_stop();
}

@end

@implementation KoreAppDelegate
- (BOOL)windowShouldClose:(NSWindow *)sender {
	if (windows[0].closeCallback != NULL) {
		if (windows[0].closeCallback(windows[0].closeCallbackData)) {
			return YES;
		}
		else {
			return NO;
		}
	}
	return YES;
}

- (void)windowWillClose:(NSNotification *)notification {
	kore_stop();
}

- (void)windowDidResize:(NSNotification *)notification {
	NSWindow *window = [notification object];
	NSSize    size   = [[window contentView] frame].size;
	[view resize:size];
	if (windows[0].resizeCallback != NULL) {
		windows[0].resizeCallback(size.width, size.height, windows[0].resizeCallbackData);
	}
}

- (void)windowWillMiniaturize:(NSNotification *)notification {
	kore_internal_background_callback();
}

- (void)windowDidDeminiaturize:(NSNotification *)notification {
	kore_internal_foreground_callback();
}

- (void)windowDidResignMain:(NSNotification *)notification {
	kore_internal_pause_callback();
}

- (void)windowDidBecomeMain:(NSNotification *)notification {
	kore_internal_resume_callback();
}

@end
