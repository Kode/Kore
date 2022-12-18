#include "System.h"

#include <Kore/Convert.h>
#include <Kore/Math/Random.h>
#include <Kore/Window.h>

#include <kinc/input/keyboard.h>
#include <kinc/system.h>
#include <kinc/video.h>
#include <kinc/window.h>

#include <assert.h>
#include <limits>
#include <string.h>

double Kore::System::time() {
	return kinc_time();
}

void Kore::System::setCallback(void (*callback)(void *), void *data) {
	kinc_set_update_callback(callback, data);
}

void Kore::System::setForegroundCallback(void (*callback)(void *), void *data) {
	kinc_set_foreground_callback(callback, data);
}

void Kore::System::setResumeCallback(void (*callback)(void *), void *data) {
	kinc_set_resume_callback(callback, data);
}

void Kore::System::setPauseCallback(void (*callback)(void *), void *data) {
	kinc_set_pause_callback(callback, data);
}

void Kore::System::setBackgroundCallback(void (*callback)(void *), void *data) {
	kinc_set_background_callback(callback, data);
}

void Kore::System::setShutdownCallback(void (*callback)(void *), void *data) {
	kinc_set_shutdown_callback(callback, data);
}

void Kore::System::setOrientationCallback(void (*callback)(Orientation, void *), void *data) {}

void Kore::System::setDropFilesCallback(void (*callback)(wchar_t *, void *), void *data) {
	kinc_set_drop_files_callback(callback, data);
}

void Kore::System::setCutCallback(char *(*callback)(void *), void *data) {
	kinc_set_cut_callback(callback, data);
}

void Kore::System::setCopyCallback(char *(*callback)(void *), void *data) {
	kinc_set_copy_callback(callback, data);
}

void Kore::System::setPasteCallback(void (*callback)(char *, void *), void *data) {
	kinc_set_paste_callback(callback, data);
}

void Kore::System::setLoginCallback(void (*callback)(void *), void *data) {
	kinc_set_login_callback(callback, data);
}

void Kore::System::setLogoutCallback(void (*callback)(void *), void *data) {
	kinc_set_logout_callback(callback, data);
}

void Kore::System::_callback() {
	kinc_internal_update_callback();
}

void Kore::System::_foregroundCallback() {
	kinc_internal_foreground_callback();
}

void Kore::System::_resumeCallback() {
	kinc_internal_resume_callback();
}

void Kore::System::_pauseCallback() {
	kinc_internal_pause_callback();
}

void Kore::System::_backgroundCallback() {
	kinc_internal_background_callback();
}

void Kore::System::_shutdownCallback() {
	kinc_internal_shutdown_callback();
}

void Kore::System::_orientationCallback(Orientation orientation) {}

void Kore::System::_dropFilesCallback(wchar_t *filePath) {
	kinc_internal_drop_files_callback(filePath);
}

char *Kore::System::_cutCallback() {
	return kinc_internal_cut_callback();
}

char *Kore::System::_copyCallback() {
	return kinc_internal_copy_callback();
}

void Kore::System::_pasteCallback(char *value) {
	kinc_internal_paste_callback(value);
}

namespace {
	bool running = false;
	// bool showWindowFlag = true;
	Kore::WindowOptions defaultWin;
	Kore::FramebufferOptions defaultFrame;
}
/*
void Kore::System::setShowWindowFlag(bool value) {
    appstate::showWindowFlag = value;
}

bool Kore::System::hasShowWindowFlag() {
    return appstate::showWindowFlag;
}

void Kore::System::setName(const char* value) {
    strcpy(appstate::name, value);
}
*/
const char *Kore::System::name() {
	return kinc_application_name();
}

#ifdef KORE_METAL
void shutdownMetalCompute();
#endif

void Kore::System::stop() {
	running = false;
	kinc_stop();
}

void Kore::System::login() {
	kinc_login();
}

bool Kore::System::waitingForLogin() {
	return kinc_waiting_for_login();
}

void Kore::System::unlockAchievement(int id) {
	kinc_unlock_achievement(id);
}

int Kore::System::windowWidth(int window) {
	assert(window < Window::count());
	return Window::get(window)->width();
}

int Kore::System::windowHeight(int window) {
	assert(window < Window::count());
	return Window::get(window)->height();
}

Kore::Window *Kore::System::init(const char *name, int width, int height, Kore::WindowOptions *win, Kore::FramebufferOptions *frame) {
	kinc_window_options_t kwin;
	if (win != nullptr) {
		kwin = convert(win);
	}

	kinc_framebuffer_options_t kframe;
	if (frame != nullptr) {
		kframe = convert(frame);
	}

	int window = kinc_init(name, width, height, win == nullptr ? nullptr : &kwin, frame == nullptr ? nullptr : &kframe);
	return Window::get(window);
}

const char *Kore::System::savePath() {
	return kinc_internal_save_path();
}

bool Kore::System::handleMessages() {
	return kinc_internal_handle_messages();
}

void Kore::System::_shutdown() {
	kinc_internal_shutdown();
}

void Kore::System::start() {
	kinc_start();
}

void Kore::System::setKeepScreenOn(bool on) {
	kinc_set_keep_screen_on(on);
}

const char *Kore::System::systemId() {
	return kinc_system_id();
}

void Kore::System::vibrate(int milliseconds) {
	kinc_vibrate(milliseconds);
}

const char *Kore::System::language() {
	return kinc_language();
}

void Kore::System::showKeyboard() {
	kinc_keyboard_show();
}

void Kore::System::hideKeyboard() {
	kinc_keyboard_hide();
}

bool Kore::System::showsKeyboard() {
	return kinc_keyboard_active();
}

void Kore::System::loadURL(const char *url) {
	kinc_load_url(url);
}

const char **Kore::System::videoFormats() {
	return kinc_video_formats();
}

float Kore::System::safeZone() {
	return kinc_safe_zone();
}

bool Kore::System::automaticSafeZone() {
	return kinc_automatic_safe_zone();
}

void Kore::System::setSafeZone(float value) {
	kinc_set_safe_zone(value);
}

void Kore::System::copyToClipboard(const char *text) {
	kinc_copy_to_clipboard(text);
}
