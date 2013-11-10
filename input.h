#include <stdio.h>

#ifndef __INPUTDEV_H__
#define __INPUTDEV_H__

#define INPUTDEV_TURN	0x01
#define INPUTDEV_ACCEL	0x02

struct inputdev_event {
	int target; // INPUTDEV_*
	float magnitude; // a value between -1.0 ... 1.0
};

#endif
