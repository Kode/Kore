#ifndef KORE_MINICLIB_STDARG_HEADER
#define KORE_MINICLIB_STDARG_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef __builtin_va_list va_list;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
/* C23 uses a special builtin. */
#define va_start(...) __builtin_c23_va_start(__VA_ARGS__)
#else
/* Versions before C23 do require the second parameter. */
#define va_start(ap, param) __builtin_va_start(ap, param)
#endif
#define va_end(ap)       __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)

#ifdef __cplusplus
}
#endif

#endif
