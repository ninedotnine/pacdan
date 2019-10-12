#pragma once

#include "types.h"

#include <assert.h>

void build_wall(int x, int y, int length, const enum direction dir, struct maze * const maze);

void distribute_food(struct maze * const maze);

/* call build_maze to make the maze */
void initialize_maze(struct maze * const maze);

void draw_maze(Display* const dpy, const Window win, const struct maze * const maze);
