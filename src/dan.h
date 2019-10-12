#include "types.h"
#include "dude.h"

#include <X11/Xlib.h>

#include <stdbool.h>
/* dan is the name of the protagonist.
 */

void draw_or_erase_dan(Display* const dpy, const Window win, const struct dude * const dan, const bool erase);


void draw_dan(Display* const dpy, const Window win, const struct dude * const dan);

void erase_dan(Display* const dpy, const Window win, const struct dude * const dan);

void move_dan(struct dude * dan,
                     const enum direction dir,
                     struct maze * const maze,
                     Display * const dpy,
                     const Window win,
                     uint64_t * const foods_eaten);
