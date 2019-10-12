#include "centre_box.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
/*  this file contains routines relevant to the scoreboard.
 */
void initialize_font_and_colours(Display * const dpy,
                                 const int screen,
                                 XFontStruct ** const font,
                                 GC * const gc_fab) {
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

static int strlen_int(const char * const str) {
    size_t length = strlen(str);
    assert (length < INT_MAX);
    return (int) length;
}

#define max_text_length 15
void update_score(Display * const dpy,
                  const Window win,
                  const GC gc,
                  XFontStruct * const font,
                  const uint64_t foods_eaten) {
    assert (dpy != NULL);
    assert (gc != NULL);
    assert (font != NULL);

    XClearWindow(dpy, win);

    char text[max_text_length] = "score:";
    int length = strlen_int(text);

    XDrawString(dpy, win, gc,
            (195-XTextWidth(font, text, length))/2,
            ((195-(font->ascent+font->descent))/2),
            text, length);

    snprintf(text, max_text_length, "%ld", foods_eaten * 100); // in games, numbers are always multiplied by 100

    length = strlen_int(text);
    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)+font->ascent,
            text, length);
}
#undef max_text_length

void game_paused(Display * const dpy,
                 const Window win,
                 const GC gc,
                 XFontStruct * const font,
                 const bool begin) {
    assert (dpy != NULL);
    assert (gc != NULL);
    assert (font != NULL);

    const char* text = "paused";
    int length = strlen_int(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*2),
            text, length);

    if (begin) {
        text = "move to begin.";
    } else {
        text = "move to resume.";
    }
    length = strlen_int(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)+(font->ascent*4),
            text, length);

    XFlush(dpy);
}

void insult_the_loser(Display * const dpy,
                      const Window win,
                      const GC gc,
                      XFontStruct * const font) {
    assert (dpy != NULL);
    assert (gc != NULL);
    assert (font != NULL);

    const char* text = "you have";
    int length = strlen_int(text);
    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*3),
            text, length);

    text = "been eaten";
    length = strlen_int(text);
    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*2),
            text, length);

    text = "ESC to quit.";
    length = strlen_int(text);
    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)+(font->ascent*4),
            text, length);
    XFlush(dpy);
}

void congratulate(Display * const dpy,
                  const Window win,
                  const GC gc,
                  XFontStruct * const font) {
    assert (dpy != NULL);
    assert (gc != NULL);
    assert (font != NULL);

    const char* text = "okay, you win.";
    int length = strlen_int(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)-(font->ascent*2),
            text, length);

    text = "ESC to quit.";
    length = strlen_int(text);

    XDrawString(dpy, win, gc,
           (195-XTextWidth(font, text, length))/2,
           ((195-(font->ascent+font->descent))/2)+(font->ascent*4),
            text, length);
    XFlush(dpy);
}
