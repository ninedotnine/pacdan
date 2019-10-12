/* pacman game with xlib */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include "types.h"
#include "maze.c"
#include "dude.c"
#include "dan.c"
#include "ghosties.c"
#include "threading.c"
#include "controls.c"
#include "centre_box.c"


static void draw_game(Display * const dpy,
                      const Window win,
                      const struct maze * const maze,
                      const struct dude * const dan,
                      const struct dude ghosties[const],
                      const uint8_t num_ghosties) {
    assert (dpy != NULL);
    assert (maze != NULL);
    assert (dan != NULL);
    assert (ghosties != NULL);
    assert (num_ghosties > 0);

//     printf("x events: %d\n", XEventsQueued(dpy, QueuedAlready));

    XFlush(dpy);
    XLockDisplay(dpy); // this seems to prevent things from breaking even more after the ghosties decide to stop moving
    puts("drawin maze");
    draw_maze(dpy, win, maze);
    puts("drawin dan");
    draw_dan(dpy, win, dan);
    for (uint8_t i = 0; i < num_ghosties; i++) {
        printf("drawin ghostie %u\n", i);
        draw_ghostie(dpy, win, &ghosties[i]);
    }
    puts("done drawin");
//     XUnlockDisplay(dpy);
    XUnlockDisplay(dpy);
}

int main(void) {
    pthread_mutex_init(&mutex, NULL);

    if (0 == XInitThreads()) {
        fputs("not able to initialize multithreading.", stderr);
        exit(EXIT_FAILURE);
    }

    struct directions dirs = { false, false, false, false };

    struct maze maze;
    initialize_maze(&maze);

    Display* const display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("no display.\n", stderr);
        exit(EXIT_FAILURE);
    }

    const int screen = DefaultScreen(display);

    struct dude dan = new_dude(display, screen, 1, 1, right, &maze, "rgb:cc/ee/11");

    const uint32_t num_ghosties = 7;
//     const uint32_t num_ghosties = 1;
    struct dude ghosties[num_ghosties];
//     ghosties[0] = new_dude(display, screen, 27, 27, right, &maze, "rgb:fa/aa/ab");
    ghosties[0] = new_dude(display, screen, 1, 27, right, &maze, "rgb:fa/aa/ab");
    ghosties[1] = new_dude(display, screen, 27, 27, up, &maze, "rgb:33/99/cc");
    ghosties[2] = new_dude(display, screen, 27, 1, down, &maze, "rgb:99/33/ff");
    ghosties[3] = new_dude(display, screen, 19, 9, down, &maze, "rgb:11/11/ee");
    ghosties[4] = new_dude(display, screen, 9, 19, down, &maze, "rgb:ee/11/11");
    ghosties[5] = new_dude(display, screen, 9, 9, down, &maze, "rgb:11/ee/11");
    ghosties[6] = new_dude(display, screen, 19, 19, down, &maze, "rgb:ee/11/ee");
//     ghosties[7] = new_dude(display, screen, 25, 25, down, &maze, "rgb:ff/ff/ff");
//     ghosties[8] = new_dude(display, screen, 25, 23, down, &maze, "rgb:ff/ff/ff");
//     ghosties[9] = new_dude(display, screen, 23, 23, down, &maze, "rgb:ff/ff/ff");
//     ghosties[10] = new_dude(display, screen, 21, 21, down, &maze, "rgb:ff/ff/ff");
//     ghosties[11] = new_dude(display, screen, 25, 21, down, &maze, "rgb:ff/ff/ff");

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
    attrs.event_mask = KeyPressMask | KeyReleaseMask |
                       ButtonPressMask | ButtonReleaseMask |
                       ExposureMask | FocusChangeMask ;
