// #include "types.h"

void draw_or_erase_pacman(Display* display, Window window, Pacman* pacman, bool erase) {
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

    assert (pacman->size > 0);
    const uint32_t halfsize = pacman->size / 2;
    const uint16_t mouth_line_length = 20;
    const uint32_t startCircle = (pacman->direction * 90 * 64) + 2500;
    const uint32_t endCircle = 360 * 64 - 5000;
    XDrawArc(display, window, gc,
        pacman->x-halfsize, pacman->y-halfsize, // x and y are in the upper-left corner
        pacman->size, pacman->size, // width and height
        startCircle, endCircle);

    switch (pacman->direction) {
        case right:
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x + mouth_line_length, pacman->y + 15);
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x + mouth_line_length, pacman->y - 15);
            break;
        case up:
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x + 15, pacman->y - mouth_line_length);
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x - 15, pacman->y - mouth_line_length);
            break;
        case left:
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x - mouth_line_length, pacman->y + 15);
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x - mouth_line_length, pacman->y - 15);
            break;
        case down:
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x + 15, pacman->y + mouth_line_length);
            XDrawLine(display, window, gc,
                      pacman->x, pacman->y, pacman->x - 15, pacman->y + mouth_line_length);
            break;
    }
}

void draw_pacman(Display * dpy, Window win, Pacman* pacman) {
    draw_or_erase_pacman(dpy, win, pacman, false);
}

void erase_pacman(Display * dpy, Window win, Pacman* pacman) {
    draw_or_erase_pacman(dpy, win, pacman, true);

}


