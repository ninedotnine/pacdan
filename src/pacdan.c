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

uint64_t move_dude(Dude* dude, Direction dir, Maze* maze, Display* dpy, Window win) {
    erase_dude(dpy, win, dude);

    assert (dir == right || dir == up || dir == left || dir == down);
    dude->direction = dir;

    if (can_proceed(dude, maze)) {
        switch (dir) {
            case right:
                assert (dude->x < WINDOW_HEIGHT);
                dude->x += CORRIDOR_SIZE / 5;
                break;
            case up:
                assert (dude->y > 0);
                dude->y -= CORRIDOR_SIZE / 5;
                break;
            case left:
                assert (dude->x > 0);
                dude->x -= CORRIDOR_SIZE / 5;
                break;
            case down:
                assert (dude->y < WINDOW_HEIGHT);
                dude->y += CORRIDOR_SIZE / 5;
                break;
        }
    }

    draw_dude(dpy, win, dude);

    if (dude->x % CORRIDOR_SIZE == 0 && dude->y % CORRIDOR_SIZE == 0) {
        if (maze->tiles[dude->x/CORRIDOR_SIZE][dude->y/CORRIDOR_SIZE] == food) {
            puts("ate food");
            maze->tiles[dude->x/CORRIDOR_SIZE][dude->y/CORRIDOR_SIZE] = vacant;
            return 1;
        }
    }
    return 0;
}

uint64_t handle_keypress(XEvent event, Dude* dude, Maze* maze, Display** dpy_p, Window win) {
    KeySym keysym = XLookupKeysym(&event.xkey, 0);
    switch (keysym) {
        case XK_Escape:
        case XK_q:
            XDestroyWindow(*dpy_p, win);
            XCloseDisplay(*dpy_p);
            *dpy_p = NULL;
            break;
        case XK_Right:
            return move_dude(dude, right, maze, *dpy_p, win);
        case XK_Up:
            return move_dude(dude, up, maze, *dpy_p, win);
        case XK_Left:
            return move_dude(dude, left, maze, *dpy_p, win);
        case XK_Down:
            return move_dude(dude, down, maze, *dpy_p, win);
        default:
            fputs("some other key was pressed, who cares.\n", stderr);
    }
    return 0; // no points for you
}

void draw_game(Display* dpy, Window win, Maze* maze, Dude* dude) {
    draw_maze(dpy, win, maze);
    draw_dude(dpy, win, dude);
}

int main(void) {
    errno = 0;

    Maze maze;
    build_maze(&maze);

    /* starting position */
    Dude dude = {
        .x = CORRIDOR_SIZE,
        .y = CORRIDOR_SIZE,
        .size = 48,
        .direction = right
    };

    maze.tiles[dude.x/CORRIDOR_SIZE][dude.y/CORRIDOR_SIZE] = vacant; // FIXME this is a hack


    Display * display = XOpenDisplay(NULL);
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

    Window window = XCreateWindow(display, RootWindow(display, screen), 1100, 50, WINDOW_HEIGHT, WINDOW_HEIGHT, 0,
                DefaultDepth(display, screen), InputOutput, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrs);

    XStoreName(display, window, "pacdan");

    XMapWindow(display, window);

    uint64_t foods_eaten = 0;
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
            draw_game(display, window, &maze, &dude);
            break;
          case KeyPress:
            foods_eaten += handle_keypress(event, &dude, &maze, &display, window);
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
    printf("final score is: %lu.\n", foods_eaten*100); // make the number look bigger, that's what makes games fun
    return 0;
}
