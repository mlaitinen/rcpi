#include <stdio.h>

#ifndef __UTIL_H__
#define __UTIL_H__

int clamp (int value, int min, int max);

float clampf (float value, float min, float max);

float lerp (float source, float target, float time); // time between 0.0f ... 1.0f

#endif
