/* dan is the name of the protagonist.
 */

static void draw_dan(Display * dpy, Window win, Dude* dan) {
    draw_or_erase_dan(dpy, win, dan, false);
}

static void erase_dan(Display * dpy, Window win, Dude* dan) {
    draw_or_erase_dan(dpy, win, dan, true);
}

static void move_dan(Dude* dan, Direction dir, Maze* maze, Display* dpy, Window win, uint64_t* foods_eaten) {
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
