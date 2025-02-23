#ifndef KORE_THREADS_EVENT_HEADER
#define KORE_THREADS_EVENT_HEADER

#include <kore3/global.h>

#include <kore3/backend/event.h>

#include <stdbool.h>

/*! \file event.h
    \brief An event is a simple threading-object that allows a thread to wait for something to happen on another thread.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_event {
	kore_event_impl impl;
} kore_event;

/// <summary>
/// Initializes an event-object.
/// </summary>
/// <param name="event">The event to initialize</param>
/// <param name="auto_clear">When auto-clear is true, the event is automatically reset to an unsignaled state after a successful wait-operation</param>
KORE_FUNC void kore_event_init(kore_event *event, bool auto_clear);

/// <summary>
/// Destroys an event-object.
/// </summary>
/// <param name="event">The event to destroy</param>
KORE_FUNC void kore_event_destroy(kore_event *event);

/// <summary>
/// Signals an event which allows threads which are waiting for the event to continue.
/// </summary>
/// <param name="event">The event to signal</param>
KORE_FUNC void kore_event_signal(kore_event *event);

/// <summary>
/// Waits for an event to be signaled.
/// </summary>
/// <param name="event">The event to wait for</param>
KORE_FUNC void kore_event_wait(kore_event *event);

/// <summary>
/// Waits for an event to be signaled or the provided timeout to run out - whatever happens first.
/// </summary>
/// <param name="event">The event to wait for</param>
/// <param name="timeout">The timeout in seconds after which the function returns if the event hasn't been signaled</param>
/// <returns>Whether the event has been signaled</returns>
KORE_FUNC bool kore_event_try_to_wait(kore_event *event, double timeout);

/// <summary>
/// Resets an event to an unsignaled state.
/// </summary>
/// <param name="event">The event to reset</param>
KORE_FUNC void kore_event_reset(kore_event *event);

#ifdef __cplusplus
}
#endif

#endif
