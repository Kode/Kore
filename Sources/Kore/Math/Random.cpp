#include "Random.h"

#include <kinc/math/random.h>

using namespace Kore;

void Random::init(int64_t seed) {
	kinc_random_init(seed);
}

int64_t Random::get() {
	return kinc_random_get();
}

int64_t Random::get(int64_t max) {
	return kinc_random_get_max(max);
}

int64_t Random::get(int64_t min, int64_t max) {
	return kinc_random_get_in(min, max);
}
