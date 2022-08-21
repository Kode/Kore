#include "Core.h"

#include <kinc/math/core.h>

#include <cmath>

float Kore::cot(float x) {
	return std::cos(x) / std::sin(x);
}

double Kore::cot(double x) {
	return std::cos(x) / std::sin(x);
}

float Kore::round(float value) {
	return kinc_round(value);
}

float Kore::abs(float value) {
	return kinc_abs(value);
}

unsigned Kore::pow(unsigned value, unsigned exponent) {
	uint result = 1;
	if (!exponent) return result;
	for (;;) {
		if (exponent & 1) result *= value;
		value *= value;
		if (!(exponent >>= 1)) return result;
	}
}

int Kore::abs(int value) {
	return value < 0 ? -value : value;
}

double Kore::round(double value) {
	return std::floor(value + 0.5);
}
