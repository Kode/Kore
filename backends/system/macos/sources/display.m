#import <Cocoa/Cocoa.h>

#include <kore3/display.h>
#include <kore3/log.h>

#define maxDisplays 10
// Display displays[maxDisplays];

/*void initMacDisplays() {
    for (int i = 0; i < maxDisplays; ++i) {
        displays[i]._data.index = i;
    }
}*/

int kore_count_displays(void) {
	NSArray *screens = [NSScreen screens];
	return (int)[screens count];
}

int kore_primary_display(void) {
	NSArray  *screens    = [NSScreen screens];
	NSScreen *mainScreen = [NSScreen mainScreen];
	for (int i = 0; i < maxDisplays; ++i) {
		if (mainScreen == screens[i]) {
			return i;
		}
	}
	return -1;
}

void kore_display_init(void) {}

kore_display_mode kore_display_available_mode(int display, int mode) {
	kore_display_mode dm;
	dm.width          = 800;
	dm.height         = 600;
	dm.frequency      = 60;
	dm.bits_per_pixel = 32;
	return dm;
}

int kore_display_count_available_modes(int display) {
	return 1;
}

bool kore_display_available(int display) {
	return true;
}

const char *kore_display_name(int display) {
	return "Display";
}

kore_display_mode kore_display_current_mode(int display) {
	NSArray          *screens    = [NSScreen screens];
	NSScreen         *screen     = screens[display];
	NSRect            screenRect = [screen frame];
	kore_display_mode dm;
	dm.width          = screenRect.size.width;
	dm.height         = screenRect.size.height;
	dm.frequency      = 60;
	dm.bits_per_pixel = 32;

	NSDictionary *description         = [screen deviceDescription];
	NSSize        displayPixelSize    = [[description objectForKey:NSDeviceSize] sizeValue];
	NSNumber     *screenNumber        = [description objectForKey:@"NSScreenNumber"];
	CGSize        displayPhysicalSize = CGDisplayScreenSize([screenNumber unsignedIntValue]);            // in millimeters
	double        ppi                 = displayPixelSize.width / (displayPhysicalSize.width * 0.039370); // Convert MM to INCH
	dm.pixels_per_inch                = round(ppi);

	return dm;
}

//**
/*
int Display::x() {
    NSArray* screens = [NSScreen screens];
    NSScreen* screen = screens[_data.index];
    NSRect rect = [screen frame];
    return rect.origin.x;
}

int Display::y() {
    NSArray* screens = [NSScreen screens];
    NSScreen* screen = screens[_data.index];
    NSRect rect = [screen frame];
    return rect.origin.y;
}

int Display::width() {
    NSArray* screenArray = [NSScreen screens];
    NSScreen* screen = [screenArray objectAtIndex:_data.index];
    NSRect screenRect = [screen visibleFrame];
    return screenRect.size.width;
}

int Display::height() {
    NSArray* screenArray = [NSScreen screens];
    // unsigned screenCount = [screenArray count];
    NSScreen* screen = [screenArray objectAtIndex:_data.index];
    NSRect screenRect = [screen visibleFrame];
    return screenRect.size.height;
}

int Display::frequency() {
    return 60;
}

int Display::pixelsPerInch() {
    return 96;
}
*/
