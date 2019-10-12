#pragma once

#include "types.h"

#include <stdbool.h>

gcc_pure bool in_centre_of_tile(const struct dude * const dude);

gcc_pure bool is_on_track(const struct dude * const dude);

gcc_pure bool path_is_clear(const struct dude * const dude, const struct maze * const maze);

bool gcc_pure can_proceed(const struct dude * const dude, const struct maze * const maze);

struct dude new_dude(Display* const dpy, const int screen, const int x, const int y, const enum direction dir,
                     struct maze * const maze, const char* const colour);

gcc_pure uint32_t unsigned_diff(int x, int y);

gcc_pure bool dudes_are_touching(const struct dude * const dan, const struct dude * const ghostie);

gcc_pure bool dan_is_eaten(const struct dude * const dan,
                           const struct dude ghosties[const],
                           const uint8_t num_ghosties);
