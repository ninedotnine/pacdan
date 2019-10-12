#pragma once

#include "types.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

void * handle_xevents(void * arg);

struct controls_thread_data gcc_pure new_thread_data(Display * const dpy,
                                                     const Window win,
                                                     struct directions * const dirs);
