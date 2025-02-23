#include <kore3/threads/threadlocal.h>

void kore_thread_local_init(kore_thread_local *local) {
	local->impl.slot = TlsAlloc();
	TlsSetValue(local->impl.slot, 0);
}

void kore_thread_local_destroy(kore_thread_local *local) {
	TlsFree(local->impl.slot);
}

void *kore_thread_local_get(kore_thread_local *local) {
	return TlsGetValue(local->impl.slot);
}

void kore_thread_local_set(kore_thread_local *local, void *data) {
	TlsSetValue(local->impl.slot, data);
}
