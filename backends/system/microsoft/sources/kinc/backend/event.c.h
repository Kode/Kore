#include <kore3/threads/event.h>

void kore_event_init(kore_event *event, bool auto_clear) {
	event->impl.event = CreateEvent(0, auto_clear ? FALSE : TRUE, 0, 0);
}

void kore_event_destroy(kore_event *event) {
	CloseHandle(event->impl.event);
}

void kore_event_signal(kore_event *event) {
	SetEvent(event->impl.event);
}

void kore_event_wait(kore_event *event) {
	WaitForSingleObject(event->impl.event, INFINITE);
}

bool kore_event_try_to_wait(kore_event *event, double seconds) {
	return WaitForSingleObject(event->impl.event, (DWORD)(seconds * 1000.0)) != WAIT_TIMEOUT;
}

void kore_event_reset(kore_event *event) {
	ResetEvent(event->impl.event);
}
