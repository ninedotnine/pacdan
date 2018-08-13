/*  this file contains routines relevant to the scoreboard.
 */
static void initialize_font_and_colours(Display* const dpy, const int screen, XFontStruct** const font,
                                        GC* const gc_fab) {
    assert (dpy != NULL);
    assert (font != NULL);
    assert (gc_fab != NULL);
    assert (*font == NULL); // this routine should be called only once
    assert (*gc_fab == NULL);

    Colormap colourmap = DefaultColormap(dpy, screen);

    XColor fab_colour;
    XParseColor(dpy, colourmap, "rgb:fa/aa/ab", &fab_colour);
    XAllocColor(dpy, colourmap, &fab_colour);

    XGCValues gcv_fab = {
        .foreground = fab_colour.pixel
    };

    *gc_fab = XCreateGC(dpy, RootWindow(dpy, screen), GCForeground | GCBackground, &gcv_fab);

    *font = XLoadQueryFont(dpy, "*-18-*");

    if ((*font) == NULL) {
        fputs("font no exist\n", stderr);
        exit(EXIT_FAILURE);
    }
}

static void update_score(Display* const dpy, const Window centre_win, const GC gc_fab, XFontStruct* const font,
                         const uint64_t foods_eaten) {
    const uint8_t max_text_length = 15;
    assert (dpy != NULL);
    assert (gc_fab != NULL);
    assert (font != NULL);

    XClearWindow(dpy, centre_win);

    XTextItem xti = {
        .delta = 0,
        .font = font->fid,
        .chars = "score:",
        .nchars = strlen(xti.chars)
    };

    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2),
            &xti, 1);

    char text[max_text_length];
    snprintf(text, max_text_length, "%ld", foods_eaten * 100); // in games, numbers are always multiplied by 100
    xti.chars = text;
    xti.nchars = strlen(xti.chars);
    XDrawText(dpy, centre_win, gc_fab,
           (195-XTextWidth(font, xti.chars, xti.nchars))/2,
           ((195-(font->ascent+font->descent))/2)+font->ascent,
            &xti, 1);
}

static void game_paused(Display* const dpy, const Window centre_win, const GC gc_fab, XFontStruct* const font,
                        const bool begin) {
    assert (dpy != NULL);
    assert (gc_fab != NULL);
    assert (font != NULL);

    XTextItem xti = {
        .delta = 0,
        .font = font->fid,
        .chars = "paused",
        .nchars = strlen(xti.chars)
    };

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

static void congratulate(Display* const dpy, const Window centre_win, const GC gc_fab, XFontStruct* const font) {
    assert (dpy != NULL);
    assert (gc_fab != NULL);
    assert (font != NULL);

    XTextItem xti = {
        .delta = 0,
        .font = font->fid,
        .chars = "okay, you win.",
        .nchars = strlen(xti.chars)
    };

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
