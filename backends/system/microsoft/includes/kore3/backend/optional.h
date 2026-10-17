#ifndef KORE_MICROSOFT_OPTIONAL_HEADER
#define KORE_MICROSOFT_OPTIONAL_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef long         LONG;
typedef LONG         HRESULT;
typedef void        *HANDLE;
typedef wchar_t      WCHAR;
typedef const WCHAR *LPCWSTR, *PCWSTR;

#ifndef WINAPI
#define WINAPI __stdcall
#endif

typedef HRESULT(WINAPI *set_thread_description_type)(HANDLE hThread, PCWSTR lpThreadDescription);
extern set_thread_description_type set_thread_description;

void kore_microsoft_optional_init(void);

#ifdef __cplusplus
}
#endif

#endif
