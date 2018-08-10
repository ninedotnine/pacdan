static void draw_or_erase_dude(Display* dpy, Window win, Dude* dude, bool erase) {
    assert (dpy != NULL);

    static XGCValues gcv;
    gcv.background = BlackPixel(dpy, DefaultScreen(dpy));
    if (erase) {
        gcv.foreground = BlackPixel(dpy, DefaultScreen(dpy));
    } else {
        gcv.foreground = WhitePixel(dpy, DefaultScreen(dpy));
    }
    const GC gc = XCreateGC(dpy, DefaultRootWindow(dpy),
            GCForeground | GCBackground, &gcv);

    assert (dude->size > 0);
    const uint32_t halfsize = dude->size / 2;
    const uint8_t mouth_line_length = 20;
    const uint32_t startCircle = (dude->direction * 90 * 64) + 2500;
    const uint32_t endCircle = 360 * 64 - 5000;
    XDrawPoint(dpy, win, gc, dude->x, dude->y);
    XDrawArc(dpy, win, gc,
        dude->x-halfsize, dude->y-halfsize, // x and y are in the upper-left corner
        dude->size, dude->size, // width and height
        startCircle, endCircle);

    switch (dude->direction) {
        case right:
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x + mouth_line_length, dude->y + 15);
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x + mouth_line_length, dude->y - 15);
            break;
        case up:
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x + 15, dude->y - mouth_line_length);
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x - 15, dude->y - mouth_line_length);
            break;
        case left:
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x - mouth_line_length, dude->y + 15);
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x - mouth_line_length, dude->y - 15);
            break;
        case down:
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x + 15, dude->y + mouth_line_length);
            XDrawLine(dpy, win, gc, dude->x, dude->y, dude->x - 15, dude->y + mouth_line_length);
            break;
    }
}

void draw_dude(Display * dpy, Window win, Dude* dude) {
    draw_or_erase_dude(dpy, win, dude, false);
}

void erase_dude(Display * dpy, Window win, Dude* dude) {
    draw_or_erase_dude(dpy, win, dude, true);
}

void move_dude(Dude* dude, Direction dir, Maze* maze, Display* dpy, Window win) {
    erase_dude(dpy, win, dude);

    assert (dir == right || dir == up || dir == left || dir == down);
    dude->direction = dir;

    if (can_proceed(dude, maze)) {
        switch (dir) {
            case right:
                assert (dude->x < WINDOW_HEIGHT);
                dude->x += CORRIDOR_SIZE / 5;
                break;
            case up:
                assert (dude->y > 0);
                dude->y -= CORRIDOR_SIZE / 5;
                break;
            case left:
                assert (dude->x > 0);
                dude->x -= CORRIDOR_SIZE / 5;
                break;
            case down:
                assert (dude->y < WINDOW_HEIGHT);
                dude->y += CORRIDOR_SIZE / 5;
                break;
        }
    }

    draw_dude(dpy, win, dude);

    if (dude->x % CORRIDOR_SIZE == 0 && dude->y % CORRIDOR_SIZE == 0) {
        if (maze->tiles[dude->x/CORRIDOR_SIZE][dude->y/CORRIDOR_SIZE] == food) {
            maze->tiles[dude->x/CORRIDOR_SIZE][dude->y/CORRIDOR_SIZE] = vacant;
            maze->food_count--; // FIXME remove this?
            dude->foods_eaten++;
        }
    }
}

Dude initialize_dude(uint32_t x, uint32_t y, Direction dir, Maze* maze) {
    /* gives the starting position */
    Dude dude = {
        .x = x * CORRIDOR_SIZE,
        .y = y * CORRIDOR_SIZE,
        .size = 48,
        .direction = dir,
        .foods_eaten = 0
    };

    maze->tiles[dude.x/CORRIDOR_SIZE][dude.y/CORRIDOR_SIZE] = vacant;
    maze->food_count--;
    return dude;
}
