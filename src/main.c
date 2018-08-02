/* pacman game with xlib */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "types.h"
#include "movements.c"
#include "maze.c"
#include "dude.c"

void initialize_font_and_colours(Display * dpy, XFontStruct** font, GC* gc_fab, GC* gc_black) {
    Colormap colourmap;
    XColor black_colour, fab_colour;
    XGCValues gcv_black, gcv_fab;

    int screen = DefaultScreen(dpy);

    colourmap = DefaultColormap(dpy, screen);
    XParseColor(dpy, colourmap, "rgb:00/00/00", &black_colour);
    XAllocColor(dpy, colourmap, &black_colour);

    XParseColor(dpy, colourmap, "rgb:fa/aa/ab", &fab_colour);
    XAllocColor(dpy, colourmap, &fab_colour);
    gcv_fab.foreground = fab_colour.pixel;
    gcv_fab.background = black_colour.pixel;
    gcv_black.foreground = black_colour.pixel;
    gcv_black.background = black_colour.pixel;
//     GC gc_fab = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_fab);
//     GC gc_black = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_black);
    *gc_fab = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_fab);
    *gc_black = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_black);

    int actual_count_return = 0;
    char ** fontList = XListFonts(dpy, "*-terminus-*", 450, &actual_count_return);
//     char ** fontList = XListFonts(dpy, "*-inconsolata-*", 450, &actual_count_return);

    if (fontList == NULL) {
        fputs("error, no fonts.", stderr);
        exit(5);
    }

//     XFontStruct * font = XLoadQueryFont(dpy, "-xos4-terminus-bold-r-normal--12-120-72-72-c-60-iso10646-1");
//     XFontStruct * font = XLoadQueryFont(dpy, fontList[0]);
//     *font = XLoadQueryFont(dpy, fontList[0]);
//     *font = XLoadQueryFont(dpy, "*-terminus-*-18-*");
    *font = XLoadQueryFont(dpy, "*-18-*");
//     XFontStruct * font = XLoadQueryFont(dpy, "7x14"); // original example code
    if ((*font) == NULL) {
        fputs("font no exist\n", stderr);
        exit(6);
    }
}

void update_score(Display * dpy, Window centre_win, uint64_t foods_eaten) {
    static uint64_t previous_foods_eaten = 42; // anything that isn't 0
    static GC gc_fab;
    static GC gc_black;
    static XFontStruct* font;
    static bool initialized = false;
    if (! initialized) {
        initialize_font_and_colours(dpy, &font, &gc_fab, &gc_black);
        initialized = true;
    }

    if (foods_eaten == previous_foods_eaten) {
        return; // no need to redraw anything
    }

    XTextItem xti;
    xti.chars = "score:";
    xti.nchars = strlen(xti.chars);
    xti.delta = 0;
    xti.font = font->fid;
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)+font->ascent-18,
            &xti, 1);

    char text[15];
    snprintf(text, 15, "%ld", previous_foods_eaten * 100); // in games, numbers are always multiplied by 100
    xti.chars = text;
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_black,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)+font->ascent,
            &xti, 1);

    previous_foods_eaten = foods_eaten;

    snprintf(text, 15, "%ld", foods_eaten * 100);
    xti.chars = text;
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)+font->ascent,
            &xti, 1);
//     XUnloadFont(dpy, font->fid);
}

void handle_keypress(XEvent event, Dude* dude, Maze* maze, Display** dpy_p, Window win) {
    KeySym keysym = XLookupKeysym(&event.xkey, 0);
    switch (keysym) {
        case XK_Escape:
        case XK_q:
            XDestroyWindow(*dpy_p, win);
            XCloseDisplay(*dpy_p);
            *dpy_p = NULL;
            return; // the end of the game
        case XK_Right:
            move_dude(dude, right, maze, *dpy_p, win);
            return;
        case XK_Up:
            move_dude(dude, up, maze, *dpy_p, win);
            return;
        case XK_Left:
            move_dude(dude, left, maze, *dpy_p, win);
            return;
        case XK_Down:
            move_dude(dude, down, maze, *dpy_p, win);
            return;
        default:
            fputs("that key doesn't do anything.\n", stderr);
    }
}

void draw_game(Display* dpy, Window win, Maze* maze, Dude* dude, Window centre_win) {
    draw_maze(dpy, win, maze);
    draw_dude(dpy, win, dude);
    update_score(dpy, centre_win, dude->foods_eaten);
}

int main(void) {
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

    // what does this do...
//     XSelectInput(display, maze.centre_win,
    XSelectInput(display, window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);

    XEvent event;
    while (display != NULL) {
        update_score(display, centre_win, dude.foods_eaten);
        assert (maze.food_count + dude.foods_eaten == 388);
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
            draw_game(display, window, &maze, &dude, centre_win);
            break;
          case KeyPress:
            handle_keypress(event, &dude, &maze, &display, window);
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
            exit(7);
        }
    }
    printf("final score is: %lu.\n", dude.foods_eaten*100); // make the score bigger, that's what makes games fun
    return 0;
}
