#include <kore3/threads/fiber.h>

static VOID WINAPI fiber_func(LPVOID param) {
#ifndef KINC_WINDOWSAPP
	kore_fiber *fiber = (kore_fiber *)param;
	fiber->impl.func(fiber->impl.param);
#endif
}

void kore_fiber_init_current_thread(kore_fiber *fiber) {
#ifndef KINC_WINDOWSAPP
	fiber->impl.fiber = ConvertThreadToFiber(NULL);
#endif
}

void kore_fiber_init(kore_fiber *fiber, void (*func)(void *param), void *param) {
#ifndef KINC_WINDOWSAPP
	fiber->impl.func = func;
	fiber->impl.param = param;
	fiber->impl.fiber = CreateFiber(0, fiber_func, fiber);
#endif
}

void kore_fiber_destroy(kore_fiber *fiber) {
#ifndef KINC_WINDOWSAPP
	DeleteFiber(fiber->impl.fiber);
	fiber->impl.fiber = NULL;
#endif
}

void kore_fiber_switch(kore_fiber *fiber) {
#ifndef KINC_WINDOWSAPP
	SwitchToFiber(fiber->impl.fiber);
#endif
}
