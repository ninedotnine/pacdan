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

void initialize_font_and_colours(Display * dpy, int screen, XFontStruct** font, GC* gc_fab) {
    assert (dpy != NULL);
    assert (font != NULL);
    assert (gc_fab != NULL);
    assert (*font == NULL); // this routine should be called only once
    assert (*gc_fab == NULL);
    Colormap colourmap;
    XColor fab_colour;
    XGCValues gcv_fab;

    colourmap = DefaultColormap(dpy, screen);

    XParseColor(dpy, colourmap, "rgb:fa/aa/ab", &fab_colour);
    XAllocColor(dpy, colourmap, &fab_colour);

    gcv_fab.foreground = fab_colour.pixel;

    *gc_fab = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_fab);

    *font = XLoadQueryFont(dpy, "*-18-*");

    if ((*font) == NULL) {
        fputs("font no exist\n", stderr);
        exit(EXIT_FAILURE);
    }
}

void update_score(Display* dpy, Window centre_win, GC gc_fab, XFontStruct* font, uint64_t foods_eaten) {
    const uint8_t max_text_length = 15;
    assert (dpy != NULL);
    assert (gc_fab != NULL);
    assert (font != NULL);
    XTextItem xti;
    xti.delta = 0;
    xti.font = font->fid;

    XClearWindow(dpy, centre_win);

    char text[max_text_length];
    xti.chars = "score:";
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2),
            &xti, 1);

    snprintf(text, max_text_length, "%ld", foods_eaten * 100); // in games, numbers are always multiplied by 100
    xti.chars = text;
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)+font->ascent,
            &xti, 1);
}

void game_paused(Display* dpy, Window centre_win, GC gc_fab, XFontStruct* font, bool begin) {
    assert (dpy != NULL);
    assert (gc_fab != NULL);
    assert (font != NULL);

    XTextItem xti;
    xti.delta = 0;
    xti.font = font->fid;

    xti.chars = "paused";
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*2),
            &xti, 1);

    if (begin) {
        xti.chars = "move to begin.";
    } else {
        xti.chars = "move to resume.";
    }
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)+(font->ascent*4),
            &xti, 1);

    XFlush(dpy);
}

void congratulate(Display* dpy, Window centre_win, GC gc_fab, XFontStruct* font) {
    assert (dpy != NULL);
    assert (gc_fab != NULL);
    assert (font != NULL);

    XTextItem xti;
    xti.delta = 0;
    xti.font = font->fid;

    xti.chars = "okay, you win.";
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*2),
            &xti, 1);

    xti.chars = "ESC to quit.";
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)+(font->ascent*4),
            &xti, 1);
    XFlush(dpy);
}

void handle_keypress(XEvent event, Xevent_thread_data* data) {
    KeySym keysym = XLookupKeysym(&event.xkey, 0);
    thread_lock();
    switch (keysym) {
        case XK_Escape:
        case XK_q:
            data->game_over = true;
            thread_unlock();
            thread_signal();
            pthread_exit(0); // the end of the game
        case XK_Pause:
        case XK_space:
            data->paused = true;
            thread_unlock();
            return;
        case XK_Right:
        case XK_d:
        case XK_l:
            if (! data->dirs->right) {
                data->dirs->right = true;
            }
            break;
        case XK_Up:
        case XK_w:
        case XK_k:
            if (! data->dirs->up) {
                data->dirs->up = true;
            }
            break;
        case XK_Left:
        case XK_a:
        case XK_h:
            if (! data->dirs->left) {
                data->dirs->left = true;
            }
            break;
        case XK_Down:
        case XK_s:
        case XK_j:
            if (! data->dirs->down) {
                data->dirs->down = true;
            }
            break;
        default:
            fputs("that key doesn't do anything.\n", stderr);
            thread_unlock();
            return;
    }
    thread_unlock();
    if (data->paused) {
        data->paused = false; // whichever way you go, unpause the game
        thread_signal();
    }
}

void handle_keyrelease(XEvent event, Directions* dirs) {
    KeySym keysym = XLookupKeysym(&event.xkey, 0);
    thread_lock();
    switch (keysym) {
        case XK_Right:
        case XK_d:
        case XK_l:
            dirs->right = false;
            break;
        case XK_Up:
        case XK_w:
        case XK_k:
            dirs->up = false;
            break;
        case XK_Left:
        case XK_a:
        case XK_h:
            dirs->left = false;
            break;
        case XK_Down:
        case XK_s:
        case XK_j:
            dirs->down = false;
            break;
        default:
            break;
    }
    thread_unlock();
}

void * handle_xevents(void * arg) {
    Xevent_thread_data* data = (Xevent_thread_data*) arg;
    assert (data != NULL);
    assert (data->dpy_p != NULL);

    XEvent event;
    while (*data->dpy_p != NULL && (! data->game_over)) {
        XNextEvent(*data->dpy_p, &event);
        assert(event.type == Expose ||
               event.type == KeyPress ||
               event.type == KeyRelease ||
               event.type == ButtonPress ||
               event.type == ButtonRelease ||
               event.type == MappingNotify);
        switch (event.type) {
          case Expose:
            puts("received expose event"); // should signal main thread to redraw
            break;
          case KeyPress:
            handle_keypress(event, data);
            break;
          case KeyRelease: // FIXME : prevent the player from holding multiple keys
            handle_keyrelease(event, data->dirs);
            break;
          case ButtonPress:
            puts("button pressed, does nothing");
            break;
          case ButtonRelease:
            puts("button released, does nothing");
            break;
          case MappingNotify:
            XRefreshKeyboardMapping(&event.xmapping);
            break;
          default:
            fprintf(stderr, "received unusual XEvent of type %d\n", event.type);
        }
    }
    pthread_exit(0);
}

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

    uint32_t num_ghosties = 7;
    Ghostie ghosties[num_ghosties];
    ghosties[0] = initialize_ghostie(1, 27, right, &maze);
    ghosties[1] = initialize_ghostie(27, 27, up, &maze);
    ghosties[2] = initialize_ghostie(27, 1, down, &maze);
    ghosties[3] = initialize_ghostie(19, 9, down, &maze);
    ghosties[4] = initialize_ghostie(9, 19, down, &maze);
    ghosties[5] = initialize_ghostie(9, 9, down, &maze);
    ghosties[6] = initialize_ghostie(19, 19, down, &maze);

    Display * display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("no display.\n", stderr);
        exit(EXIT_FAILURE);
    }

    int screen = DefaultScreen(display);

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

    Xevent_thread_data data = {
        .dpy_p = &display,
        .win_p = &window,
        .dirs = &dirs,
        .game_over = false,
        .paused = true // start game paused
    };

    pthread_t thread;
    if (0 != pthread_create(&thread, NULL, handle_xevents, &data)) {
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
    pthread_join(thread, NULL); // quit for player to hit q or escape
    XUnloadFont(display, font->fid);
    XDestroyWindow(display, centre_win);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
