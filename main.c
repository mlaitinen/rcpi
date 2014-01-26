#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <syslog.h>
#include <pigpio.h>

#include "input.h"
#include "joystick.h"
#include "keyboard.h"
#include "udp.h"
#include "util.h"

int (*open_input)(void);
void (*close_input)(void);
int (*read_input)(struct inputdev_event *);

const int READ_FREQ = 8000; // 8 ms
const int SERVO_CENTER = 1500;

const int TURN_MAX = 1805;
const int TURN_MIN = 1195;

const int ACCEL_MAX = 2280;
const int ACCEL_MIN = 1200;

const int PIN_SERVO = 23;
const int PIN_ESC = 18;

// handle system signals
void sig_handler(int signo)
{
	syslog(LOG_INFO, "Received signal: %d", signo);
	if(signo == SIGINT || signo == SIGTERM)
	{
		gpioServo(18, 0);
		gpioTerminate();
		syslog(LOG_INFO, "GPIO terminated.");

		close_input();
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char *argv[])
{
	// set the permission umask for new files
	umask(0);

	// open the log
	openlog("rcpi", LOG_PID|LOG_CONS|LOG_NDELAY, LOG_LOCAL1);

	int start_ok = 0;

	if (argc > 1)
	{
		if (strcmp(argv[1], "kb") == 0)
		{
			open_input = &open_keyboard;
			close_input = &close_keyboard;
			read_input = &read_inputevent_kb;

			start_ok = 1;
			syslog(LOG_INFO, "Using keyboard as the input device");
		}
		else if (strcmp(argv[1], "js") == 0)
		{
			open_input = &open_joystick;
			close_input = &close_joystick;
			read_input = &read_inputevent_js;

			start_ok = 1;
			syslog(LOG_INFO, "Using joystick as the input device");
		}
		else if (strcmp(argv[1], "udp") == 0)
		{
			open_input = &open_udp;
			close_input = &close_udp;
			read_input = &read_inputevent_udp;

			start_ok = 1;
			syslog(LOG_INFO, "Using UDP socket as the input device");
		}
	}

	if (start_ok != 1)
	{
		syslog(LOG_INFO, "Usage: %s [kb|js|udp]", argv[0]);
		exit(1);
	}


	int fd, rc;
	int done = 0;

	struct inputdev_event ev;

	fd = open_input();
	if (fd < 0)
	{
		syslog(LOG_ERR, "open input device failed.");
		exit(1);
	}


	pid_t cpid = fork();
	if (cpid < 0)
	{
		// parent process exits unsuccessfully.
		exit(EXIT_FAILURE);
	}
	else if (cpid > 0)
	{
		// parent process exits successfully.
		exit(EXIT_SUCCESS);
	}

	pid_t sid = setsid();
	if (sid < 0)
	{
		syslog(LOG_ERR, "failed to create a new SID");
		exit(EXIT_FAILURE);
	}

	// close the standard file descriptors
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// initialize the GPIO
	gpioInitialise();
	syslog(LOG_INFO, "GPIO initialized");

	gpioSetMode(PIN_SERVO, PI_OUTPUT); // servo output pin
	gpioSetMode(PIN_ESC, PI_OUTPUT); // esc output pin

	// sig_handler catches the SIGINT signal.
	if(signal(SIGINT, sig_handler) == SIG_ERR) syslog(LOG_WARNING, "Can't catch SIGINT");
	signal(SIGQUIT, sig_handler);
	signal(SIGKILL, sig_handler);
	signal(SIGTERM, sig_handler);

	// center the motors
	gpioServo(PIN_SERVO, SERVO_CENTER);
	gpioServo(PIN_ESC, SERVO_CENTER);

//	int accel_source = SERVO_CENTER;
	int accel_target = SERVO_CENTER;
//	float accel_time = 0.0f;

	while (!done)
	{
		rc = read_input(&ev);
		usleep(READ_FREQ);
		if (rc == 1)
		{
			if(ev.target == INPUTDEV_ACCEL)
			{
				int offset = ev.magnitude * 500.0f;
				if(offset < -10) offset -= 100;
				else if(offset > 10) offset += 100;
				syslog(LOG_DEBUG, "Accel offset: %d", offset);
				accel_target = clamp(SERVO_CENTER+offset, ACCEL_MIN, ACCEL_MAX);
				// accel_time = 0.0f;
				gpioServo(18, accel_target);
				syslog(LOG_DEBUG, "Accelerate: %d", accel_target);
			}
			else if(ev.target == INPUTDEV_TURN)
			{
				int offset = ev.magnitude * 310.0f;
				int position = clamp(SERVO_CENTER+offset, TURN_MIN, TURN_MAX);

				gpioServo(23, position);
				syslog(LOG_DEBUG, "Turn: %d", position);
			}
			else
			{
				syslog(LOG_WARNING, "Unknown event");
			}

		}
		else
		{
			syslog(LOG_DEBUG, "Input: %d", rc);
		}

		// Linear interpolation of acceleration
/*		if (accel_source != accel_target)
		{

			printf("Source: %d, Target: %d, ", accel_source, accel_target);
			int position = lerp(accel_source, accel_target, 0.99f);

			printf("Accelerate: %d", position);
			gpioServo(18, position);

			if (abs(accel_source - accel_target) <= 10)
			{
				accel_source = accel_target;
			} else
			{
				accel_source = position;
			}
		}*/
	}
	return 0;
}
