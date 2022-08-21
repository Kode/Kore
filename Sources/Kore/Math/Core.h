#pragma once

namespace Kore {
	const float pi = 3.141592654f;
	const float tau = 6.283185307f;
	float cot(float x);
	double cot(double x);
	double round(double value);
	float round(float value);
	unsigned pow(unsigned value, unsigned exponent);
	float abs(float value);
	int abs(int value);

	template <class T> T min(T a, T b) {
		return (a < b) ? a : b;
	}

	template <class T> T max(T a, T b) {
		return (a > b) ? a : b;
	}

	template <class T> T clamp(T value, T minValue, T maxValue) {
		const T clampedToMin = value < minValue ? minValue : value;
		return clampedToMin > maxValue ? maxValue : clampedToMin;
	}
}
