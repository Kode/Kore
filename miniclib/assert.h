#ifndef KORE_MINICLIB_ASSERT_HEADER
#define KORE_MINICLIB_ASSERT_HEADER

#ifdef NDEBUG
#define assert(condition)
#else
static void assert(int condition) {}
#endif

#endif
