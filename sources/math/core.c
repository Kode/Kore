#include <kore3/math/core.h>

#include <math.h>

float kore_cot(float x) {
	return cosf(x) / sinf(x);
}

float kore_round(float value) {
	return floorf(value + 0.5f);
}

float kore_abs(float value) {
	return value < 0 ? -value : value;
}

float kore_min(float a, float b) {
	return a > b ? b : a;
}

float kore_max(float a, float b) {
	return a > b ? a : b;
}

int kore_mini(int a, int b) {
	return a > b ? b : a;
}

int kore_maxi(int a, int b) {
	return a > b ? a : b;
}

float kore_clamp(float value, float minValue, float maxValue) {
	return kore_max(minValue, kore_min(maxValue, value));
}
