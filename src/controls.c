static void handle_keypress(XEvent event, Controls_thread_data* const data) {
    const KeySym keysym = XLookupKeysym(&event.xkey, 0);
    thread_lock();
    switch (keysym) {
        case XK_Escape:
        case XK_q:
            data->game_over = true;
            thread_unlock();
            thread_signal();
            pthread_exit(0); // the end of the game
        case XK_Pause:
        case XK_space:
            data->paused = true;
            thread_unlock();
            return;
        case XK_Right:
        case XK_d:
        case XK_l:
            if (! data->dirs->right) {
                data->dirs->right = true;
            }
            break;
        case XK_Up:
        case XK_w:
        case XK_k:
            if (! data->dirs->up) {
                data->dirs->up = true;
            }
            break;
        case XK_Left:
        case XK_a:
        case XK_h:
            if (! data->dirs->left) {
                data->dirs->left = true;
            }
            break;
        case XK_Down:
        case XK_s:
        case XK_j:
            if (! data->dirs->down) {
                data->dirs->down = true;
            }
            break;
        default:
            fputs("that key doesn't do anything.\n", stderr);
            thread_unlock();
            return;
    }
    thread_unlock();
    if (data->paused) {
        data->paused = false; // whichever way you go, unpause the game
        thread_signal();
    }
}

static void handle_keyrelease(XEvent event, Directions* const dirs) {
    const KeySym keysym = XLookupKeysym(&event.xkey, 0);
    thread_lock();
    switch (keysym) {
        case XK_Right:
        case XK_d:
        case XK_l:
            dirs->right = false;
            break;
        case XK_Up:
        case XK_w:
        case XK_k:
            dirs->up = false;
            break;
        case XK_Left:
        case XK_a:
        case XK_h:
            dirs->left = false;
            break;
        case XK_Down:
        case XK_s:
        case XK_j:
            dirs->down = false;
            break;
        default:
            break;
    }
    thread_unlock();
}

static void * handle_xevents(void * arg) {
    Controls_thread_data* data = (Controls_thread_data*) arg;
    assert (data != NULL);
    assert (data->dpy != NULL);

    XEvent event;
    while (data->dpy != NULL && (! data->game_over)) {
        XNextEvent(data->dpy, &event);
        assert(event.type == Expose ||
               event.type == KeyPress ||
               event.type == KeyRelease ||
               event.type == ButtonPress ||
               event.type == ButtonRelease ||
               event.type == MappingNotify);
        switch (event.type) {
          case Expose:
            puts("received expose event"); // should signal main thread to redraw
            break;
          case KeyPress:
            handle_keypress(event, data);
            break;
          case KeyRelease: // FIXME : prevent the player from holding multiple keys
            handle_keyrelease(event, data->dirs);
            break;
          case ButtonPress:
            puts("button pressed, does nothing");
            break;
          case ButtonRelease:
            puts("button released, does nothing");
            break;
          case MappingNotify:
            XRefreshKeyboardMapping(&event.xmapping);
            break;
          default:
            fprintf(stderr, "received unusual XEvent of type %d\n", event.type);
        }
    }
    pthread_exit(0);
}


static Controls_thread_data gcc_pure new_thread_data(Display* const dpy, const Window win, Directions* const dirs) {
    const Controls_thread_data data = {
        .dpy = dpy,
        .win = win,
        .dirs = dirs,
        .game_over = false,
        .paused = true // start game paused
    };
    return data;
}
