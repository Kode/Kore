#pragma once

/*
#include <Kore/MutexImpl.h>

#if !defined(KINC_WINDOWS) && !defined(KINC_WINDOWSAPP) && defined(KINC_POSIX)
#include <pthread.h>
#endif

namespace Kore {
    class Mutex : public MutexImpl {
    public:
        void create();
        void destroy();
        void lock();
        bool tryToLock();
        void unlock();

    private:
#if defined(KINC_WINDOWS) || defined(KINC_WINDOWSAPP) || defined(KINC_XBOX_ONE)
        struct CriticalSection {
            void* DebugInfo;
            long LockCount;
            long RecursionCount;
            void* OwningThread;
            void* LockSemaphore;
            unsigned long __w64 SpinCount;
        } criticalSection;
#elif defined(KINC_POSIX)
        pthread_mutex_t pthread_mutex;
#endif
    };

    class UberMutex {
    public:
#if defined(KINC_WINDOWS) || defined(KINC_WINDOWSAPP)
        void* id;
#endif

        bool create(const wchar_t* name);
        void destroy();
        void lock();
        void unlock();
    };
}
*/

#include <kinc/threads/mutex.h>

namespace Kore {
	class Mutex {
	public:
		void create();
		void destroy();
		void lock();
		bool tryToLock();
		void unlock();

	private:
		kinc_mutex_t mutex;
	};

	class UberMutex {
	public:
		bool create(const char *name);
		void destroy();
		void lock();
		void unlock();

	private:
		kinc_uber_mutex_t mutex;
	};
}
