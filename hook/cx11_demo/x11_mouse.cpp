/* ref: http://www.ishiboo.com/~danny/Projects/xwarppointer/ */
#include <stdio.h>
#include <string.h>
//头文件
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
//全局变量
Display *display;
Window root;
//初始化
void init()
{
    if ((display = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Cannot open local X-display.\n");
        return;
    }
    root = DefaultRootWindow(display);
}
void GetCursorPos(int *x,int *y)
{
    int tmp;unsigned int tmp2;
    Window fromroot, tmpwin;
    XQueryPointer(display, root, &fromroot, &tmpwin, x, y, &tmp, &tmp, &tmp2);
}
//设置坐标
void SetCursorPos(int x,int y)
{
    int tmp;
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XFlush(display);
}
//模拟点击
/* http://www.linuxquestions.org/questions/programming-9/simulating-a-mouse-click-594576/ */
void mouseClick(int button)
{
    Display *display = XOpenDisplay(NULL);

    XEvent event;

    if(display == NULL)
    {
        printf("Errore nell'apertura del Display !!!\n");
        return;
    }

    memset(&event, 0x00, sizeof(event));

    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;

    XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;

    while(event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;

        XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) printf("Errore nell'invio dell'evento !!!\n");

    XFlush(display);


    sleep(1);

    event.type = ButtonRelease;
    event.xbutton.state = 0x100;

    if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) printf("Errore nell'invio dell'evento !!!\n");

    XFlush(display);

    XCloseDisplay(display);
}
int main()
{
//init();
//int x,y;
//GetCursorPos(&x,&y);
//printf("%d %d\n",x,y);
//SetCursorPos(500,522);
//XCloseDisplay(display);
//mouseClick(Button1);
//sleep(5);
    mouseClick(Button3);


    return 0;
}