static void draw_or_erase_ghostie(Display* dpy, Window win, Dude* ghostie, bool erase) {
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

    assert (ghostie->size > 0);
    const uint32_t halfsize = ghostie->size / 2;
    const uint8_t squiggle_line_length = 8;
    const uint32_t fullCircle = 360 * 64;
    const uint32_t halfCircle = fullCircle / 2;
    const uint32_t eye_height = ghostie->y-12;
//     XDrawPoint(dpy, win, gc, ghostie->x, ghostie->y); // middle of the ghostie, a nose maybe

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

    XDrawLines(dpy, win, gc, points, 9, CoordModePrevious);

    XDrawArc(dpy, win, gc,
        ghostie->x-halfsize, ghostie->y-halfsize, // x and y are in the upper-left corner
        ghostie->size, ghostie->size, // width and height
        0, halfCircle);

    // draw the eyes
    XDrawArc(dpy, win, gc,
        ghostie->x-15, eye_height, // x and y are in the upper-left corner
        6, 6, // width and height
        0, fullCircle);

    XDrawArc(dpy, win, gc,
        ghostie->x+9, eye_height, // x and y are in the upper-left corner
        6, 6, // width and height
        0, fullCircle);

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
}

void draw_ghostie(Display * dpy, Window win, Dude* ghostie) {
    draw_or_erase_ghostie(dpy, win, ghostie, false);
}

void erase_ghostie(Display * dpy, Window win, Dude* ghostie) {
    draw_or_erase_ghostie(dpy, win, ghostie, true);


}

void turn_around(Dude* ghostie, Tile neighbours[static 4]) {
    for (uint8_t i = 0; i < 4; i++) {
        if (neighbours[i] != blocked) {
            ghostie->direction = i;
            return;
        }
    }
}

void proceed_forward(Dude* ghostie, Tile neighbours[static 4]) {
    Direction came_from = (ghostie->direction + 2) % 4;
    for (uint8_t i = 0; i < 4; i++) {
        if (neighbours[i] != blocked && i != came_from) {
            ghostie->direction = i;
            return;
        }
    }
}

void choose_fork(Dude* ghostie, Tile neighbours[static 4], uint8_t options) {
    Direction came_from = (ghostie->direction + 2) % 4;
    uint32_t randy = rand(); // FIXME better random numbers?
    options--;
    for (uint8_t i = 0; i < 4; i++) {
        if (neighbours[i] != blocked && i != came_from) {
            if (randy % (options) == 0) {
                ghostie->direction = i;
                return;
            } else {
                options--;
            }
        }
    }
}

void ghostie_set_direction(Dude* ghostie, Maze* maze) {
    assert ((ghostie->x % CORRIDOR_SIZE == 0 && ghostie->y % CORRIDOR_SIZE == 0));
    uint32_t x = ghostie->x / CORRIDOR_SIZE;
    uint32_t y = ghostie->y / CORRIDOR_SIZE;
    assert ((x < TILES_HEIGHT && y < TILES_HEIGHT));

    Tile neighbours[4] = {
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

void move_ghostie(Dude* ghostie, Maze* maze, Display* dpy, Window win) {
    erase_ghostie(dpy, win, ghostie);

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
