#ifndef KORE_LOG_HEADER
#define KORE_LOG_HEADER

#include <kore3/global.h>

#include <stdarg.h>

/*! \file log.h
    \brief Contains basic logging functionality.

    Logging functionality is similar to plain printf but provides some system-specific bonuses.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Pass this to kore_log or kore_log_args
/// </summary>
/// <remarks>
/// When used on Android the log level is converted to the equivalent
/// Android logging level. It is currently ignored on all other targets.
/// </remarks>
typedef enum { KORE_LOG_LEVEL_INFO, KORE_LOG_LEVEL_WARNING, KORE_LOG_LEVEL_ERROR } kore_log_level;

/// <summary>
/// Logging function similar to printf including some system-specific bonuses
/// </summary>
/// <remarks>
/// On most systems this is equivalent to printf.
/// On Windows it works with utf-8 strings (like printf does on any other target)
/// and also prints to the debug console in IDEs.
/// On Android this uses the android logging functions and also passes the logging level.
/// </remarks>
/// <param name="log_level">
/// The logLevel is ignored on all targets but Android where it is converted
/// to the equivalent Android log level
/// </param>
/// <param name="format">The parameter is equivalent to the first printf parameter.</param>
/// <param name="...">The parameter is equivalent to the second printf parameter.</param>
KORE_FUNC void kore_log(kore_log_level log_level, const char *format, ...);

/// <summary>
/// Equivalent to kore_log but uses a va_list parameter
/// </summary>
/// <remarks>
/// You will need this if you want to log parameters using va_start/va_end.
/// </remarks>
/// <param name="log_level">
/// The logLevel is ignored on all targets but Android where it is converted
/// to the equivalent Android log level
/// </param>
/// <param name="format">The parameter is equivalent to the first vprintf parameter.</param>
/// <param name="args">The parameter is equivalent to the second vprintf parameter.</param>
KORE_FUNC void kore_log_args(kore_log_level log_level, const char *format, va_list args);

#ifdef __cplusplus
}
#endif

#endif
