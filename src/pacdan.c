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
            fputs("that key doesn't do anything.\n", stderr);
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

    Dude dude = starting_dude(1, 1, right, &maze);

    Display * display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("no display.\n", stderr);
        exit(2);
    }

    int screen = DefaultScreen(display);

    XWindowAttributes root_attrs;
    if (0 == XGetWindowAttributes(display, RootWindow(display, screen), &root_attrs)) {
        fputs("error getting root window attributes.\n", stderr);
        exit(3);
    }

    if (root_attrs.width < WINDOW_HEIGHT || root_attrs.height < WINDOW_HEIGHT) {
        fputs("display isn't big enough.\n", stderr);
        exit(4);
    }

    XSetWindowAttributes attrs;
    attrs.border_pixel = WhitePixel(display, screen);
    attrs.background_pixel = BlackPixel(display, screen);
    attrs.override_redirect = true;
    attrs.colormap = CopyFromParent;
    attrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask;

    Window window = XCreateWindow(display, RootWindow(display, screen),
                root_attrs.width/2-WINDOW_HEIGHT/2, root_attrs.height/2-WINDOW_HEIGHT/2, WINDOW_HEIGHT, WINDOW_HEIGHT,
                0, DefaultDepth(display, screen), InputOutput, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrs);

    XStoreName(display, window, "pacdan");

    XMapWindow(display, window);

    uint64_t foods_eaten = 0;
    XEvent event;
    while (display != NULL) {
        if (0 == maze.food_count) {
            XCloseDisplay(display);
            display = NULL;
            puts("okay, you win.");
            break;
        }
        XNextEvent(display, &event);
        assert(event.type == Expose ||
               event.type == KeyPress ||
               event.type == KeyRelease ||
               event.type == ButtonPress ||
               event.type == ButtonRelease);
        switch (event.type) {
          case Expose:
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
