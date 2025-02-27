#import <Cocoa/Cocoa.h>

#include <kore3/backend/windowdata.h>
#include <kore3/input/mouse.h>
#include <kore3/window.h>

NSWindow *kore_get_mac_window_handle(int window_index);

void kore_internal_mouse_lock(int window) {
	kore_mouse_hide();
}

void kore_internal_mouse_unlock(void) {
	kore_mouse_show();
}

bool kore_mouse_can_lock(void) {
	return true;
}

void kore_mouse_show(void) {
	CGDisplayShowCursor(kCGDirectMainDisplay);
}

void kore_mouse_hide(void) {
	CGDisplayHideCursor(kCGDirectMainDisplay);
}

void kore_mouse_set_position(int windowId, int x, int y) {

	NSWindow *window = kore_get_mac_window_handle(windowId);
	float scale = [window backingScaleFactor];
	NSRect rect = [[NSScreen mainScreen] frame];

	CGPoint point;
	point.x = window.frame.origin.x + (x / scale);
	point.y = rect.size.height - (window.frame.origin.y + (y / scale));

	CGDisplayMoveCursorToPoint(windowId, point);
	CGAssociateMouseAndMouseCursorPosition(true);
}

void kore_mouse_get_position(int windowId, int *x, int *y) {

	NSWindow *window = kore_get_mac_window_handle(windowId);
	NSPoint point = [window mouseLocationOutsideOfEventStream];
	*x = (int)point.x;
	*y = (int)point.y;
}

void kore_mouse_set_cursor(int cursor_index) {}
