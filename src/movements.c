// #include "types.h"

#define gcc_pure __attribute__((pure))

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
    if (dude->x % CORRIDOR_SIZE != 0 || dude->y % CORRIDOR_SIZE != 0) {
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

bool gcc_pure can_proceed(Dude* dude, Maze* maze) {
    return isOnTrack(dude) && isNotBlocked(dude, maze);
}

