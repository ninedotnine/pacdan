/* draw a little icon */
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WINDOW_HEIGHT 70

typedef struct {
    uint32_t size; // ghostie is a square, this means width and height
    GC gc;
} Ghostie;


static Ghostie new_ghostie(Display* const dpy, const int screen, const char* const colour) {
    const Colormap colourmap = DefaultColormap(dpy, screen);

    XColor ghostie_colour;
    XParseColor(dpy, colourmap, colour, &ghostie_colour);
    XAllocColor(dpy, colourmap, &ghostie_colour);

    XGCValues gcv = {
        .foreground = ghostie_colour.pixel
    };

    const GC gc = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv);

    const Ghostie ghostie = {
        .size = 48,
        .gc = gc
    };

    return ghostie;
}

static void draw_ghostie(Display* const dpy, const Window win, const Ghostie* const ghostie, int x, int y) {
    assert (dpy != NULL);
    assert (ghostie != NULL);
    assert (ghostie->size > 0);

    GC gc = ghostie->gc;

    const uint32_t halfsize = ghostie->size / 2;
    const uint8_t squiggle_line_length = 8;
    const uint32_t fullCircle = 360 * 64;
    const uint32_t halfCircle = fullCircle / 2;
    const uint32_t eye_height = y-12;

    XPoint points[9] = {{
        .x = x-halfsize,
        .y = y
    }, {
        .x = 0,
        .y = +halfsize
    }, {
        .x = squiggle_line_length,
        .y = -squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = -squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = -squiggle_line_length
    }, {
        .x = squiggle_line_length,
        .y = squiggle_line_length
    }, {
        .x = 0,
        .y = -halfsize
    }};

    XDrawLines(dpy, win, gc, points, 9, CoordModePrevious);

    XDrawArc(dpy, win, gc,
        x-halfsize, y-halfsize, // x and y are in the upper-left corner
        ghostie->size, ghostie->size, // width and height
        0, halfCircle);

    // draw the eyes
    XDrawArc(dpy, win, gc,
        x-15, eye_height, // x and y are in the upper-left corner
        6, 6, // width and height
        0, fullCircle);

    XDrawArc(dpy, win, gc,
        x+9, eye_height, // x and y are in the upper-left corner
        6, 6, // width and height
        0, fullCircle);

    // the irises
    XDrawArc(dpy, win, gc,
        x+12, eye_height+2, // x and y are in the upper-left corner
        2, 2, // width and height
        0, fullCircle);
    XDrawArc(dpy, win, gc,
        x-12, eye_height+2, // x and y are in the upper-left corner
        2, 2, // width and height
        0, fullCircle);
}

int main(void) {
    Display* const display = XOpenDisplay(NULL);
    if (display == NULL) {
        fputs("no display.\n", stderr);
        exit(EXIT_FAILURE);
    }

    const int screen = DefaultScreen(display);

    Ghostie ghostie = new_ghostie(display, screen, "rgb:fa/aa/ab");

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
    attrs.event_mask = ExposureMask | KeyPressMask ;

    const Window window = XCreateWindow(display, RootWindow(display, screen),
                root_attrs.width/2-WINDOW_HEIGHT/2, root_attrs.height/2-WINDOW_HEIGHT/2,
                WINDOW_HEIGHT, WINDOW_HEIGHT,
                0, DefaultDepth(display, screen), InputOutput, CopyFromParent,
                CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrs);

    XStoreName(display, window, "pacdan");

    XMapWindow(display, window);

    XEvent event;
    while (true) {
        XNextEvent(display, &event);
        assert(event.type == Expose ||
               event.type == KeyPress ||
               event.type == MappingNotify);
        switch (event.type) {
          case Expose:
            XMapWindow(display, window);
            draw_ghostie(display, window, &ghostie, WINDOW_HEIGHT/2, WINDOW_HEIGHT/2);
            break;
          case KeyPress:
            switch (XLookupKeysym(&event.xkey, 0)) {
                case XK_Escape:
                case XK_q:
                    XDestroyWindow(display, window);
                    XCloseDisplay(display);
                    return 0;
            }
            break;
          case MappingNotify:
            XRefreshKeyboardMapping(&event.xmapping);
            break;
          default:
            fprintf(stderr, "received unusual XEvent of type %d\n", event.type);
        }
    }
}
