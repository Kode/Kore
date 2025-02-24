#include <kore3/network/http.h>

#if !defined KORE_MACOS && !defined KORE_IOS && !defined KORE_WINDOWS

#include <assert.h>

void kore_http_request(const char *url, const char *path, const char *data, int port, bool secure, int method, const char *header, kore_http_callback callback,
                       void *callbackdata) {
	assert(false); // not implemented for the current system, please send a pull-request
}

#endif
