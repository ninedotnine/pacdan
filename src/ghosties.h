#pragma once

#include "types.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <stdbool.h>
#include <stdint.h>
/*  this file contains routines relevant to the ghosties.
 */

void draw_ghostie(Display* const dpy, const Window win, const struct dude * const ghostie);

void move_ghostie(struct dude * const ghostie,
                         const struct maze * const maze,
                         Display* const dpy,
                         const Window win);
