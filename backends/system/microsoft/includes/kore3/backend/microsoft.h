#ifndef KORE_MICROSOFT_HEADER
#define KORE_MICROSOFT_HEADER

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
#ifdef __midl
typedef LONG HRESULT;
#else
typedef _Return_type_success_(return >= 0) long HRESULT;
#endif // __midl
#endif // !_HRESULT_DEFINED

void kore_microsoft_affirm(HRESULT result);
void kore_microsoft_affirm_message(HRESULT result, const char *format, ...);
void kore_microsoft_format(const char *format, va_list args, wchar_t *buffer);
void kore_microsoft_convert_string(wchar_t *destination, const char *source, int num);

#ifdef __cplusplus
}
#endif

#endif
