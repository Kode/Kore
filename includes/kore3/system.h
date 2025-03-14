#ifndef KORE_SYSTEM_HEADER
#define KORE_SYSTEM_HEADER

#include <kore3/global.h>

#include <kore3/log.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! \file system.h
    \brief Provides basic system and application-management functionality which doesn't fit anywhere else.
*/

#ifdef __cplusplus
extern "C" {
#endif

struct kore_window_parameters;
struct kore_framebuffer_parameters;

/// <summary>
/// Initializes a Kore application and creates an initial window for systems which support windows (systems which do not support windows are treated as if the
/// would provide a single window which cannot change). This has to be called before any other Kore-function with the exception of the Display-API which can
/// optionally be initialized beforehand using kore_display_init.
/// </summary>
/// <returns>The id of the initial window</returns>
KORE_FUNC int kore_init(const char *name, int width, int height, struct kore_window_parameters *win, struct kore_framebuffer_parameters *frame);

/// <summary>
/// Returns the current application name as set by kore_init or kore_set_application_name.
/// </summary>
/// <returns>The current name of the application</returns>
KORE_FUNC const char *kore_application_name(void);

/// <summary>
/// Changes the application-name that was initially set by kore_init.
/// </summary>
/// <param name="name">The new application-name</param>
KORE_FUNC void kore_set_application_name(const char *name);

/// <summary>
/// Returns the current width of the initial application-window which is equivalent to calling kore_window_width(0).
/// </summary>
/// <returns>The width of the initial window</returns>
KORE_FUNC int kore_width(void);

/// <summary>
/// Returns the current height of the initial application-window which is equivalent to calling kore_window_height(0).
/// </summary>
/// <returns>The height of the initial window</returns>
KORE_FUNC int kore_height(void);

/// <summary>
/// Instruct the system to load up the provided URL which will usually open it in the system's default browser.
/// </summary>
/// <param name="url">The URL to open</param>
KORE_FUNC void kore_load_url(const char *url);

/// <summary>
/// Returns an ID representing the current type of target-system.
/// </summary>
/// <returns>The ID representing the target system</returns>
KORE_FUNC const char *kore_system_id(void);

/// <summary>
/// Returns the current system-language.
/// </summary>
/// <returns>The current system-language as a two-letter language code</returns>
KORE_FUNC const char *kore_language(void);

/// <summary>
/// Vibrates the whole system if supported. This is primarily supported on mobile phones but don't blame us if your computer falls over.
/// </summary>
KORE_FUNC void kore_vibrate(int milliseconds);

/// <summary>
/// Returns the portion of the screen which can be safely used for important content. This is mostly relevant for TVs which often scale the image by default and
/// thefore cut off some of the content.
/// </summary>
/// <returns>The safe-zone which can be multiplied with the width or height of the display to convert it to pixels</returns>
KORE_FUNC float kore_safe_zone(void);

/// <summary>
/// Returns whether the system itself handles configuration of the safe-zone.
/// </summary>
/// <returns>Whether the safe-zone is handlet by the syste</returns>
KORE_FUNC bool kore_automatic_safe_zone(void);

/// <summary>
/// Sets the safe-zone for systems which return false for kore_automatic_safe_zone.
/// </summary>
/// <param name="value">The safe-zone for width and height as a ratio of the full display-resolution.</param>
KORE_FUNC void kore_set_safe_zone(float value);

typedef uint64_t kore_ticks;

/// <summary>
/// Returns the frequency of system-timestamps.
/// </summary>
/// <returns>The frequency of the system's timestamps in 1 / second</returns>
KORE_FUNC double kore_frequency(void);

/// <summary>
/// Returns a timestamp for right now in a system-specific unit.
/// </summary>
/// <returns>The current timestamp</returns>
KORE_FUNC kore_ticks kore_timestamp(void);

/// <summary>
/// Returns the number of proper CPU-cores (not the number of hardware-threads)
/// </summary>
/// <returns>Number of cores</returns>
KORE_FUNC int kore_cpu_cores(void);

/// <summary>
/// Returns the number of hardware-threads
/// </summary>
/// <returns>Number of hardware-threads</returns>
KORE_FUNC int kore_hardware_threads(void);

/// <summary>
/// Returns the current time. This can also be calculated ala kore_timestamp() / kore_frequency() but kore_time is a little more precise on some systems.
/// </summary>
/// <returns>The current time in seconds</returns>
KORE_FUNC double kore_time(void);

/// <summary>
/// Starts Kore's main-loop. kore_set_update_callback should be called before kore_start so the main-loop actually has something to do.
/// </summary>
KORE_FUNC void kore_start(void);

/// <summary>
/// Stops Kore's main loop and thereby returns to the function which called kore_start.
/// </summary>
KORE_FUNC void kore_stop(void);

/// <summary>
/// Instructs the system to login a user if that is supported.
/// </summary>
KORE_FUNC void kore_login(void);

/// <summary>
/// Returns true if kore_login was called and the login-process is still ongoing.
/// </summary>
/// <returns>Whether a login-process is still in progress</returns>
KORE_FUNC bool kore_waiting_for_login(void);

/// <summary>
/// Unlocks an achievement or trophy or however you prefer to call it.
/// </summary>
/// <param name="id">The id of the achievement/tropy</param>
KORE_FUNC void kore_unlock_achievement(int id);

/// <summary>
/// Disallows the system to logout the current user.
/// </summary>
KORE_FUNC void kore_disallow_user_change(void);

/// <summary>
/// Allows the system to logout the current user.
/// </summary>
KORE_FUNC void kore_allow_user_change(void);

/// <summary>
/// Instructs the system whether it is allowed to turn off the screen while the application is running.
/// </summary>
/// <param name="on">Whether turning off the screen is allowed</param>
KORE_FUNC void kore_set_keep_screen_on(bool on);

/// <summary>
/// Tries to halt program-execution in an attached debugger when compiled in debug-mode (aka when NDEBUG is not defined).
/// </summary>
KORE_INLINE void kore_debug_break(void) {
#ifndef NDEBUG
#if defined(_MSC_VER)
	__debugbreak();
#elif defined(__clang__)
	__builtin_debugtrap();
#else
#if defined(__aarch64__)
	__asm__ volatile(".inst 0xd4200000");
#elif defined(__x86_64__)
	__asm__ volatile("int $0x03");
#else
	kore_log(KORE_LOG_LEVEL_WARNING, "Oh no, kore_debug_break is not implemented for the current compiler and CPU.");
#endif
#endif
#endif
}

/// <summary>
/// Returns whether a debugger is currently attached to the running program. This is not yet working though.
/// </summary>
/// <returns>Whether a debugger is currently attached</returns>
KORE_FUNC bool kore_debugger_attached(void);

/// <summary>
/// Copies the provided string to the system's clipboard.
/// </summary>
/// <param name="text">The text to be copied into the clipboard</param>
KORE_FUNC void kore_copy_to_clipboard(const char *text);

/// <summary>
/// Sets the update-callback which drives the application and is called for every frame.
/// </summary>
/// <param name="callback">The callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_update_callback(void (*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called whenever the application is brought to the foreground.
/// </summary>
/// <param name="callback">The foreground-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_foreground_callback(void (*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called whenever the application was paused and is being resumed.
/// </summary>
/// <param name="callback">The resume-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_resume_callback(void (*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called whenever the application is paused.
/// </summary>
/// <param name="callback">The pause-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_pause_callback(void (*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called whenever the application is brought to the background.
/// </summary>
/// <param name="callback">The background-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_background_callback(void (*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called whenever the application is about to shut down.
/// </summary>
/// <param name="callback">The shutdown-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_shutdown_callback(void (*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called when files are dropped on the application-window.
/// </summary>
/// <param name="callback">The drop-files-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_drop_files_callback(void (*callback)(wchar_t *, void *), void *data);

/// <summary>
/// Sets a callback which is called when the application is instructed to cut, typically via ctrl+x or cmd+x.
/// Kore does not take ownership of the provided string.
/// </summary>
/// <param name="callback">The cut-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_cut_callback(char *(*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called when the application is instructed to copy, typically via ctrl+c or cmd+c.
/// Kore does not take ownership of the provided string.
/// </summary>
/// <param name="callback">The copy-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_copy_callback(char *(*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called when the application is instructed to paste, typically via ctrl+v or cmd+v.
/// The provided string is only valid during the callback-call - copy it if you want to keep it.
/// </summary>
/// <param name="callback">The paste-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_paste_callback(void (*callback)(char *, void *), void *data);

/// <summary>
/// Sets a callback which is called when a user logs in.
/// </summary>
/// <param name="callback">The login-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_login_callback(void (*callback)(void *), void *data);

/// <summary>
/// Sets a callback which is called when a user logs out.
/// </summary>
/// <param name="callback">The logout-callback</param>
/// <param name="data">Arbitrary data-pointer that's passed to the callback</param>
KORE_FUNC void kore_set_logout_callback(void (*callback)(void *), void *data);

#ifdef KORE_VTUNE
#include <ittnotify.h>

extern __itt_domain *kore_itt_domain;
#endif

#ifdef KORE_SUPERLUMINAL
#include <Superluminal/PerformanceAPI_capi.h>
#endif

/// <summary>
/// Starts a marker for profiling. Please match it with a marker_end-call in the same function.
/// </summary>
/// <param name="name">A unique name that will be shown in the profiler</param>
/// <param name="color">A nice color in RGBX that will show up in the profiler</param>
static inline void kore_marker_start(const char *name, uint32_t color) {
#ifdef KORE_VTUNE
	__itt_task_begin(kore_itt_domain, __itt_null, __itt_null, __itt_string_handle_create(name));
#endif

#ifdef KORE_SUPERLUMINAL
	PerformanceAPI_BeginEvent(name, NULL, color);
#endif
}

/// <summary>
/// Ends the previously started marker of the same function.
/// </summary>
static inline void kore_marker_end(const char *name) {
#ifdef KORE_VTUNE
	__itt_task_end(kore_itt_domain);
#endif

#ifdef KORE_SUPERLUMINAL
	PerformanceAPI_EndEvent();
#endif
}

/// <summary>
/// Currently only supported on Windows.
/// Starts up Live++ (see https://liveplusplus.tech), typically called right before kore_start().
/// After calling this you can hit Ctrl+Alt+F11 to hot-reload.
/// Call Project.addLivePP(path) in your kfile to make this do something.
/// The path-parameter for addLivePP is the path to the unpacked Live++-archive that has to contain a LivePP-subdirectory.
/// This will set the appropricate compiler- and linker-options and set the KORE_LIVEPP-define
/// for the Debug- and Develop-configs (it does not touch the Release-config).
/// </summary>
/// <returns></returns>
KORE_FUNC void kore_LivePP_start(void);

/// <summary>
/// Stops Live++, typically called right after kore_start().
/// </summary>
/// <param name=""></param>
/// <returns></returns>
KORE_FUNC void kore_LivePP_stop(void);

bool        kore_internal_frame(void);
const char *kore_internal_save_path(void);
bool        kore_internal_handle_messages(void);
void        kore_internal_shutdown(void);
void        kore_internal_update_callback(void);
void        kore_internal_foreground_callback(void);
void        kore_internal_resume_callback(void);
void        kore_internal_pause_callback(void);
void        kore_internal_background_callback(void);
void        kore_internal_shutdown_callback(void);
void        kore_internal_drop_files_callback(wchar_t *);
char       *kore_internal_cut_callback(void);
char       *kore_internal_copy_callback(void);
void        kore_internal_paste_callback(char *);
void        kore_internal_login_callback(void);
void        kore_internal_logout_callback(void);

#ifdef __cplusplus
}
#endif

#endif
