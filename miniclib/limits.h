#ifndef KORE_MINICLIB_LIMITS_HEADER
#define KORE_MINICLIB_LIMITS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define INT_MAX  2147483647
#define INT_MIN  (-INT_MAX - 1)
#define UINT_MAX 4294967295U

#define LLONG_MAX  __LONG_LONG_MAX__
#define LLONG_MIN  (-__LONG_LONG_MAX__ - 1LL)
#define ULLONG_MAX (__LONG_LONG_MAX__ * 2ULL + 1ULL)

#ifdef __cplusplus
}
#endif

#endif
