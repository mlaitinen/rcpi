#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "joystick.h"

static int joystick_fd = -1;

int open_joystick()
{
	joystick_fd = open(JOYSTICK_DEVNAME, O_RDONLY | O_NONBLOCK); /* read write for force feedback? */
	if (joystick_fd < 0)
		return joystick_fd;

	/* maybe ioctls to interrogate features here? */

	return joystick_fd;
}

int read_joystick_event(struct js_event *jse)
{
	int bytes;

	bytes = read(joystick_fd, jse, sizeof(*jse)); 

	if (bytes == -1)
		return 0;

	if (bytes == sizeof(*jse))
		return 1;

	printf("Unexpected bytes from joystick:%d\n", bytes);

	return -1;
}

int read_inputevent_js(struct inputdev_event *ev)
{
	struct js_event jse;

	int succ = read_joystick_event(&jse);
	if (succ != 1) return succ;

	if (jse.number == 1 && jse.type == 2)
	{
		ev->target = INPUTDEV_ACCEL;
	}
	else if (jse.number == 2 && jse.type == 2)
	{
		ev->target = INPUTDEV_TURN;
	}
	else
	{
		return 0;
	}

	ev->magnitude = -(jse.value / 32.767);

	return 1;
}

void close_joystick()
{
	close(joystick_fd);
}

/*int get_joystick_status(struct wwvi_js_event *wjse)
{
	int rc;
	struct js_event jse;
	if (joystick_fd < 0)
		return -1;

	// memset(wjse, 0, sizeof(*wjse));
	while ((rc = read_joystick_event(&jse) == 1)) {
		jse.type &= ~JS_EVENT_INIT; // ignore synthetic events
		if (jse.type == JS_EVENT_AXIS) {
			switch (jse.number) {
			case 0: wjse->stick1_x = jse.value;
				break;
			case 1: wjse->stick1_y = jse.value;
				break;
			case 2: wjse->stick2_x = jse.value;
				break;
			case 3: wjse->stick2_y = jse.value;
				break;
			default:
				break;
			}
		} else if (jse.type == JS_EVENT_BUTTON) {
			if (jse.number < 10) {
				switch (jse.value) {
				case 0:
				case 1: wjse->button[jse.number] = jse.value;
					break;
				default:
					break;
				}
			}
		}
	}
	// printf("%d\n", wjse->stick1_y);
	return 0;
}*/
