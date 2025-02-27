#ifndef KORE_MICROSOFT_HEADER
#define KORE_MICROSOFT_HEADER

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long HRESULT;

void kore_microsoft_affirm(HRESULT result);
void kore_microsoft_affirm_message(HRESULT result, const char *format, ...);
void kore_microsoft_format(const char *format, va_list args, wchar_t *buffer);
void kore_microsoft_convert_string(wchar_t *destination, const char *source, int num);

#ifdef __cplusplus
}
#endif

#endif
