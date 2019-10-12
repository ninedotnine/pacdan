#pragma once

#include "types.h"

#include <stdbool.h>

gcc_pure bool in_centre_of_tile(const struct dude * dude);

bool gcc_pure can_proceed(const struct dude * dude, const struct maze * maze);

struct dude new_dude(Display * dpy, int screen, int x, int y, enum direction dir,
                     struct maze * maze, const char * colour);

gcc_pure bool dan_is_eaten(const struct dude * dan,
                           const struct dude ghosties[const],
                           uint8_t num_ghosties);
