/* this file contains functions and routines applicable to both dan and the ghosties.
 */
static gcc_pure bool in_centre_of_tile(const Dude* const dude) {
    return (dude->x % CORRIDOR_SIZE == 0 && dude->y % CORRIDOR_SIZE == 0);
}

static gcc_pure bool isOnTrack(const Dude* const dude) {
    // returns true if dude can proceed in this direction
    assert (dude->direction == right || dude->direction == up || dude->direction == left || dude->direction == down);
    switch (dude->direction) {
        case up:
        case down:
            return dude->x % CORRIDOR_SIZE == 0;
        case left:
        case right:
            return dude->y % CORRIDOR_SIZE == 0;
    }
    abort();
    return false; // should never happen
}

static gcc_pure bool isNotBlocked(const Dude* const dude, const Maze* const maze) {
    // returns true if dude is not wak-blocked
    assert (dude->x > 0);
    assert (dude->y > 0);
    assert (dude->x < WINDOW_HEIGHT);
    assert (dude->y < WINDOW_HEIGHT);
    assert (dude->x % CORRIDOR_SIZE == 0 || dude->y % CORRIDOR_SIZE == 0);
//     if (dude->x % CORRIDOR_SIZE != 0 || dude->y % CORRIDOR_SIZE != 0) {
    if (! in_centre_of_tile(dude)) {
        return true; // walls are only relevant at turning points
    }

    assert (dude->x % CORRIDOR_SIZE == 0);
    assert (dude->y % CORRIDOR_SIZE == 0);

    uint32_t x = dude->x;
    uint32_t y = dude->y;
    switch (dude->direction) {
        case right:
            x += CORRIDOR_SIZE;
            break;
        case up:
            y -= CORRIDOR_SIZE;
            break;
        case down:
            y += CORRIDOR_SIZE;
            break;
        case left:
            x -= CORRIDOR_SIZE;
            break;
    }
    return (maze->tiles[x/CORRIDOR_SIZE][y/CORRIDOR_SIZE] != blocked);
}

static bool gcc_pure can_proceed(const Dude* const dude, const Maze* const maze) {
    return isOnTrack(dude) && isNotBlocked(dude, maze);
}

static Dude new_dude(Display* const dpy, const int screen, const uint32_t x, const uint32_t y, const Direction dir,
                     Maze* const maze, const char* const colour) {
    /* gives the starting position */
    const Colormap colourmap = DefaultColormap(dpy, screen);

    XColor dude_colour;
    XParseColor(dpy, colourmap, colour, &dude_colour);
    XAllocColor(dpy, colourmap, &dude_colour);

    XGCValues gcv_dude = {
        .foreground = dude_colour.pixel
    };

    const GC gc_dude = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_dude);

    const Dude dude = {
        .x = x * CORRIDOR_SIZE,
        .y = y * CORRIDOR_SIZE,
        .size = 48,
        .direction = dir,
        .gc = gc_dude
    };

    if (maze->tiles[dude.x/CORRIDOR_SIZE][dude.y/CORRIDOR_SIZE] == food) {
        maze->tiles[dude.x/CORRIDOR_SIZE][dude.y/CORRIDOR_SIZE] = vacant;
        maze->food_count--;
    }
    return dude;
}

static gcc_pure bool dudes_are_touching(const Dude* const dan, const Dude* const ghostie) {
    if (dan->x == ghostie->x) {
        return abs(dan->y - ghostie->y) < (CORRIDOR_SIZE-3)*2;
    } else if (dan->y == ghostie->y) {
        return abs(dan->x - ghostie->x) < (CORRIDOR_SIZE-3)*2;
    } else {
        return ((abs(dan->x - ghostie->x) < (CORRIDOR_SIZE-5)*2) && (abs(dan->y - ghostie->y) < (CORRIDOR_SIZE-5)*2));
    }
}

static gcc_pure bool dan_is_eaten(const Dude* const dan, const Dude ghosties[const], const uint8_t num_ghosties) {
    for (uint8_t i = 0; i < num_ghosties; i++) {
        if (dudes_are_touching(dan, &ghosties[i])) {
            return true;
        }
    }
    return false;
}
