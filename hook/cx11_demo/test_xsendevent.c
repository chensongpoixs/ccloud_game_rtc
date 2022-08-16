#include <X11/Xlib.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <signal.h>

#include <time.h>

#include <sys/time.h>

int x=10,y=10;
struct _XDisplay *d;
int s;
Window w;
XEvent e;

void sigroutine(int signo){

    /* send event */
    XSendEvent(d, w, 1, ExposureMask, &e);
    printf("Catch a signal -- SIGALRM \n");
    signal(SIGALRM, sigroutine);
    return;
}

int main()
{

    struct itimerval value, ovalue;
    /* open connection with the server */
    d = XOpenDisplay(NULL);
    if(d == NULL)
    {
        printf("Cannot open display\n");
        exit(1);
    }
    s = DefaultScreen(d);

    /* create window */
    w = XCreateSimpleWindow(d,
                            RootWindow(d, s),
                            10,
                            10,
                            500,
                            500,
                            1,
                            BlackPixel(d, s),
                            WhitePixel(d, s));

    /* select kind of events we are interested in */
    XSelectInput(d, w, ExposureMask | KeyPressMask);

    /* map (show) the window */
    XMapWindow(d, w);

    /* Set timer */
    signal(SIGALRM, sigroutine);

    value.it_value.tv_sec = 1;

    value.it_value.tv_usec = 0;

    value.it_interval.tv_sec = 1;

    value.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &value, &ovalue);

    /* event loop */
    while(1)
    {
        XNextEvent(d, &e);
        /* draw or redraw the window */
        if(e.type == Expose)
        {
            printf("Expose \n");
            XFillRectangle(d, w, DefaultGC(d, s), x, y, 10, 10);
            x+=10;
            y+=10;
        }
        /* exit on key press */
        if(e.type == KeyPress)

            break;
    }

    /* close connection to server */
    XCloseDisplay(d);

    return 0;
}
