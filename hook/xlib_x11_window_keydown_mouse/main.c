/**
 * Phase 01 - Get a Window that works and can be closed.
 *
 * This code won't be structured very well, just trying to get stuff working.
 */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// ~16.6 ms between frames is ~60 fps.
#define RATE_LIMIT 16.6

// Forward declaration of this function so we can use it in main().
double timespec_diff(struct timespec *a, struct timespec *b);

int main(int argc, char *argv[])
{
    // Create application display.
    Display *dpy = XOpenDisplay(NULL);

    if (dpy == NULL) {
        return EXIT_FAILURE;
    }

    // Create the application Window.
    unsigned long black = WhitePixel(dpy, DefaultScreen(dpy));
    Window win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 350, 200, 0, black, black);

    // Setup the Window Manager hints.
    XWMHints *wmhints = XAllocWMHints();
    // This basically tells other functions that this contains a value for input and initial state.
    wmhints->flags = InputHint | StateHint;
    // And these are the values for input and initial state.
    wmhints->input = True;
    wmhints->initial_state = NormalState;

    // Setup the Size Hints (also for the Window Manager).
    XSizeHints *sizehints = XAllocSizeHints();
    // This tells other functions that the value for min width and height.
    sizehints->flags = PMinSize;
    // And these are the values for min width and height.
    sizehints->min_width = 100;
    sizehints->min_height = 60;

    /*
     * This particular function does some allocating that doesn't ever get freed.
     * Valgrind reports 27,262 bytes in 384 blocks as still reachable because of this.
     * It's possible that this "leak" could be avoided by using XSetWMProperties()
     * and creating our own XTextProperty's.
     */
    // Sets Window properties that are used by the Window Manager.
    Xutf8SetWMProperties(dpy, win, "test keyboard", "", NULL, 0, sizehints, wmhints, NULL);

    // Tell X that we want to be notified of the Exposure event, so we can know when our window is initially visible.
    XSelectInput(dpy, win, ExposureMask);

    // Grab a copy of X's representation of WM_PROTOCOLS, used in checking for window closed events.
    Atom wm_protocol = XInternAtom(dpy, "WM_PROTOCOLS", True);
    // Let the Window Manager know that we want the event when a user closes the window.
    Atom wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(dpy, win, &wm_delete, 1);

    // Map the window to the display.
    XMapWindow(dpy, win);

    // This variable will be used to examine events thrown to our application window.
    XEvent e;

    // Block execution until the window is exposed.
    XWindowEvent(dpy, win, ExposureMask, &e);

    // After being exposed, we'll tell X what input events we want to know about here.
    XSelectInput(dpy, win, KeyPressMask);

    // The loop
    // @TODO: Use sleeping to avoid taking up all CPU cycles.
    Bool done = False;

    // We need to track very small periods of time (nanoseconds), so we use the struct timespec.
    struct timespec prev, curr;

    /*
     * Get the current time with CLOCK_MONOTONIC_RAW, which gets the time past since a certain time.
     * CLOCK_MONOTONIC_RAW is not subject to adjustments to the system clock.
     */
    clock_gettime(CLOCK_MONOTONIC_RAW, &curr);
    // Initialize the previous time with the current time, that way our current vs. previous comparison is valid.
    prev.tv_sec = curr.tv_sec;
    prev.tv_nsec = curr.tv_nsec;

    // This variable will be used to normalize our loop to a specific rate.
    double mill_store = 0;

    // A couple of variables used to deal with KeyPress and KeyRelease events.
    KeySym event_key;
    char *key_string = NULL;

    while(!done) {
        // Get the current time.
        clock_gettime(CLOCK_MONOTONIC_RAW, &curr);
        // Store the difference in ms between curr and prev, store it in mill_store for use later.
        mill_store += timespec_diff(&curr, &prev);

        // @TODO: Determine if this should happen before updating curr.
        // Handle events in the event queue.
        while(XPending(dpy) > 0) {
            XNextEvent(dpy, &e);
            switch(e.type) {
                case ClientMessage:
                    // This client message is a window manager protocol.
                    if (e.xclient.message_type == wm_protocol) {
                        // Somehow this checks if the protocol was a WM_DELETE protocol, so we can exit the loop and be done.
                        if (e.xclient.data.l[0] == wm_delete) {
                            done = True;
                        }
                    }
                    break;
                case KeyPress:
                    // Handle KeyPress events.
                    // @TODO: set the second value (index) properly
                    event_key = XLookupKeysym(&(e.xkey), 0);
                    key_string = XKeysymToString(event_key);
                    printf("Key pressed: %s\n", key_string);
                    break;
            }
        }

        // Only do stuff if the ms passed is greater than our rate limit.
        if (mill_store > RATE_LIMIT && !done) {
            /*
             * This loop counts down the mill_store, so if we have more ms stored than the Rate limit,
             * we run all the processes once. If we have three times the rate limit, we run all the
             * processes thrice. If the mill_store is less than the rate limit, then we pass on
             * processing for this time around the loop (which shouldn't really happen).
             *
             * This helps us have predictable numbers when we do things dependent on numbers, like physics.
             */
            for (; mill_store > RATE_LIMIT && ! done; mill_store -= RATE_LIMIT) {
                // Things that should run once per tick/frame will go here.
            }
        }

        // Update the previous timespec with the most recent timespec so we can calculate the diff next time around.
        prev.tv_sec = curr.tv_sec;
        prev.tv_nsec = curr.tv_nsec;

        /**
         * Make our process sleep to avoid locking up the CPU.
         *
         * From what I understand, the following sleep code will not work on Windows.
         * It works on Linux, it probably works on OSX, but a different approach is needed
         * for Windows.
         */
        if (mill_store < RATE_LIMIT && !done) {
            // We'll need a couple of timespecs, and an int to check for errors.
            struct timespec sleep_required, sleep_remaining;
            int was_error = 0;

            // initialize the remaining sleep time with the value in mill_store.
            sleep_remaining.tv_sec = mill_store / 1000.0;
            sleep_remaining.tv_nsec = ((int)mill_store % 1000) * 1000000;

            do {
                // Set the required sleep time using the remaining time, so we can continue sleeping if nanosleep is interrupted.
                sleep_required.tv_sec = sleep_remaining.tv_sec;
                sleep_required.tv_nsec = sleep_remaining.tv_nsec;

                // Clear out the errno variable before calling nanosleep so we can catch errors.
                errno = 0;
                // Try sleeping for the required time, if nanosleep is interrupted, sleep_remaining will have the time left to sleep.
                was_error = nanosleep(&sleep_required, &sleep_remaining);
                // Keep looping if nanosleep was interrupted and there is some sleep time remaining.
            } while (was_error == -1 && errno == EINTR);
        }
    }

    // Free all the things.
    XFree(sizehints);
    sizehints = NULL;
    XFree(wmhints);
    wmhints = NULL;

    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    dpy = NULL;

    return EXIT_SUCCESS;
}

/**
 * This returns the difference between the values of two timespecs.
 */
double timespec_diff(struct timespec *a, struct timespec *b)
{
    return (((a->tv_sec * 1000000000) + a->tv_nsec) - ((b->tv_sec * 1000000000) + b->tv_nsec)) / 1000000.0;
}