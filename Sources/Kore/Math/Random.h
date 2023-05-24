#pragma once

#include <Kore/global.h>

namespace Kore {
	namespace Random {
		void init(int64_t seed);
		int64_t get();
		int64_t get(int64_t max);
		int64_t get(int64_t min, int64_t max);
	}
}
