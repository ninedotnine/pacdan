/* pacman game with xlib */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "types.h"
#include "movements.c"
#include "maze.c"
#include "dude.c"
#include "ghosties.c"
#include "threading.c"
#include "controls.c"
#include "centre_box.c"


void draw_game(Display* dpy, Window win, Maze* maze, Dude* dude, Ghostie ghosties[], uint8_t num_ghosties) {
//     XLockDisplay(dpy);
    draw_maze(dpy, win, maze);
    draw_dude(dpy, win, dude);
    for (uint8_t i = 0; i < num_ghosties; i++) {
        draw_ghostie(dpy, win, &ghosties[i]);
    }
//     XUnlockDisplay(dpy);
}

int main(void) {
//     mutex = (void *) 0;
    pthread_mutex_init(&mutex, NULL);

    if (0 == XInitThreads()) {
        fputs("not able to initialize multithreading.", stderr);
    }

    Directions dirs = { false, false, false, false };

    Maze maze;
    build_maze(&maze);

    Dude dude = initialize_dude(1, 1, right, &maze);

    Display * display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("no display.\n", stderr);
        exit(EXIT_FAILURE);
    }

    int screen = DefaultScreen(display);

    uint32_t num_ghosties = 7;
    Ghostie ghosties[num_ghosties];
    ghosties[0] = new_ghostie(display, screen, 1, 27, right, &maze, "rgb:fa/aa/ab");
    ghosties[1] = new_ghostie(display, screen, 27, 27, up, &maze, "rgb:33/99/cc");
    ghosties[2] = new_ghostie(display, screen, 27, 1, down, &maze, "rgb:99/33/ff");
    ghosties[3] = new_ghostie(display, screen, 19, 9, down, &maze, "rgb:11/11/ee");
    ghosties[4] = new_ghostie(display, screen, 9, 19, down, &maze, "rgb:ee/11/11");
//     ghosties[5] = new_ghostie(display, screen, 9, 9, down, &maze, "rgb:cc/ee/11"); // this will be the dude's colour
    ghosties[5] = new_ghostie(display, screen, 9, 9, down, &maze, "rgb:11/ee/11");
    ghosties[6] = new_ghostie(display, screen, 19, 19, down, &maze, "rgb:ee/11/ee");

    XWindowAttributes root_attrs;
    if (0 == XGetWindowAttributes(display, RootWindow(display, screen), &root_attrs)) {
        fputs("error getting root window attributes.\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (root_attrs.width < WINDOW_HEIGHT || root_attrs.height < WINDOW_HEIGHT) {
        fputs("display isn't big enough.\n", stderr);
        exit(EXIT_FAILURE);
    }

    XSetWindowAttributes attrs;
    attrs.border_pixel = WhitePixel(display, screen);
    attrs.background_pixel = BlackPixel(display, screen);
    attrs.override_redirect = true;
    attrs.colormap = CopyFromParent;
    attrs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask;
    // FIXME use FocusChangeMask to automatically pause

    Window window = XCreateWindow(display, RootWindow(display, screen),
                root_attrs.width/2-WINDOW_HEIGHT/2, root_attrs.height/2-WINDOW_HEIGHT/2,
                WINDOW_HEIGHT, WINDOW_HEIGHT,
                0, DefaultDepth(display, screen), InputOutput, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrs);

    XStoreName(display, window, "pacdan");

    XMapWindow(display, window);

    Window centre_win = XCreateWindow(display, window, 252, 252, 195, 195,
                0, CopyFromParent, CopyFromParent, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel, &attrs);
    XMapWindow(display, centre_win);

    GC gc_fab = NULL;
    XFontStruct* font = NULL;
    initialize_font_and_colours(display, screen, &font, &gc_fab);

    Controls_thread_data data = new_thread_data(&display, &window, &dirs);

    pthread_t controls;
    if (0 != pthread_create(&controls, NULL, handle_xevents, &data)) {
        fputs("could not create thread.\n", stderr);
        exit(EXIT_FAILURE);
    }

    const struct timespec tim = {.tv_sec = 0, .tv_nsec = 50000000L};
    while (game_in_progress(&data)) {
        assert (maze.food_count + dude.foods_eaten == 388 - num_ghosties);
        draw_game(display, window, &maze, &dude, ghosties, num_ghosties);
        update_score(display, centre_win, gc_fab, font, dude.foods_eaten); // FIXME only update if necessary
        XFlush(display);

        if (game_is_paused(&data)) {
            game_paused(display, centre_win, gc_fab, font, dude.foods_eaten == 0);
            thread_wait();
        } else if (0 == maze.food_count) {
            congratulate(display, centre_win, gc_fab, font);
            break;
        }

        nanosleep(&tim, NULL);

        thread_lock();
        if (dirs.right) {
            move_dude(&dude, right, &maze, display, window);
        } else if (dirs.up) {
            move_dude(&dude, up, &maze, display, window);
        } else if (dirs.left) {
            move_dude(&dude, left, &maze, display, window);
        } else if (dirs.down) {
            move_dude(&dude, down, &maze, display, window);
        }
        for (uint8_t i = 0; i < num_ghosties; i++) {
            move_ghostie(&ghosties[i], &maze, display, window);
        }
        thread_unlock();
    }
    printf("final score is: %lu.\n", dude.foods_eaten*100); // make the score bigger, that's what makes games fun
    pthread_join(controls, NULL); // quit for player to hit q or escape
    XUnloadFont(display, font->fid);
    XDestroyWindow(display, centre_win);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
