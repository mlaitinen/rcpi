#include <stdio.h>
#include "input.h"

#include "keyboard.h"

int open_keyboard()
{
	return 0;
}

void close_keyboard()
{

}

int read_inputevent_kb(struct inputdev_event *ev)
{
	float val;
	int res = scanf("%f", &val);
	if (res == EOF || res == 0)
	{
		printf("Failed to read a float value from the user\n");
		return res;
	}

	ev->target = INPUTDEV_ACCEL;
	ev->magnitude = val;

	return 1;
}

