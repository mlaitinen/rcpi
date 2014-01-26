#include <stdio.h>
#include "input.h"

#ifndef __UDP_H__
#define __UDP_H__

extern int open_udp();
extern void close_udp();
int read_inputevent_udp(struct inputdev_event *ev);

#endif
