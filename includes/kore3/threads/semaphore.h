#ifndef KORE_THREADS_SEMAPHORE_HEADER
#define KORE_THREADS_SEMAPHORE_HEADER

#include <kore3/global.h>

#include <kore3/backend/semaphore.h>

#include <stdbool.h>

/*! \file semaphore.h
    \brief A semaphore is a fancier version of an event that includes a counter to control how many threads are allowed to work on a task.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_semaphore {
	kore_semaphore_impl impl;
} kore_semaphore;

/// <summary>
/// Initializes a semaphore.
/// </summary>
/// <param name="semaphore">The semaphore to initialize</param>
/// <param name="current">The current count of the semaphore</param>
/// <param name="max">The maximum allowed count of the semaphore</param>
KORE_FUNC void kore_semaphore_init(kore_semaphore *semaphore, int current, int max);

/// <summary>
/// Destroys a semaphore.
/// </summary>
/// <param name="semaphore">The semaphore to destroy</param>
KORE_FUNC void kore_semaphore_destroy(kore_semaphore *semaphore);

/// <summary>
/// Increases the current count of the semaphore, therefore allowing more acquires to succeed.
/// </summary>
/// <param name="semaphore">The semaphore to increase the count on</param>
/// <param name="count">The amount by which the count will be increased</param>
KORE_FUNC void kore_semaphore_release(kore_semaphore *semaphore, int count);

/// <summary>
/// Decreases the count of the semaphore by one. Blocks until it is possible to decrease the count if it already reached zero.
/// </summary>
/// <param name="semaphore">The semaphore to acquire</param>
KORE_FUNC void kore_semaphore_acquire(kore_semaphore *semaphore);

/// <summary>
/// Attempts to decrease the count of the semaphore by one.
/// </summary>
/// <param name="semaphore">The semaphore to acquire</param>
/// <param name="timeout">The timeout in seconds after which the function returns if the semaphore-count could not be decreased</param>
/// <returns>Whether the semaphore-count could be decreased</returns>
KORE_FUNC bool kore_semaphore_try_to_acquire(kore_semaphore *semaphore, double timeout);

#ifdef __cplusplus
}
#endif

#endif
