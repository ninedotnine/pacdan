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

void draw_dan(Display * dpy, Window win, Dude* dan) {
    draw_or_erase_dan(dpy, win, dan, false);
}

void erase_dan(Display * dpy, Window win, Dude* dan) {
    draw_or_erase_dan(dpy, win, dan, true);
}

void move_dan(Dude* dan, Direction dir, Maze* maze, Display* dpy, Window win, uint64_t* foods_eaten) {
    erase_dan(dpy, win, dan);

    assert (dir == right || dir == up || dir == left || dir == down);
    dan->direction = dir;

    if (can_proceed(dan, maze)) {
        switch (dir) {
            case right:
                assert (dan->x < WINDOW_HEIGHT);
                dan->x += CORRIDOR_SIZE / 5;
                break;
            case up:
                assert (dan->y > 0);
                dan->y -= CORRIDOR_SIZE / 5;
                break;
            case left:
                assert (dan->x > 0);
                dan->x -= CORRIDOR_SIZE / 5;
                break;
            case down:
                assert (dan->y < WINDOW_HEIGHT);
                dan->y += CORRIDOR_SIZE / 5;
                break;
        }
    }

    draw_dan(dpy, win, dan);

    if (dan->x % CORRIDOR_SIZE == 0 && dan->y % CORRIDOR_SIZE == 0) {
        if (maze->tiles[dan->x/CORRIDOR_SIZE][dan->y/CORRIDOR_SIZE] == food) {
            maze->tiles[dan->x/CORRIDOR_SIZE][dan->y/CORRIDOR_SIZE] = vacant;
            maze->food_count--; // FIXME remove this?
            (*foods_eaten)++;
        }
    }
}

Dude new_dude(Display* dpy, int screen, uint32_t x, uint32_t y, Direction dir, Maze* maze, char* colour) {
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
