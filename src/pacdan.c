/* pacman game with xlib */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WALL_LIMIT 66
#define WINDOW_HEIGHT 700 // also window width, because the game is a square
#define CORRIDOR_SIZE 25

Display * display;
Window window;

typedef enum { right, up, left, down } Direction;

typedef struct {
    uint32_t x;
    uint32_t y;
} Point;

typedef struct {
    uint32_t x; // these are the centre of pacman
    uint32_t y;
    uint32_t size; // pacman is a square, this means width and height
    Direction direction;
} Pacman;

/* starting position */
Pacman pacman = {
    .x = CORRIDOR_SIZE,
    .y = CORRIDOR_SIZE,
    .size = 48,
    .direction = right
};

/* don't initialize a Wall except by calling build_wall */
typedef struct {
//     uint32_t x1; // coordinates of one end
//     uint32_t y1;
    Point start;
    Point end;
//     uint32_t x2; // coordinates of the other end
//     uint32_t y2;
} Wall;

/* call build_maze to make the maze */
typedef struct {
    int16_t wall_count; // 255 walls ought to suffice
    Wall walls[WALL_LIMIT];
    bool tiles_blocked[WINDOW_HEIGHT/CORRIDOR_SIZE][WINDOW_HEIGHT/CORRIDOR_SIZE]; // FIXME use a bitfield instead.
} Maze;

Maze maze;

Point get_new_coords(void) {
    Point point = { pacman.x, pacman.y };
    switch (pacman.direction) {
        case right:
            point.x++;
            break;
        case up:
            point.y--;
            break;
        case left:
            point.x--;
            break;
        case down:
            point.y++;
            break;
    }
    return point;
}

bool isOffTrack(void) {
    // returns true if pacman can proceed in this direction
    switch (pacman.direction) {
        case up:
        case down:
            return pacman.x % CORRIDOR_SIZE != 0;
        case left:
        case right:
            return pacman.y % CORRIDOR_SIZE != 0;
    }
    abort();
    return true; // should never happen
}

bool isBorder(Point p) {
    // window borders are essentially walls, but whatever
    if ((p.x < pacman.size / 2+1) || (p.x > WINDOW_HEIGHT - (pacman.size/2+1))) {
        return true;
    } else if ((p.y < pacman.size / 2+1) || (p.y > WINDOW_HEIGHT - (pacman.size/2+1))) {
        return true;
    }
    return false;
}

bool isWall(void) {
    // returns true if pacman is wak-blocked
    assert (pacman.x > 0);
    assert (pacman.y > 0);
    assert (pacman.x < WINDOW_HEIGHT);
    assert (pacman.y < WINDOW_HEIGHT);
    assert (pacman.x % CORRIDOR_SIZE == 0 || pacman.y % CORRIDOR_SIZE == 0);
    if (pacman.x % CORRIDOR_SIZE != 0 || pacman.y % CORRIDOR_SIZE != 0) {
        return false; // walls are only relevant at turning points
    }
    assert (pacman.x % CORRIDOR_SIZE == 0);
    assert (pacman.y % CORRIDOR_SIZE == 0);

    uint32_t x = pacman.x;
    uint32_t y = pacman.y;
    switch (pacman.direction) {
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

    if (maze.tiles_blocked[x/CORRIDOR_SIZE][y/CORRIDOR_SIZE]) {
        return true;
    }
    return false;
}

bool can_proceed(void) {
    Point point = get_new_coords();
    if (isBorder(point)) {
        fprintf(stderr, "can't move through border at %u %u\n", point.x, point.y);
        return false;
    }

    if (isOffTrack()) {
        fprintf(stderr, "not on track at %u %u\n", point.x, point.y);
        return false;
    }

    if (isWall()) {
        fprintf(stderr, "running into a wall at %u %u\n", point.x, point.y);
        return false;
    }
    return true;
}

void build_wall(uint32_t x, uint32_t y, uint32_t length, Direction dir) {
    assert (maze.wall_count < WALL_LIMIT);
    assert (dir == right || dir == up || dir == left || dir == down);

    x = x * CORRIDOR_SIZE;
    y = y * CORRIDOR_SIZE;
    length = length * CORRIDOR_SIZE;

    assert (length > 0);
    assert (length % CORRIDOR_SIZE == 0);
    assert (length + x < WINDOW_HEIGHT || length + y < WINDOW_HEIGHT);

    Wall wall = {
        .start.x = x,
        .start.y = y,
        .end.x = x, // one of these
        .end.y = y, // will be changed
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
            maze.tiles_blocked[wall.start.x/CORRIDOR_SIZE][i] = true;
        }
    } else {
        for (uint32_t i = wall.start.x/CORRIDOR_SIZE; i-1 < wall.end.x/CORRIDOR_SIZE; i++) {
            maze.tiles_blocked[i][wall.end.y/CORRIDOR_SIZE] = true;
        }
    }

    maze.walls[maze.wall_count] = wall;
    maze.wall_count++;
}

