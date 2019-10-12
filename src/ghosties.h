#pragma once

#include "types.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <stdbool.h>
#include <stdint.h>
/*  this file contains routines relevant to the ghosties.
 */

void draw_or_erase_ghostie(Display* const dpy, const Window win, const struct dude * const ghostie, bool erase);

void draw_ghostie(Display* const dpy, const Window win, const struct dude * const ghostie);

void erase_ghostie(Display* const dpy, const Window win, const struct dude * const ghostie);

void turn_around(struct dude * const ghostie, const enum tile neighbours[const static 4]);

void proceed_forward(struct dude * const ghostie, const enum tile neighbours[const static 4]);

void choose_fork(struct dude * const ghostie, const enum tile neighbours[const static 4], uint8_t options);

void ghostie_set_direction(struct dude * const ghostie, const struct maze * const maze);

void move_ghostie(struct dude * const ghostie,
                         const struct maze * const maze,
                         Display* const dpy,
                         const Window win);
