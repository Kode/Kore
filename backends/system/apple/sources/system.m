#include <kore3/system.h>

int kore_hardware_threads(void) {
	return (int)[[NSProcessInfo processInfo] processorCount];
}

#ifdef KORE_APPLE_SOC

int kore_cpu_cores(void) {
	return kore_hardware_threads();
}

#else

#include <sys/sysctl.h>

int kore_cpu_cores(void) {
	uint32_t proper_cpu_count = 1;
	size_t count_length       = sizeof(count_length);
	sysctlbyname("hw.physicalcpu", &proper_cpu_count, &count_length, 0, 0);
	return (int)proper_cpu_count;
}

#endif
