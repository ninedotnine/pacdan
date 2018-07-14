// #include "types.h"

#define gcc_pure __attribute__((pure))

static gcc_pure bool isBorder(Pacman* pacman) {
    // window borders are essentially walls, but whatever
    assert (pacman->direction == right ||
            pacman->direction == up    ||
            pacman->direction == left  ||
            pacman->direction == down);
    switch (pacman->direction) {
        case right:
            return (pacman->x+1 > WINDOW_HEIGHT - (pacman->size/2+1));
        case up:
            return (pacman->y-1 < pacman->size / 2+1);
        case left:
            return (pacman->x-1 < pacman->size / 2+1);
        case down:
            return (pacman->y+1 > WINDOW_HEIGHT - (pacman->size/2+1));
    }
    return false; // should never happen
}

static gcc_pure bool isOffTrack(Pacman* pacman) {
    // returns true if pacman can proceed in this direction
    switch (pacman->direction) {
        case up:
        case down:
            return pacman->x % CORRIDOR_SIZE != 0;
        case left:
        case right:
            return pacman->y % CORRIDOR_SIZE != 0;
    }
    abort();
    return true; // should never happen
}

static gcc_pure bool isWall(Pacman* pacman, Maze* maze) {
    // returns true if pacman is wak-blocked
    assert (pacman->x > 0);
    assert (pacman->y > 0);
    assert (pacman->x < WINDOW_HEIGHT);
    assert (pacman->y < WINDOW_HEIGHT);
    assert (pacman->x % CORRIDOR_SIZE == 0 || pacman->y % CORRIDOR_SIZE == 0);
    if (pacman->x % CORRIDOR_SIZE != 0 || pacman->y % CORRIDOR_SIZE != 0) {
        return false; // walls are only relevant at turning points
    }
    assert (pacman->x % CORRIDOR_SIZE == 0);
    assert (pacman->y % CORRIDOR_SIZE == 0);

    uint32_t x = pacman->x;
    uint32_t y = pacman->y;
    switch (pacman->direction) {
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

    if (maze->tiles_blocked[x/CORRIDOR_SIZE][y/CORRIDOR_SIZE]) {
        return true;
    }
    return false;
}

bool can_proceed(Pacman* pacman, Maze* maze) {
    if (isBorder(pacman)) {
        fprintf(stderr, "can't move through border at %u %u\n", pacman->x, pacman->y);
        return false;
    }

    if (isOffTrack(pacman)) {
        fprintf(stderr, "not on track at %u %u\n", pacman->x, pacman->y);
        return false;
    }

    if (isWall(pacman, maze)) {
        fprintf(stderr, "running into a wall at %u %u\n", pacman->x, pacman->y);
        return false;
    }
    return true;
}

