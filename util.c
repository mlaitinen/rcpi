#include <stdio.h>

#include "util.h"

int clamp (int value, int min, int max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float clampf (float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float lerp (float source, float target, float time) {
	return source + (target - source) * time;
}
