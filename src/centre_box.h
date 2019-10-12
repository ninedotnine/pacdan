#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <stdbool.h>
/*  this file contains routines relevant to the scoreboard.
 */
void initialize_font_and_colours(Display * dpy, int screen, XFontStruct ** font, GC * gc_fab);

void update_score(Display * dpy, Window win, GC gc, XFontStruct * font, uint64_t foods_eaten);

void game_paused(Display * dpy, Window win, GC gc, XFontStruct * font, bool begin);

void insult_the_loser(Display * dpy, Window win, GC gc, XFontStruct * font);

void congratulate(Display * dpy, Window win, GC gc, XFontStruct * font);
