// #include "types.h"

static void draw_or_erase_dude(Display* display, Window window, Dude* dude, bool erase) {
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

    assert (dude->size > 0);
    const uint32_t halfsize = dude->size / 2;
    const uint16_t mouth_line_length = 20;
    const uint32_t startCircle = (dude->direction * 90 * 64) + 2500;
    const uint32_t endCircle = 360 * 64 - 5000;
    XDrawArc(display, window, gc,
        dude->x-halfsize, dude->y-halfsize, // x and y are in the upper-left corner
        dude->size, dude->size, // width and height
        startCircle, endCircle);

    switch (dude->direction) {
        case right:
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x + mouth_line_length, dude->y + 15);
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x + mouth_line_length, dude->y - 15);
            break;
        case up:
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x + 15, dude->y - mouth_line_length);
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x - 15, dude->y - mouth_line_length);
            break;
        case left:
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x - mouth_line_length, dude->y + 15);
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x - mouth_line_length, dude->y - 15);
            break;
        case down:
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x + 15, dude->y + mouth_line_length);
            XDrawLine(display, window, gc,
                      dude->x, dude->y, dude->x - 15, dude->y + mouth_line_length);
            break;
    }
}

void draw_dude(Display * dpy, Window win, Dude* dude) {
    draw_or_erase_dude(dpy, win, dude, false);
}

void erase_dude(Display * dpy, Window win, Dude* dude) {
    draw_or_erase_dude(dpy, win, dude, true);

}


