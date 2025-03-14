#import "KoreAppDelegate.h"

#include <kore3/input/gamepad.h>
#include <kore3/input/keyboard.h>
#include <kore3/system.h>
#include <kore3/video.h>
#include <kore3/window.h>

#import <AudioToolbox/AudioToolbox.h>
#import <UIKit/UIKit.h>

bool withAutoreleasepool(bool (*f)(void)) {
	@autoreleasepool {
		return f();
	}
}

static bool keyboardshown = false;

const char *iphonegetresourcepath(void) {
	return [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:1];
}

bool kore_internal_handle_messages(void) {
	SInt32 result;
	do {
		result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, TRUE);
	} while (result == kCFRunLoopRunHandledSource);
	return true;
}

void kore_set_keep_screen_on(bool on) {}

void showKeyboard(void);
void hideKeyboard(void);

void kore_keyboard_show(void) {
	keyboardshown = true;
	showKeyboard();
}

void kore_keyboard_hide(void) {
	keyboardshown = false;
	hideKeyboard();
}

bool kore_keyboard_active(void) {
	return keyboardshown;
}

void loadURL(const char *url);

void kore_load_url(const char *url) {
	loadURL(url);
}

// On iOS you can't set the length of the vibration.
void kore_vibrate(int ms) {
	AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
};

static char language[3];

const char *kore_language(void) {
	NSString *nsstr  = [[NSLocale preferredLanguages] objectAtIndex:0];
	const char *lang = [nsstr UTF8String];
	language[0]      = lang[0];
	language[1]      = lang[1];
	language[2]      = 0;
	return language;
}

// called on rotation event
void KoreUpdateKeyboard(void) {
	if (keyboardshown) {
		hideKeyboard();
		showKeyboard();
	}
	else {
		hideKeyboard();
	}
}

void kore_internal_shutdown(void) {}

int kore_init(const char *name, int width, int height, struct kore_window_parameters *win, struct kore_framebuffer_parameters *frame) {
	kore_window_parameters defaultWin;
	if (win == NULL) {
		kore_window_options_set_defaults(&defaultWin);
		win = &defaultWin;
	}
	kore_framebuffer_parameters defaultFrame;
	if (frame == NULL) {
		kore_framebuffer_options_set_defaults(&defaultFrame);
		frame = &defaultFrame;
	}
	// kore_g4_internal_init();
	// kore_g4_internal_init_window(0, frame->depth_bits, frame->stencil_bits, true);

	return 0;
}

void endGL(void);

void swapBuffersiOS(void) {
	endGL();
}

static char sysid[512];

const char *kore_system_id(void) {
	const char *name     = [[[UIDevice currentDevice] name] UTF8String];
	const char *vendorId = [[[[UIDevice currentDevice] identifierForVendor] UUIDString] UTF8String];
	strcpy(sysid, name);
	strcat(sysid, "-");
	strcat(sysid, vendorId);
	return sysid;
}

static const char *getSavePath(void) {
	NSArray *paths         = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
	NSString *resolvedPath = [paths objectAtIndex:0];
	NSString *appName      = [NSString stringWithUTF8String:kore_application_name()];
	resolvedPath           = [resolvedPath stringByAppendingPathComponent:appName];

	NSFileManager *fileMgr = [[NSFileManager alloc] init];

	NSError *error;
	[fileMgr createDirectoryAtPath:resolvedPath withIntermediateDirectories:YES attributes:nil error:&error];

	resolvedPath = [resolvedPath stringByAppendingString:@"/"];
	return [resolvedPath cStringUsingEncoding:1];
}

const char *kore_internal_save_path(void) {
	return getSavePath();
}

static const char *videoFormats[] = {"mp4", NULL};

const char **kore_video_formats(void) {
	return videoFormats;
}

#include <mach/mach_time.h>

double kore_frequency(void) {
	mach_timebase_info_data_t info;
	mach_timebase_info(&info);
	return (double)info.denom / (double)info.numer / 1e-9;
}

kore_ticks kore_timestamp(void) {
	kore_ticks time = mach_absolute_time();
	return time;
}

void kore_login(void) {}

void kore_unlock_achievement(int id) {}

const char *kore_gamepad_vendor(int gamepad) {
	return "nobody";
}

const char *kore_gamepad_product_name(int gamepad) {
	return "none";
}

bool kore_gamepad_connected(int num) {
	return true;
}

void kore_gamepad_rumble(int gamepad, float left, float right) {}

int main(int argc, char *argv[]) {
	int retVal = 0;
	@autoreleasepool {
		[KoreAppDelegate description]; // otherwise removed by the linker
		retVal = UIApplicationMain(argc, argv, nil, @"KoreAppDelegate");
	}
	return retVal;
}
