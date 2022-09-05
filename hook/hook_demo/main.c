#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>

int main(int argc, char *argv[])
{
    Display * display_ptr = XOpenDisplay(NULL);

//    XWarpPointer(display_ptr, None, DefaultRootWindow(display_ptr), 0, 0, 0, 0, 300, 300);

    XEvent  xButton;
    xButton.xbutton.type = ButtonPress;
    xButton.xbutton.x = 5;
    xButton.xbutton.y = 5;
    xButton.xbutton.x_root = 5;
    xButton.xbutton.y_root = 5;
    xButton.xbutton.send_event = False;
    xButton.xbutton.same_screen = True;
    xButton.xbutton.button =   4  ;
//            xButton.xbutton.button = Button4;
    xButton.xbutton.time = CurrentTime;
    xButton.xbutton.window = atoi(argv[1]);
    if ( display_ptr)
    {
        Status status = XSendEvent(display_ptr, atoi(argv[1]), True, ButtonPress , &xButton);
        int xflush = XFlush(display_ptr);
        printf("ButtonPress [status = %u][xflush = %u]\n", status, xflush);
    }


    XEvent  xWheel;
    xWheel.xbutton.type = ButtonRelease;
    xWheel.xbutton.display = display_ptr;
    xWheel.xbutton.x = 5;
    xWheel.xbutton.y = 5;
    xWheel.xbutton.x_root = 5;
    xWheel.xbutton.y_root = 5;
    xWheel.xbutton.send_event = False;
    xWheel.xbutton.same_screen = True;
    xWheel.xbutton.button =   4  ;
//            xWheel.xbutton.button = Button4;
    xWheel.xbutton.time = CurrentTime;
    xWheel.xbutton.window = atoi(argv[1]);
    if (display_ptr)
    {
        Status status =     XSendEvent(display_ptr, atoi(argv[1]), False, ButtonRelease , &xWheel);
        int xflush = XFlush(display_ptr);
        printf("BUttonRelease [status = %u][xflush = %u]\n", status, xflush);
    }

//	printf("enter\n");
//	void * p = malloc(1);
//	free(p);
//	printf("left = %p \n", p);
	return 0;
}
