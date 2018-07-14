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

#include "types.h"
#include "movements.c"
#include "maze.c"

Display * display;
Window window;

/* starting position */
Pacman pacman = {
    .x = CORRIDOR_SIZE,
    .y = CORRIDOR_SIZE,
    .size = 48,
    .direction = right
};

Maze maze;

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

    if (! can_proceed(&pacman, &maze)) {
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

void draw_game(Display* display, Window win, Maze* maze) {
    draw_maze(display, win, maze);
    draw_pacman();
}

int main(void) {
    errno = 0;

    build_maze(&maze);

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
            draw_game(display, window, &maze);
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
