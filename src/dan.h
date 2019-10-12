#pragma once

#include "types.h"
#include "dude.h"

#include <X11/Xlib.h>

#include <stdbool.h>
/* dan is the name of the protagonist.
 */

void draw_dan(Display * dpy, Window win, const struct dude * dan);

void move_dan(struct dude * dan,
              enum direction dir,
              struct maze * maze,
              Display * dpy,
              Window win,
              uint64_t * foods_eaten);
