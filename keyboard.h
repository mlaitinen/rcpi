#include <stdio.h>
#include "input.h"

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

extern int open_keyboard();
extern void close_keyboard();
int read_inputevent_kb(struct inputdev_event *ev);

#endif
