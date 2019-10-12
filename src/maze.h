#pragma once

#include "types.h"

#include <assert.h>

/* call initialize_maze to make the maze */
void initialize_maze(struct maze * const maze);

void draw_maze(Display* const dpy, const Window win, const struct maze * const maze);
