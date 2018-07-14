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
#include "dude.c"

Display * display;
Window window;

/* starting position */
Dude dude = {
    .x = CORRIDOR_SIZE,
    .y = CORRIDOR_SIZE,
    .size = 48,
    .direction = right
};

Maze maze;

void move_dude(Direction dir) {
    erase_dude(display, window, &dude);

    assert (dir == right || dir == up || dir == left || dir == down);
    dude.direction = dir;

    if (! can_proceed(&dude, &maze)) {
        draw_dude(display, window, &dude); // dude doesn't move
        return;
    }

    switch (dir) {
        case right:
            assert (dude.x < WINDOW_HEIGHT);
            dude.x += CORRIDOR_SIZE / 5;
            break;
        case up:
            assert (dude.y > 0);
            dude.y -= CORRIDOR_SIZE / 5;
            break;
        case left:
            assert (dude.x > 0);
            dude.x -= CORRIDOR_SIZE / 5;
            break;
        case down:
            assert (dude.y < WINDOW_HEIGHT);
            dude.y += CORRIDOR_SIZE / 5;
            break;
    }
    draw_dude(display, window, &dude);
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
            move_dude(right);
            break;
        case XK_Up:
            move_dude(up);
            break;
        case XK_Left:
            move_dude(left);
            break;
        case XK_Down:
            move_dude(down);
            break;
        default:
            fputs("some other key was pressed, who cares.\n", stderr);
    }
}

void draw_game(Display* display, Window win, Maze* maze) {
    draw_maze(display, win, maze);
    draw_dude(display, window, &dude);
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
