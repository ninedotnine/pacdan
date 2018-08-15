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

    XSetFont(dpy, *gc_fab, (*font)->fid);
}

#define max_text_length 15
static void update_score(Display* const dpy, const Window win, const GC gc, XFontStruct* const font,
                         const uint64_t foods_eaten) {
    assert (dpy != NULL);
    assert (gc != NULL);
    assert (font != NULL);

    XClearWindow(dpy, win);

    char text[max_text_length] = "score:";
    int length = strlen(text);

    XDrawString(dpy, win, gc,
            (195-XTextWidth(font, text, length))/2,
            ((195-(font->ascent+font->descent))/2),
            text, length);

    snprintf(text, max_text_length, "%ld", foods_eaten * 100); // in games, numbers are always multiplied by 100

    length = strlen(text);
    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)+font->ascent,
            text, length);
}
#undef max_text_length

static void game_paused(Display* const dpy, const Window win, const GC gc, XFontStruct* const font, const bool begin) {
    assert (dpy != NULL);
    assert (gc != NULL);
    assert (font != NULL);

    char* text = "paused";
    int length = strlen(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*2),
            text, length);

    if (begin) {
        text = "move to begin.";
    } else {
        text = "move to resume.";
    }
    length = strlen(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)+(font->ascent*4),
            text, length);

    XFlush(dpy);
}

static void congratulate(Display* const dpy, const Window win, const GC gc, XFontStruct* const font) {
    assert (dpy != NULL);
    assert (gc != NULL);
    assert (font != NULL);

    char* text = "okay, you win.";
    int length = strlen(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*2),
            text, length);

    text = "ESC to quit.";
    length = strlen(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)+(font->ascent*4),
            text, length);
    XFlush(dpy);
}
