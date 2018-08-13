/* dan is the name of the protagonist.
 */

static void draw_or_erase_dan(Display* const dpy, const Window win, const Dude* const dan, const bool erase) {
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


static void draw_dan(Display* const dpy, const Window win, const Dude* const dan) {
    draw_or_erase_dan(dpy, win, dan, false);
}

static void erase_dan(Display* const dpy, const Window win, const Dude* const dan) {
    draw_or_erase_dan(dpy, win, dan, true);
}

static void move_dan(Dude* dan, const Direction dir, Maze* const maze, Display* const dpy, const Window win,
                     uint64_t* const foods_eaten) {
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