void build_maze(void) {
    maze.wall_count = 0;
    memset(maze.tiles_blocked, 0, sizeof(maze.tiles_blocked));

    // the walls are hardcoded, deal with it.
    assert (maze.wall_count == 0);
    assert (WALL_LIMIT == 66);

    // box in the centre
    build_wall(10, 10, 8, right);
    build_wall(10, 10, 8, down);
    build_wall(10, 18, 8, right);
    build_wall(18, 10, 8, down);

    // rest of the maze
    build_wall(2, 2, 4, right);
    build_wall(2, 2, 4, down);
    build_wall(2, 8, 10, right);
    build_wall(2, 10, 4, down);
    build_wall(2, 10, 2, right);
    build_wall(2, 14, 2, right);
    build_wall(2, 16, 4, right);
    build_wall(2, 18, 6, right);
    build_wall(2, 18, 2, down);
    build_wall(2, 22, 4, right);
    build_wall(2, 24, 2, down);
    build_wall(2, 26, 8, right);
    build_wall(4, 4, 4, down);
    build_wall(4, 12, 2, right);
    build_wall(4, 20, 4, down);
    build_wall(6, 4, 6, right);
    build_wall(6, 6, 2, right);
    build_wall(6, 10, 6, down);
    build_wall(6, 18, 2, down);
    build_wall(6, 22, 2, down);
    build_wall(8, 2, 4, down);
    build_wall(8, 10, 8, down);
    build_wall(8, 20, 4, down);
    build_wall(10, 2, 2, down);
    build_wall(10, 6, 2, down);
    build_wall(10, 18, 4, down);
    build_wall(10, 24, 2, down);
    build_wall(12, 2, 4, down);
    build_wall(12, 24, 4, right);
    build_wall(12, 20, 2, down);
    build_wall(12, 24, 2, down);
    build_wall(14, 2, 4, down);
    build_wall(14, 8, 2, down);
    build_wall(14, 18, 2, down);
    build_wall(14, 22, 6, right);
    build_wall(14, 24, 2, down);
    build_wall(16, 2, 4, down);
    build_wall(16, 2, 10, right);
    build_wall(16, 6, 2, right);
    build_wall(16, 8, 2, right);
    build_wall(16, 20, 8, right);
    build_wall(16, 26, 4, right);
    build_wall(18, 4, 6, right);
    build_wall(18, 12, 2, right);
    build_wall(18, 24, 2, down);
    build_wall(20, 6, 4, right);
    build_wall(20, 6, 4, down);
    build_wall(20, 14, 2, right);
    build_wall(20, 16, 4, right);
    build_wall(20, 18, 2, right);
    build_wall(20, 22, 2, down);
    build_wall(22, 8, 4, down);
    build_wall(22, 22, 4, right);
    build_wall(22, 24, 2, down);
    build_wall(24, 8, 2, right);
    build_wall(24, 10, 8, down);
    build_wall(24, 24, 2, right);
    build_wall(24, 26, 2, right);
    build_wall(26, 4, 4, down);
    build_wall(26, 10, 4, down);
    build_wall(26, 16, 4, down);
    build_wall(26, 24, 2, down);

    assert (WALL_LIMIT == maze.wall_count); // couldn't make any more walls if we wanted to
}

void draw_maze(void) {
    assert (WALL_LIMIT == maze.wall_count); // don't try to draw the maze until you've populated it
    XGCValues gcv;
    gcv.background = BlackPixel(display, DefaultScreen(display));
    gcv.foreground = WhitePixel(display, DefaultScreen(display));
    const GC gc = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);
    Wall wall;
    for (uint16_t i = 0; i < WALL_LIMIT; i++) {
        wall = maze.walls[i];
        XDrawLine(display, window, gc, wall.start.x, wall.start.y, wall.end.x, wall.end.y);
    }
}

void draw_or_erase_pacman(bool erase) {
    assert (display != NULL);

    static XGCValues gcv;
    gcv.background = BlackPixel(display, DefaultScreen(display));
    if (erase) {
        gcv.foreground = BlackPixel(display, DefaultScreen(display));
    } else {
        gcv.foreground = WhitePixel(display, DefaultScreen(display));
    }
    const GC gc = XCreateGC(display, DefaultRootWindow(display),
            GCForeground | GCBackground, &gcv);

    assert (pacman.size > 0);
    const uint32_t halfsize = pacman.size / 2;
    const uint16_t mouth_line_length = 20;
    const uint32_t startCircle = (pacman.direction * 90 * 64) + 2500;
    const uint32_t endCircle = 360 * 64 - 5000;
    XDrawArc(display, window, gc,
        pacman.x-halfsize, pacman.y-halfsize, // x and y are in the upper-left corner
        pacman.size, pacman.size, // width and height
        startCircle, endCircle);

    switch (pacman.direction) {
        case right:
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x + mouth_line_length, pacman.y + 15);
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x + mouth_line_length, pacman.y - 15);
            break;
        case up:
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x + 15, pacman.y - mouth_line_length);
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x - 15, pacman.y - mouth_line_length);
            break;
        case left:
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x - mouth_line_length, pacman.y + 15);
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x - mouth_line_length, pacman.y - 15);
            break;
        case down:
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x + 15, pacman.y + mouth_line_length);
            XDrawLine(display, window, gc,
                      pacman.x, pacman.y, pacman.x - 15, pacman.y + mouth_line_length);
            break;
    }
}

