#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <stdbool.h>
/*  this file contains routines relevant to the scoreboard.
 */
void initialize_font_and_colours(Display* const dpy, const int screen, XFontStruct** const font,
                                        GC* const gc_fab);

void update_score(Display* const dpy, const Window win, const GC gc, XFontStruct* const font,
                         const uint64_t foods_eaten);

void game_paused(Display* const dpy, const Window win, const GC gc, XFontStruct* const font, const bool begin);

void insult_the_loser(Display* const dpy, const Window win, const GC gc, XFontStruct* const font);

void congratulate(Display* const dpy, const Window win, const GC gc, XFontStruct* const font);
