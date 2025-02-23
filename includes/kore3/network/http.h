#ifndef KORE_NETWORK_HTTP_HEADER
#define KORE_NETWORK_HTTP_HEADER

#include <kore3/global.h>

#include <stdbool.h>

/*! \file http.h
    \brief Provides a simple http-API.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_HTTP_GET 0
#define KORE_HTTP_POST 1
#define KORE_HTTP_PUT 2
#define KORE_HTTP_DELETE 3

typedef void (*kore_http_callback)(int error, int response, const char *body, void *callbackdata);

/// <summary>
/// Fires off an http request.
/// </summary>
KINC_FUNC void kore_http_request(const char *url, const char *path, const char *data, int port, bool secure, int method, const char *header,
                                 kore_http_callback callback, void *callbackdata);

#ifdef __cplusplus
}
#endif

#endif
