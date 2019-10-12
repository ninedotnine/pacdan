#pragma once

#include "types.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <stdbool.h>
#include <stdint.h>
/*  this file contains routines relevant to the ghosties.
 */

void draw_ghostie(Display * dpy, Window win, const struct dude * ghostie);

void move_ghostie(struct dude * ghostie, const struct maze * maze, Display * dpy, Window win);
