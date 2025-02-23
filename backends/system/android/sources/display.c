#include <kore3/backend/android.h>

#include <kore3/display.h>
#include <kore3/log.h>

typedef struct {
	bool available;
	int x;
	int y;
	int width;
	int height;
	bool primary;
	int number;
} kore_display;

static kore_display display;

int kore_count_displays(void) {
	return 1;
}

int kore_primary_display(void) {
	return 0;
}

static int width() {
	JNIEnv *env;
	JavaVM *vm = kinc_android_get_activity()->vm;
	(*vm)->AttachCurrentThread(vm, &env, NULL);
	jclass koreActivityClass = kinc_android_find_class(env, "tech.kore.KoreActivity");
	jmethodID koreActivityGetScreenDpi = (*env)->GetStaticMethodID(env, koreActivityClass, "getDisplayWidth", "()I");
	int width = (*env)->CallStaticIntMethod(env, koreActivityClass, koreActivityGetScreenDpi);
	(*vm)->DetachCurrentThread(vm);
	return width;
}

static int height() {
	JNIEnv *env;
	JavaVM *vm = kinc_android_get_activity()->vm;
	(*vm)->AttachCurrentThread(vm, &env, NULL);
	jclass koreActivityClass = kinc_android_find_class(env, "tech.kinc.KincActivity");
	jmethodID koreActivityGetScreenDpi = (*env)->GetStaticMethodID(env, koreActivityClass, "getDisplayHeight", "()I");
	int height = (*env)->CallStaticIntMethod(env, koreActivityClass, koreActivityGetScreenDpi);
	(*vm)->DetachCurrentThread(vm);
	return height;
}

static int pixelsPerInch() {
	JNIEnv *env;
	JavaVM *vm = kinc_android_get_activity()->vm;
	(*vm)->AttachCurrentThread(vm, &env, NULL);
	jclass koreActivityClass = kinc_android_find_class(env, "tech.kinc.KincActivity");
	jmethodID koreActivityGetScreenDpi = (*env)->GetStaticMethodID(env, koreActivityClass, "getScreenDpi", "()I");
	int dpi = (*env)->CallStaticIntMethod(env, koreActivityClass, koreActivityGetScreenDpi);
	(*vm)->DetachCurrentThread(vm);
	return dpi;
}

static int refreshRate() {
	JNIEnv *env;
	JavaVM *vm = kinc_android_get_activity()->vm;
	(*vm)->AttachCurrentThread(vm, &env, NULL);
	jclass koreActivityClass = kinc_android_find_class(env, "tech.kinc.KincActivity");
	jmethodID koreActivityGetScreenDpi = (*env)->GetStaticMethodID(env, koreActivityClass, "getRefreshRate", "()I");
	int dpi = (*env)->CallStaticIntMethod(env, koreActivityClass, koreActivityGetScreenDpi);
	(*vm)->DetachCurrentThread(vm);
	return dpi;
}

void kore_display_init() {}

kore_display_mode kore_display_available_mode(int display_index, int mode_index) {
	kore_display_mode mode;
	mode.x = 0;
	mode.y = 0;
	mode.width = width();
	mode.height = height();
	mode.frequency = refreshRate();
	mode.bits_per_pixel = 32;
	mode.pixels_per_inch = pixelsPerInch();
	return mode;
}

int kore_display_count_available_modes(int display_index) {
	return 1;
}

kore_display_mode kore_display_current_mode(int display) {
	kore_display_mode mode;
	mode.x = 0;
	mode.y = 0;
	mode.width = width();
	mode.height = height();
	mode.frequency = refreshRate();
	mode.bits_per_pixel = 32;
	mode.pixels_per_inch = pixelsPerInch();
	return mode;
}

const char *kore_display_name(int display) {
	return "Display";
}

bool kore_display_available(int display) {
	return display == 0;
}
