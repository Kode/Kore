#ifndef KORE_THREADS_MUTEX_HEADER
#define KORE_THREADS_MUTEX_HEADER

#include <kinc/global.h>

#include <kinc/backend/mutex.h>

#include <stdbool.h>

/*! \file mutex.h
    \brief Provides mutexes which are used to synchronize threads and uber-mutexes which are used to synchronize processes.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_mutex {
	kore_mutex_impl impl;
} kore_mutex;

/// <summary>
/// Initializes a mutex-object.
/// </summary>
/// <param name="mutex">The mutex to initialize</param>
KORE_FUNC void kore_mutex_init(kore_mutex *mutex);

/// <summary>
/// Destroys a mutex-object.
/// </summary>
/// <param name="mutex">The mutex to destroy</param>
KORE_FUNC void kore_mutex_destroy(kore_mutex *mutex);

/// <summary>
/// Locks a mutex. A mutex can only be locked from one thread - when other threads attempt to lock the mutex the function will only return once the mutex has
/// been unlocked.
/// </summary>
/// <param name="mutex">The mutex to lock</param>
KORE_FUNC void kore_mutex_lock(kore_mutex *mutex);

/// <summary>
/// Attempts to lock the mutex which will only succeed if no other thread currently holds the lock.
/// </summary>
/// <param name="mutex">The mutex to lock</param>
/// <returns>Whether the mutex could be locked</returns>
KORE_FUNC bool kore_mutex_try_to_lock(kore_mutex *mutex);

/// <summary>
/// Unlocks the mutex which then allows other threads to lock it.
/// </summary>
/// <param name="mutex">The mutex to unlock</param>
KORE_FUNC void kore_mutex_unlock(kore_mutex *mutex);

typedef struct kore_uber_mutex {
	kore_uber_mutex_impl impl;
} kore_uber_mutex;

/// <summary>
/// Initializes an uber-mutex-object.
/// </summary>
/// <param name="mutex">The uber-mutex to initialize</param>
/// <param name="name">A name assigned to the uber-mutex - uber-mutex-creation fails if an uber-mutex of that name already exists</param>
/// <returns>Whether the uber-mutex could be created</returns>
KORE_FUNC bool kore_uber_mutex_init(kore_uber_mutex *mutex, const char *name);

/// <summary>
/// Destroys an uber-mutex-obejct.
/// </summary>
/// <param name="mutex">The uber-mutex to destroy</param>
KORE_FUNC void kore_uber_mutex_destroy(kore_uber_mutex *mutex);

/// <summary>
/// Locks an uber-mutex.
/// </summary>
/// <param name="mutex">The uber-mutex to lock</param>
KORE_FUNC void kore_uber_mutex_lock(kore_uber_mutex *mutex);

/// <summary>
/// Unlocks an uber-mutex.
/// </summary>
/// <param name="mutex">The uber-mutex to unlock</param>
/// <returns></returns>
KORE_FUNC void kore_uber_mutex_unlock(kore_uber_mutex *mutex);

#ifdef __cplusplus
}
#endif

#endif
