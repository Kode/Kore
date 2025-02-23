#ifndef KORE_THREADS_FIBER_HEADER
#define KORE_THREADS_FIBER_HEADER

#include <kore3/global.h>

#include <kore3/backend/fiber.h>

/*! \file fiber.h
    \brief The fiber-API is experimental and only supported on a few system.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_fiber {
	kore_fiber_impl impl;
} kore_fiber;

/// <summary>
/// Uses the current thread as a fiber.
/// </summary>
/// <param name="fiber">The fiber-object to initialize using the current thread</param>
KORE_FUNC void kore_fiber_init_current_thread(kore_fiber *fiber);

/// <summary>
/// Initializes a fiber.
/// </summary>
/// <param name="fiber">The fiber-object to initialize</param>
/// <param name="func">The function to be run in the fiber-context</param>
/// <param name="param">A parameter to be provided to the fiber-function when it starts running</param>
KORE_FUNC void kore_fiber_init(kore_fiber *fiber, void (*func)(void *param), void *param);

/// <summary>
/// Destroys a fiber.
/// </summary>
/// <param name="fiber">The fiber to destroy</param>
KORE_FUNC void kore_fiber_destroy(kore_fiber *fiber);

/// <summary>
/// Switch the current thread to a different fiber.
/// </summary>
/// <param name="fiber">The fiber to switch to</param>
KORE_FUNC void kore_fiber_switch(kore_fiber *fiber);

#ifdef __cplusplus
}
#endif

#endif
