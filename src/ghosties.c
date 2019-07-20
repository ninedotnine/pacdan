/*  this file contains routines relevant to the ghosties.
 */

static void draw_or_erase_ghostie(Display* const dpy, const Window win, const Dude* const ghostie, bool erase) {
    assert (dpy != NULL);
    assert (ghostie != NULL);

    GC gc = NULL;
    if (erase) {
        XGCValues gcv = {
            .background = BlackPixel(dpy, DefaultScreen(dpy)),
            .foreground = BlackPixel(dpy, DefaultScreen(dpy))
        };
        gc = XCreateGC(dpy, DefaultRootWindow(dpy), GCForeground | GCBackground, &gcv);
    } else {
        gc = ghostie->gc;
    }


    erase = true;
    assert (ghostie->size > 0);

    const uint32_t halfsize = ghostie->size / 2;
    const uint8_t squiggle_line_length = 8;
    const uint32_t fullCircle = 360 * 64;
    const uint32_t halfCircle = fullCircle / 2;
    const uint32_t eye_height = ghostie->y-12;

    XPoint points[9] = {{
        .x = ghostie->x-halfsize,
        .y = ghostie->y
    }, {
        .x = 0,
        .y = +halfsize
    }, {
        .x = squiggle_line_length,
        .y = -squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = -squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = -squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = squiggle_line_length
    }, {
        .x = 0,
        .y = -halfsize
    }};

    puts("ghostie -- drawin lines");
    XDrawLines(dpy, win, gc, points, 9, CoordModePrevious);

    puts("ghostie -- drawin arcs");
    XDrawArc(dpy, win, gc,
        ghostie->x-halfsize, ghostie->y-halfsize, // x and y are in the upper-left corner
        ghostie->size, ghostie->size, // width and height
        0, halfCircle); // FIXME  why does this function sometimes not return until more inputs?

    puts("ghostie: arc 1");


    if (! erase) {
        puts("drew a arc");
    }

    // draw the eyes
    XDrawArc(dpy, win, gc,
        ghostie->x-15, eye_height, // x and y are in the upper-left corner
        6, 6, // width and height
        0, fullCircle); // FIXME  why does this function sometimes not return until more inputs?

    puts("ghostie: arc 2");

    if (! erase) {
        puts("drew another arc");
    }

    XDrawArc(dpy, win, gc,
        ghostie->x+9, eye_height, // x and y are in the upper-left corner
        6, 6, // width and height
        0, fullCircle);

    puts("ghostie: arc 3");
    if (! erase) {
        puts("drew a third arc");
    }

    // the little irises can face different directions
    switch (ghostie->direction) {
        case right:
            XDrawArc(dpy, win, gc,
                ghostie->x+12, eye_height+2, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            XDrawArc(dpy, win, gc,
                ghostie->x-12, eye_height+2, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            break;
        case up:
            XDrawArc(dpy, win, gc,
                ghostie->x+11, eye_height+1, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            XDrawArc(dpy, win, gc,
                ghostie->x-13, eye_height+1, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            break;
        case left:
            XDrawArc(dpy, win, gc,
                ghostie->x+10, eye_height+2, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            XDrawArc(dpy, win, gc,
                ghostie->x-14, eye_height+2, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            break;
        case down:
            XDrawArc(dpy, win, gc,
                ghostie->x+11, eye_height+3, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            XDrawArc(dpy, win, gc,
                ghostie->x-13, eye_height+3, // x and y are in the upper-left corner
                2, 2, // width and height
                0, fullCircle);
            break;
    }
    puts("ghostie -- all lines drawn.");
}

static void draw_ghostie(Display* const dpy, const Window win, const Dude* const ghostie) {
    puts("called draw_ghostie");
    draw_or_erase_ghostie(dpy, win, ghostie, false);
//     puts("finished draw_ghostie");
}

static void erase_ghostie(Display* const dpy, const Window win, const Dude* const ghostie) {
    draw_or_erase_ghostie(dpy, win, ghostie, true);
}

static void turn_around(Dude* const ghostie, const Tile neighbours[const static 4]) {
    for (uint8_t i = 0; i < 4; i++) {
        if (neighbours[i] != blocked) {
            ghostie->direction = i;
            return;
        }
    }
}

static void proceed_forward(Dude* const ghostie, const Tile neighbours[const static 4]) {
    const Direction came_from = (ghostie->direction + 2) % 4;
    for (uint8_t i = 0; i < 4; i++) {
        if (neighbours[i] != blocked && i != came_from) {
            ghostie->direction = i;
            return;
        }
    }
}

static void choose_fork(Dude* const ghostie, const Tile neighbours[const static 4], uint8_t options) {
    const Direction came_from = (ghostie->direction + 2) % 4;
    uint32_t randy = rand(); // FIXME better random numbers?
    options--;
    for (uint8_t i = 0; i < 4; i++) {
        if (neighbours[i] != blocked && i != came_from) {
            if (randy % options == 0) {
                ghostie->direction = i;
                return;
            } else {
                options--;
            }
        }
    }
}

void ghostie_set_direction(Dude* const ghostie, const Maze* const maze) {
    assert ((ghostie->x % CORRIDOR_SIZE == 0 && ghostie->y % CORRIDOR_SIZE == 0));
    const uint32_t x = ghostie->x / CORRIDOR_SIZE;
    const uint32_t y = ghostie->y / CORRIDOR_SIZE;
    assert ((x < TILES_HEIGHT && y < TILES_HEIGHT));

    const Tile neighbours[4] = {
        maze->tiles[x+1][y],
        maze->tiles[x][y-1],
        maze->tiles[x-1][y],
        maze->tiles[x][y+1]
    };

    uint8_t options = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (neighbours[i] != blocked) {
            options++;
        }
    }

    assert (options > 0 && options < 5);
    if (options == 1) {
        turn_around(ghostie, neighbours);
    }

    if (options == 2) {
        proceed_forward(ghostie, neighbours);
    }

    if (options > 2) {
        choose_fork(ghostie, neighbours, options);
    }
}

static void move_ghostie(Dude* const ghostie, const Maze* const maze, Display* const dpy, const Window win) {
    puts("erasin ghostie");
    erase_ghostie(dpy, win, ghostie);
    puts("movin ghostie");

    assert (ghostie->direction == right ||
            ghostie->direction == up ||
            ghostie->direction == left ||
            ghostie->direction == down);

    if (in_centre_of_tile(ghostie)) {
        ghostie_set_direction(ghostie, maze);
    }

    assert (can_proceed(ghostie, maze));
    switch (ghostie->direction) {
        case right:
            assert (ghostie->x < WINDOW_HEIGHT);
            ghostie->x += CORRIDOR_SIZE / 5;
            break;
        case up:
            assert (ghostie->y > 0);
            ghostie->y -= CORRIDOR_SIZE / 5;
            break;
        case left:
            assert (ghostie->x > 0);
            ghostie->x -= CORRIDOR_SIZE / 5;
            break;
        case down:
            assert (ghostie->y < WINDOW_HEIGHT);
            ghostie->y += CORRIDOR_SIZE / 5;
            break;
    }

    draw_ghostie(dpy, win, ghostie);
}
