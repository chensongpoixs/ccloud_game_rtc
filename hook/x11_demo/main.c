


#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

int main(void) {
    Display *d;
    Window w;
    XEvent e;
    char *msg = "Hello, World!";
    int s;


    d = XOpenDisplay(NULL);
    if (d == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    s = DefaultScreen(d);


    w = XCreateSimpleWindow(d, RootWindow(d, s), 200, 200, 600, 600, 1,
                            BlackPixel(d, s), WhitePixel(d, s));


    XSelectInput(d, w, ExposureMask |
                        KeyPressMask
   |         KeyPress
   | KeyRelease
   | ButtonPress
   | ButtonRelease
   | MotionNotify
   | EnterNotify
   | LeaveNotify
   | FocusIn
   | FocusOut
   | KeymapNotify
   | Expose
   | GraphicsExpose
   | NoExpose
   | VisibilityNotify
   | CreateNotify
   | DestroyNotify
   | UnmapNotify
   | MapNotify
   | MapRequest
   | ReparentNotify
   | ConfigureNotify
   | ConfigureRequest
   | GravityNotify
   | ResizeRequest
   | CirculateNotify
   | CirculateRequest
   | PropertyNotify
   | SelectionClear
   | SelectionRequest
   | SelectionNotify
   | ColormapNotify
   | ClientMessage
   | MappingNotify
   | GenericEvent
   | LASTEvent




































    );


    XMapWindow(d, w);


    while (1) {

        XNextEvent(d, &e);


        if (e.type == Expose) {
            XFillRectangle(d, w, DefaultGC(d, s), 20, 20, 10, 10);
            XDrawString(d, w, DefaultGC(d, s), 50, 50, msg, strlen(msg));
        }

        if (e.type == KeyPress)
            break;
    }


    XCloseDisplay(d);

    return 0;
}