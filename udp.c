#include <stdio.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "input.h"
#include "udp.h"

#define PORT 18169
#define BUFFERSIZE 2

int sd; // Socket file descriptor
signed char buffer[BUFFERSIZE];

int open_udp()
{
	// Create a socket.
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd < 0)
	{
		syslog(LOG_ERR, "Unable to create a socket.");
		return sd;
	}

	// Name the socket
	struct sockaddr_in address;
	address.sin_family = AF_INET; // IPv4
	address.sin_addr.s_addr = htonl(INADDR_ANY); // Any IP address
	address.sin_port = htons(PORT);
	if (bind(sd, (struct sockaddr *) &address, sizeof(address)) < 0)
	{
		syslog(LOG_ERR, "Unable to bind socket.");
		return -1;
	}

	// Set the timeout to one second
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval));

	syslog(LOG_INFO, "Listening for UDP packets. Port: %d.", PORT);

	return 0;
}

int read_inputevent_udp(struct inputdev_event *ev)
{
	// Receive data from socket (blocking function call w/ timeout)
	int recvlen = recv(sd, buffer, BUFFERSIZE, 0);

	if (recvlen > 0)
	{
		int target = buffer[0];
		int magnitude = buffer[1];

		ev->target = target;
		ev->magnitude = ((float) magnitude) / 100.0f;

		return 1;
	}

	return 0;
}

void close_udp()
{
	close(sd);
}
