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
