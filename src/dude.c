/* this file contains functions and routines applicable to both dan and the ghosties.
 */
static gcc_pure bool in_centre_of_tile(Dude* dude) {
    return (dude->x % CORRIDOR_SIZE == 0 && dude->y % CORRIDOR_SIZE == 0);
}

static gcc_pure bool isOnTrack(Dude* dude) {
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

static gcc_pure bool isNotBlocked(Dude* dude, Maze* maze) {
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

static bool gcc_pure can_proceed(Dude* dude, Maze* maze) {
    return isOnTrack(dude) && isNotBlocked(dude, maze);
}

static void draw_or_erase_dan(Display* dpy, Window win, Dude* dan, bool erase) {
    assert (dpy != NULL);
    assert (dan != NULL);

    GC gc = NULL;
    if (erase) {
        XGCValues gcv = {
            .background = BlackPixel(dpy, DefaultScreen(dpy)),
            .foreground = BlackPixel(dpy, DefaultScreen(dpy))
        };
        gc = XCreateGC(dpy, DefaultRootWindow(dpy),
                GCForeground | GCBackground, &gcv);
    } else {
        gc = dan->gc;
    }

    assert (dan->size > 0);
    const uint32_t halfsize = dan->size / 2;
    const uint8_t mouth_line_length = 20;
    const uint32_t startCircle = (dan->direction * 90 * 64) + 2500;
    const uint32_t endCircle = 360 * 64 - 5000;
    XDrawPoint(dpy, win, gc, dan->x, dan->y);
    XDrawArc(dpy, win, gc,
        dan->x-halfsize, dan->y-halfsize, // x and y are in the upper-left corner
        dan->size, dan->size, // width and height
        startCircle, endCircle);

    switch (dan->direction) {
        case right:
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x + mouth_line_length, dan->y + 15);
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x + mouth_line_length, dan->y - 15);
            break;
        case up:
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x + 15, dan->y - mouth_line_length);
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x - 15, dan->y - mouth_line_length);
            break;
        case left:
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x - mouth_line_length, dan->y + 15);
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x - mouth_line_length, dan->y - 15);
            break;
        case down:
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x + 15, dan->y + mouth_line_length);
            XDrawLine(dpy, win, gc, dan->x, dan->y, dan->x - 15, dan->y + mouth_line_length);
            break;
    }
}

static Dude new_dude(Display* dpy, int screen, uint32_t x, uint32_t y, Direction dir, Maze* maze, char* colour) {
    /* gives the starting position */
    Colormap colourmap = DefaultColormap(dpy, screen);

    XColor dude_colour;
    XParseColor(dpy, colourmap, colour, &dude_colour);
    XAllocColor(dpy, colourmap, &dude_colour);

    XGCValues gcv_dude = {
        .foreground = dude_colour.pixel
    };

    GC gc_dude = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_dude);

    Dude dude = {
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
