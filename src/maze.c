// #include "types.h"

static void build_wall(uint32_t x, uint32_t y, uint32_t length, Direction dir, Maze* maze) {
    assert (maze->wall_count < WALL_LIMIT);
    assert (dir == right || dir == up || dir == left || dir == down);
    assert (length > 0);

    x = x * CORRIDOR_SIZE;
    y = y * CORRIDOR_SIZE;
    length = length * CORRIDOR_SIZE;

    assert (length + x < WINDOW_HEIGHT || length + y < WINDOW_HEIGHT);

    Wall wall = {
        // one of these will be changed
        .start.x = x,
        .start.y = y,
        .end.x = x,
        .end.y = y,
    };

    switch (dir) {
        case right:
            assert (x + length < WINDOW_HEIGHT);
            wall.end.x = x+length;
            break;
        case up:
            assert (y - length > 0);
            wall.start.y = y-length;
            break;
        case left:
            assert (x - length > 0);
            wall.start.x = x-length;
            break;
        case down:
            assert (y + length < WINDOW_HEIGHT);
            wall.end.y = y+length;
            break;
    }

    assert (wall.start.x == wall.end.x || wall.start.y == wall.end.y); // no diagonal walls
    assert (wall.start.x < wall.end.x || wall.start.y < wall.end.y); // all walls must go either downward or rightward
    assert (wall.start.x > 0); // make sure the wall doesn't go past the edge of the window
    assert (wall.start.x < WINDOW_HEIGHT);
    assert (wall.start.y > 0);
    assert (wall.start.y < WINDOW_HEIGHT);
    assert (wall.end.x > 0);
    assert (wall.end.x < WINDOW_HEIGHT);
    assert (wall.end.y > 0);
    assert (wall.end.y < WINDOW_HEIGHT);

    if (dir == up || dir == down) {
        for (uint32_t i = wall.start.y/CORRIDOR_SIZE; i-1 < wall.end.y/CORRIDOR_SIZE; i++) {
            maze->tiles_blocked[wall.start.x/CORRIDOR_SIZE][i] = true;
        }
    } else {
        for (uint32_t i = wall.start.x/CORRIDOR_SIZE; i-1 < wall.end.x/CORRIDOR_SIZE; i++) {
            maze->tiles_blocked[i][wall.end.y/CORRIDOR_SIZE] = true;
        }
    }

    maze->walls[maze->wall_count] = wall;
    maze->wall_count++;
}

/* call build_maze to make the maze */
void build_maze(Maze* maze) {
    maze->wall_count = 0;
    memset(maze->tiles_blocked, 0, sizeof(maze->tiles_blocked));

    // the walls are hardcoded, deal with it.
    assert (maze->wall_count == 0);
    assert (WALL_LIMIT == 66);

    // box in the centre
    build_wall(10, 10, 8, right, maze);
    build_wall(10, 10, 8, down, maze);
    build_wall(10, 18, 8, right, maze);
    build_wall(18, 10, 8, down, maze);

    // rest of the maze
    build_wall(2, 2, 4, right, maze);
    build_wall(2, 2, 4, down, maze);
    build_wall(2, 8, 10, right, maze);
    build_wall(2, 10, 4, down, maze);
    build_wall(2, 10, 2, right, maze);
    build_wall(2, 14, 2, right, maze);
    build_wall(2, 16, 4, right, maze);
    build_wall(2, 18, 6, right, maze);
    build_wall(2, 18, 2, down, maze);
    build_wall(2, 22, 4, right, maze);
    build_wall(2, 24, 2, down, maze);
    build_wall(2, 26, 8, right, maze);
    build_wall(4, 4, 4, down, maze);
    build_wall(4, 12, 2, right, maze);
    build_wall(4, 20, 4, down, maze);
    build_wall(6, 4, 6, right, maze);
    build_wall(6, 6, 2, right, maze);
    build_wall(6, 10, 6, down, maze);
    build_wall(6, 18, 2, down, maze);
    build_wall(6, 22, 2, down, maze);
    build_wall(8, 2, 4, down, maze);
    build_wall(8, 10, 8, down, maze);
    build_wall(8, 20, 4, down, maze);
    build_wall(10, 2, 2, down, maze);
    build_wall(10, 6, 2, down, maze);
    build_wall(10, 18, 4, down, maze);
    build_wall(10, 24, 2, down, maze);
    build_wall(12, 2, 4, down, maze);
    build_wall(12, 24, 4, right, maze);
    build_wall(12, 20, 2, down, maze);
    build_wall(12, 24, 2, down, maze);
    build_wall(14, 2, 4, down, maze);
    build_wall(14, 8, 2, down, maze);
    build_wall(14, 18, 2, down, maze);
    build_wall(14, 22, 6, right, maze);
    build_wall(14, 24, 2, down, maze);
    build_wall(16, 2, 4, down, maze);
    build_wall(16, 2, 10, right, maze);
    build_wall(16, 6, 2, right, maze);
    build_wall(16, 8, 2, right, maze);
    build_wall(16, 20, 8, right, maze);
    build_wall(16, 26, 4, right, maze);
    build_wall(18, 4, 6, right, maze);
    build_wall(18, 12, 2, right, maze);
    build_wall(18, 24, 2, down, maze);
    build_wall(20, 6, 4, right, maze);
    build_wall(20, 6, 4, down, maze);
    build_wall(20, 14, 2, right, maze);
    build_wall(20, 16, 4, right, maze);
    build_wall(20, 18, 2, right, maze);
    build_wall(20, 22, 2, down, maze);
    build_wall(22, 8, 4, down, maze);
    build_wall(22, 22, 4, right, maze);
    build_wall(22, 24, 2, down, maze);
    build_wall(24, 8, 2, right, maze);
    build_wall(24, 10, 8, down, maze);
    build_wall(24, 24, 2, right, maze);
    build_wall(24, 26, 2, right, maze);
    build_wall(26, 4, 4, down, maze);
    build_wall(26, 10, 4, down, maze);
    build_wall(26, 16, 4, down, maze);
    build_wall(26, 24, 2, down, maze);

    assert (WALL_LIMIT == maze->wall_count); // couldn't make any more walls if we wanted to
}

void draw_maze(Display* display, Window win, Maze* maze) {
    assert (WALL_LIMIT == maze->wall_count); // don't try to draw the maze until you've populated it
    XGCValues gcv;
    gcv.background = BlackPixel(display, DefaultScreen(display));
    gcv.foreground = WhitePixel(display, DefaultScreen(display));
    const GC gc = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);
    Wall wall;
    for (uint16_t i = 0; i < WALL_LIMIT; i++) {
        wall = maze->walls[i];
        XDrawLine(display, win, gc, wall.start.x, wall.start.y, wall.end.x, wall.end.y);
    }
}

