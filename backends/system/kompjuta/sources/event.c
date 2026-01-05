#include <kore3/threads/event.h>

void kore_event_init(kore_event *event, bool auto_reset) {}

void kore_event_destroy(kore_event *event) {}

void kore_event_signal(kore_event *event) {}

void kore_event_wait(kore_event *event) {}

bool kore_event_try_to_wait(kore_event *event, double seconds) {
	return false;
}

void kore_event_reset(kore_event *event) {}
