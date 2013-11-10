#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <pigpio.h>

#include "input.h"
#include "joystick.h"
#include "keyboard.h"

int (*open_input)(void);
void (*close_input)(void);
int (*read_input)(struct inputdev_event *);

void sig_handler(int signo)
{
	printf("Sig: %d\n", signo);
	if(signo == SIGINT)
	{
		gpioServo(18, 0);
		gpioTerminate();
		printf("GPIO terminated.\n");

		close_input();
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	int start_ok = 0;
	if (argc > 1)
	{
		if (strcmp(argv[1], "kb") == 0)
		{
			open_input = &open_keyboard;
			close_input = &close_keyboard;
			read_input = &read_inputevent_kb;

			start_ok = 1;
			printf("Using keyboard as the input device\n");
		}
		else if (strcmp(argv[1], "js") == 0)
		{
			open_input = &open_joystick;
			close_input = &close_joystick;
			read_input = &read_inputevent_js;

			start_ok = 1;
			printf("Using joystick as the input device\n");
		}
	}

	if (start_ok != 1)
	{
		printf("Usage: %s [kb|js]\n", argv[0]);
		exit(1);
	}

	int fd, rc;
	int done = 0;

	struct inputdev_event ev;

	gpioInitialise();
	printf("GPIO initialized\n");

	gpioSetMode(23, PI_OUTPUT); // servo
	gpioSetMode(18, PI_OUTPUT); // teu

	// sig_handler catches the SIGINT signal.
	if(signal(SIGINT, sig_handler) == SIG_ERR) printf("Can't catch SIGINT\n");
	signal(SIGQUIT, sig_handler);
	signal(SIGKILL, sig_handler);

	fd = open_input();
	if (fd < 0)
	{
		printf("open input device failed.\n");
		exit(1);
	}

	while (!done)
	{
		rc = read_input(&ev);
		usleep(18000);
		if (rc == 1) {
			if(ev.target == INPUTDEV_ACCEL) {

				int servopos = 1500;
				//int offset = -(jse.value / 32.767);
				int offset = ev.magnitude * 1000.0f;

				int position = servopos+offset;
				if(position < 1200) position = 1200;
				else if(position > 2280) position = 2280;

				printf("Accelerate: %.2f\n", ev.magnitude*100.0f);
				gpioServo(18, position);
			} else if(ev.target == INPUTDEV_TURN) {
				int servopos = 1500;
				//int offset = -(jse.value / 32.767);
				int offset = ev.magnitude * 1000.0f;

				int position = servopos+offset;
				if(position < 1195) position = 1195;
				else if(position > 1805) position = 1805;

				printf("Turn: %.2f\n", ev.magnitude*100.0f);
				gpioServo(23, position);
			} else {
				printf("Unknown event\n");
			}

		}
	}
	return 0;
}