void draw_pacman(void) {
    draw_or_erase_pacman(false);
}

void erase_pacman(void) {
    draw_or_erase_pacman(true);
}


void move_pacman(Direction dir) {
    erase_pacman();

    assert (dir == right || dir == up || dir == left || dir == down);
    pacman.direction = dir;

    if (! can_proceed()) {
        draw_pacman(); // pacman doesn't move
        return;
    }

    switch (dir) {
        case right:
            assert (pacman.x < WINDOW_HEIGHT);
            pacman.x += CORRIDOR_SIZE / 5;
            break;
        case up:
            assert (pacman.y > 0);
            pacman.y -= CORRIDOR_SIZE / 5;
            break;
        case left:
            assert (pacman.x > 0);
            pacman.x -= CORRIDOR_SIZE / 5;
            break;
        case down:
            assert (pacman.y < WINDOW_HEIGHT);
            pacman.y += CORRIDOR_SIZE / 5;
            break;
    }
    draw_pacman();
}

void handle_keypress(XEvent event) {
    KeySym keysym = XLookupKeysym(&event.xkey, 0);
    switch (keysym) {
        case XK_Escape:
        case XK_q:
            XDestroyWindow(display, window);
            XCloseDisplay(display);
            display = NULL;
            exit(0);
        case XK_Right:
            move_pacman(right);
            break;
        case XK_Up:
            move_pacman(up);
            break;
        case XK_Left:
            move_pacman(left);
            break;
        case XK_Down:
            move_pacman(down);
            break;
        default:
            fputs("some other key was pressed, who cares.\n", stderr);
    }
}

void draw_grid(void) {
    static XGCValues gcv;
    gcv.background = BlackPixel(display, DefaultScreen(display));
    gcv.foreground = WhitePixel(display, DefaultScreen(display));
    const GC gc = XCreateGC(display, DefaultRootWindow(display), GCForeground | GCBackground, &gcv);

    XWindowAttributes win_attrs;
    int status = XGetWindowAttributes(display, window, &win_attrs);
    assert (status != 0); // docs say this should not be 0

    for (uint8_t i = 0; i < 20; i++) {
        XDrawLine(display, window, gc, i*CORRIDOR_SIZE, 0, i*CORRIDOR_SIZE, win_attrs.height);
    }
    for (uint8_t i = 0; i < 20; i++) {
        XDrawLine(display, window, gc, 0, i*CORRIDOR_SIZE, win_attrs.width, i*CORRIDOR_SIZE);
    }
}

void draw_game(void) {
    draw_maze();
    draw_pacman();
}

int main(void) {
    errno = 0;

    build_maze();

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("no display.", stderr);
        exit(2);
    }

    int screen = DefaultScreen(display);

    XSetWindowAttributes attrs;
    attrs.border_pixel = WhitePixel(display, screen);
    attrs.background_pixel = BlackPixel(display, screen);
    attrs.override_redirect = true;
    attrs.colormap = CopyFromParent;
    attrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask;

    window = XCreateWindow(display, RootWindow(display, screen), 1100, 50, WINDOW_HEIGHT, WINDOW_HEIGHT, 0,
                DefaultDepth(display, screen), InputOutput, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrs);

    XStoreName(display, window, "pacdan");

    XMapWindow(display, window);

    XEvent event;
    while (display != NULL) {
        XNextEvent(display, &event);
        assert(event.type == Expose ||
               event.type == KeyPress ||
               event.type == KeyRelease ||
               event.type == ButtonPress ||
               event.type == ButtonRelease);
        switch (event.type) {
          case Expose:
            puts("Expose detected");
            draw_game();
            break;
          case KeyPress:
            handle_keypress(event);
            break;
          case KeyRelease: // FIXME : prevent the player from holding multiple keys
            break;
          case ButtonPress:
            puts("button pressed, does nothing");
            break;
          case ButtonRelease:
            puts("button released, does nothing");
            break;
          case MappingNotify:
            puts("MappingNotify, don't care");
            break;
          default:
            puts("huh?");
            exit(3);
        }
    }
    return 0;
}
