#ifndef KORE_THREAD_THREADLOCAL_HEADER
#define KORE_THREAD_THREADLOCAL_HEADER

#include <kinc/global.h>

#include <kinc/backend/threadlocal.h>

/*! \file threadlocal.h
    \brief Provides storage-slots for thread-specific data.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_thread_local {
	kore_thread_local_impl impl;
} kore_thread_local;

/// <summary>
/// Initializes a thread-specific storage-slot.
/// </summary>
/// <param name="local">The storage-slot to initialize</param>
KORE_FUNC void kore_thread_local_init(kore_thread_local *local);

/// <summary>
/// Destroys a storage-slot.
/// </summary>
/// <param name="local">The storage-slot to destroy</param>
KORE_FUNC void kore_thread_local_destroy(kore_thread_local *local);

/// <summary>
/// Gets the data in the storage-slot.
/// </summary>
/// <param name="local">The slot to query</param>
KORE_FUNC void *kore_thread_local_get(kore_thread_local *local);

/// <summary>
/// Sets the data in the storage-slot.
/// </summary>
/// <param name="local">The slot to put the data into</param>
/// <param name="data">The data to put in the slot</param>
KORE_FUNC void kore_thread_local_set(kore_thread_local *local, void *data);

#ifdef __cplusplus
}
#endif

#endif
