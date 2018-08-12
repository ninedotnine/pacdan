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

static gcc_pure bool ghostie_isOnTrack(Ghostie* ghostie) {
    // returns true if ghostie can proceed in this direction
    assert (ghostie->direction == right || ghostie->direction == up
            || ghostie->direction == left || ghostie->direction == down);
    switch (ghostie->direction) {
        case up:
        case down:
            return ghostie->x % CORRIDOR_SIZE == 0;
        case left:
        case right:
            return ghostie->y % CORRIDOR_SIZE == 0;
    }
    abort();
    return false; // should never happen
}

static gcc_pure bool ghostie_in_centre_of_tile(Ghostie* ghostie) {
    return (ghostie->x % CORRIDOR_SIZE == 0 && ghostie->y % CORRIDOR_SIZE == 0);
}

static gcc_pure bool ghostie_isNotBlocked(Ghostie* ghostie, Maze* maze) {
    // returns true if ghostie is not wak-blocked
    assert (ghostie->x > 0);
    assert (ghostie->y > 0);
    assert (ghostie->x < WINDOW_HEIGHT);
    assert (ghostie->y < WINDOW_HEIGHT);
    assert (ghostie->x % CORRIDOR_SIZE == 0 || ghostie->y % CORRIDOR_SIZE == 0);
//     if (ghostie->x % CORRIDOR_SIZE != 0 || ghostie->y % CORRIDOR_SIZE != 0) {
    if (! ghostie_in_centre_of_tile(ghostie)) {
        return true; // walls are only relevant at turning points
    }

    assert (ghostie->x % CORRIDOR_SIZE == 0);
    assert (ghostie->y % CORRIDOR_SIZE == 0);

    uint32_t x = ghostie->x;
    uint32_t y = ghostie->y;
    switch (ghostie->direction) {
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

bool gcc_pure ghostie_can_proceed(Ghostie* ghostie, Maze* maze) {
    return ghostie_isOnTrack(ghostie) && ghostie_isNotBlocked(ghostie, maze);
}

