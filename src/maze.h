#pragma once

#include "types.h"

#include <assert.h>

/* call initialize_maze to make the maze */
void initialize_maze(struct maze * maze);

void draw_maze(Display* dpy, Window win, const struct maze * maze);