//                        ExposureMask ;
    // FIXME use FocusChangeMask to automatically pause

    const Window window = XCreateWindow(display, RootWindow(display, screen),
                root_attrs.width/2-WINDOW_HEIGHT/2, root_attrs.height/2-WINDOW_HEIGHT/2,
                WINDOW_HEIGHT, WINDOW_HEIGHT,
                0, DefaultDepth(display, screen), InputOutput, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrs);

    XStoreName(display, window, "pacdan");

    XMapWindow(display, window);

    const Window centre_win = XCreateWindow(display, window, 252, 252, 195, 195,
                0, CopyFromParent, CopyFromParent, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel, &attrs);
    XMapWindow(display, centre_win);

    GC gc_fab = NULL;
    XFontStruct* font = NULL;
    initialize_font_and_colours(display, screen, &font, &gc_fab);

    struct controls_thread_data data = new_thread_data(display, window, &dirs);

    pthread_t controls;
    if (0 != pthread_create(&controls, NULL, handle_xevents, &data)) {
        fputs("could not create thread.\n", stderr);
        exit(EXIT_FAILURE);
    }

    draw_game(display, window, &maze, &dan, ghosties, num_ghosties);

    uint64_t foods_eaten = 0;

//     const struct timespec tim = {.tv_sec = 0, .tv_nsec = 5000000L};
    const struct timespec tim = {.tv_sec = 0, .tv_nsec = 1000000L};
    uint64_t debug_iteration = 0;
    while (game_in_progress(&data)) {
        debug_iteration++;
        printf("------------------------------------------------------------iteration: %lu\n", debug_iteration);
        assert (maze.food_count + foods_eaten == 388 - num_ghosties);

        XNoOp(display);

        long unsigned req_num = X_DPY_GET_REQUEST(display);
        long unsigned req_last_read = X_DPY_GET_LAST_REQUEST_READ(display);
        printf("-------------------- req num: %lu\n", req_num);
        printf("-------------------- req num: %lu\n", req_last_read);
        if (req_num > 10000 ) {
            puts("-------------------------------------- goin in");
//             __attribute__((unused)) xGetInputFocusReply rep;
            _XGetRequest(display, X_GetInputFocus, SIZEOF(xReq));
//             (void) _XReply (display, (xReply *)&rep, 0, xTrue);
//             sync_while_locked(display);
        }

        puts("drawin game");
        draw_game(display, window, &maze, &dan, ghosties, num_ghosties);
        update_score(display, centre_win, gc_fab, font, foods_eaten); // FIXME only update if necessary
        puts("flushin");
        XFlush(display);
        puts("flushed");

        while (game_is_paused(&data)) {
            game_paused(display, centre_win, gc_fab, font, foods_eaten == 0);
            draw_game(display, window, &maze, &dan, ghosties, num_ghosties);
            thread_wait();
        }

        if (0 == maze.food_count) {
            congratulate(display, centre_win, gc_fab, font);
            break;
        } else if (dan_is_eaten(&dan, ghosties, num_ghosties)) {
            insult_the_loser(display, centre_win, gc_fab, font);
            break;
        }

        puts("sleepin");
        nanosleep(&tim, NULL);

        thread_lock();
        if (dirs.right) {
            move_dan(&dan, right, &maze, display, window, &foods_eaten);
        } else if (dirs.up) {
            move_dan(&dan, up, &maze, display, window, &foods_eaten);
        } else if (dirs.left) {
            move_dan(&dan, left, &maze, display, window, &foods_eaten);
        } else if (dirs.down) {
            move_dan(&dan, down, &maze, display, window, &foods_eaten);
        }
        puts("movin ghosties");
        for (uint8_t i = 0; i < num_ghosties; i++) {
            move_ghostie(&ghosties[i], &maze, display, window);
        }
        thread_unlock();
    }
    printf("final score is: %lu.\n", foods_eaten*100); // make the score bigger, that's what makes games fun
    pthread_join(controls, NULL); // quit for player to hit q or escape
    XUnloadFont(display, font->fid);
    XDestroyWindow(display, centre_win);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
