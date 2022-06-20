//
// Created by chensong on 6/20/22.
//

#include <X11/Xlib.h>
// 每一个Xlib 程序都必须包含这个头文件
#include <stdio.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <regex.h>
#include <ctype.h>
#include <locale.h>
#include <stdarg.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xinerama.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include <xkbcommon/xkbcommon.h>

// #include "xdo.h"
// #include "xdo_util.h"
// #include "xdo_version.h"

int main(int argc, char *argv[])
{

    // 首先打开与server 的连接。在你的程序可以使用display 之前，必须先建立一个和X server 的连接。这个连接建立以后，就可以使用Xlib 的函数或宏定义来获得display 的信息了。
// 参数为hardware display name，当设为NULL时，为默认的DISPLAY环境变量。这个函数返回一个指向Display类型的结构的指针，表明已与X server建立了连接，并且包含了这个X serer的所有信息。
// 这样我们就可以使用display之上的所有窗口了。

    Display *display = XOpenDisplay(NULL);





    int screen = DefaultScreen(display);

// 得到display打开之后的窗口号。本例中，得到0。我理解是：尚未创建窗口，所以窗口号为0。不知道对不对。
// DefaultScreen是个宏，对应的函数为XDefaultScreen。两者作用相同。



    int width = DisplayWidth(display, screen)/2;

    int height = DisplayHeight(display, screen)/2;

// 函数DisplayWidth得到窗口的宽，DisplayHeight得到窗口的高。窗口虽然没有创建，但是有关窗口的默认信息在display打开之时就已经从X server处获得了。本例，获得默认的尺寸为1024*768，就是显示器的尺寸。



    int black_pixel = BlackPixel(display, screen);

    int white_pixel = WhitePixel(display, screen);

// X使用一种很复杂的颜色模型，每个颜色用一个整数表示。但是当机器不同，甚至程序不同时，一个整数不一定就代表固定的某个颜色。X能够保证的颜色只有两个：黑和白。用 BlackPixel和 WhitePixel可以得到这两个颜色的值。



    Window win = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, 3, black_pixel, white_pixel);

// 创建窗口的最普遍的函数是XCreateWindow和XCreateSimpleWindow（我目前还没有研究两者具体差别）。
// Window XcreateSimpleWindow(
// Display *display,
// Window parent,
// int x,
// int y,
// unsigned int width,
// unsigned int height,
// unsigned int border_width,
// unsigned long border,
// unsigned long background
// );
// 其中， RootWindow函数产生的是我们创建的窗口的父窗口（我还不太明白到底是哪个）；x, y为创建的窗口的起始坐标；width, height为窗口的尺寸；border_width为窗口边框的像素数；border为边框的颜色；background为窗口的背景色。
// 函数返回创建的窗口的ID，并使得X server产生一个CreateNotify事件。



    XSelectInput(display, win, ExposureMask|StructureNotifyMask);

// 我们知道，X是一个服务器-客户端的结构。由服务器向客户端发送事件信息，让客户端知道发生了什么事情，然后客户端告诉服务器它感兴趣的是什么事情，也就是说，客户端会对那些事件产生反应。用XSelectInput这个函数，就是告诉服务器，这个窗口会对那些消息，也就是事件有响应。这里讲的“事件”，比如又创建，画，改变大小等等。
// XselectInput(
// Display *display,
// Window w,
// long event_mask
// );
// 其中，w为希望对事件作出响应的窗口；event_mask为事件号。希望窗口会有那些响应，就在参数中加上这个事件对应的名字。

/*10.*/ XMapWindow(display, win);

// 用XCreateSimpleWindow创建窗口之后，窗口并不能显示出来，需要调用这个函数来画窗口让它显示。如果这个窗口有父窗口，那么在所有父窗口没有画出来之前，这个窗口即使用了这个函数，也是不能显示出来的。必须等所有父窗口都显示了，这个窗口才能画。（不太明白这一步）
// （没看明白在什么情况下）X server产生一个MapNotify事件。

/*11.*/ GC gc = XCreateGC(display, win, 0, NULL);

// X并不能记住要画的窗口的属性，那么每次要画窗口时都要把它的全部属性传给server，为了避免每次都传太多参数，X使用了一个结构Graphics Context，简称GC，存储图形操作的大部分属性，比如线的宽度，风格，背景色等等。

/*12.*/
    while(1)
    {

        XEvent event = {0};

        XNextEvent(display, &event);

        // test


        if(event.type == MapNotify)
        {
            break;

        }

        XDrawLine(display, win, gc, 10, 160, 180, 20);

        XFlush(display);

        //sleep(3);
    }

// 接下来就是进入事件循环了。获得事件，处理或丢弃。上面讲到，调用XMapWindow函数后，X server会发出一个MapNotify事件给客户端，这时客户端就已经有相应操作了，就是画窗口，所以事件循环里并没有写出来。



    return 0;

}

// 至此，整个程序结束。



// 编译：g++ -lX11 -o x11_test x11_test.cxx
// 运行，屏幕上就显示一个最简单的窗口了。



// 如果我们想看看MapNotify事件到底是怎么回事，就这样写：
// while(1)
// {
// XEvent event;
// XNextEvent(display, &event);
// if(event.type == MapNotify) break;
// }
// //sleep(3);
// return 0;
// 运行发现，窗口一闪而过。也就是说，客户端一接收到这个事件就显示窗口，一显示窗口就break了。把注释拿掉，则显示窗口3秒钟后才消失。
// 这里看到，接收到的事件由XNextEvent函数从消息队列里获得，把事件放到event.type里并从队列里删除该消息。
// 当队列为空也就是没有下一个事件被接收时，XNextEvent“flushes the output buffer”，
// 也就是窗口最终被显示，并且程序就一直停留在XNextEvent里直到有下一个事件，除非有跳出循环的语句。



// 继续改程序：
// while(1)
// {
// XEvent event;
// XNextEvent(display, &event);
// if(event.type == MapNotify) break;
// }
// XDrawLine(display, win, gc, 10, 160, 180, 20);
// //XFlush(display);
// sleep(3);
// return 0;
// 跳出循环后，用XDrawLine画一条线。这时运行程序，窗口上并没有出现直线。因为窗口的信息改变了，
// 就需要XFlush函数来flush一下，让窗口重画。而之前之所以没有XFlush函数，
// 是因为XNextEvent函数隐式地调用XFlush了，而且调用后并没有改变窗口信息。



// 继续改，添加事件：
// XSelectInput(display, win, ExposureMask|KeyPressMask|StructureNotifyMask);
// while(1)
// {
// XEvent event = {0};
// XNextEvent(display, &event);
// switch(event.type)
// {
// case ConfigureNotify:
// {
// width = event.xconfigure.width;
// height = event.xconfigure.height;
// break;
// }
// case Expose:
// {
// XSetForeground(display, gc, WhitePixel(display, screen));
// XFillRectangle(display, win, gc, 0, 0, width, height);
// XSetForeground(display, gc, BlackPixel(display, screen));
// XDrawString(display, win, gc, width/2, height/2, "XWindow", 7);
// break;
// }
// case KeyPress:
// {
// if(event.xkey.keycode == XKeysymToKeycode(display, XK_Escape))
// {
// XFreeGC(display, gc);
// XCloseDisplay(display);
// return 0;
// }
// }
// default: break;
// }
// }
// return 0;
// 事件类型StructureNotifyMask对应事件ConfigureNotify，即改变窗口状态，比如尺寸，位置等；
// ExposureMask对应事件Expose（我不知道怎么解释这个事件），本例中是在这里设置窗口颜色，画字符串并设置字符串颜色；
// KeyPressMask对应KeyPress，即键盘响应，本例是当Escape键按下时，退出窗口。退出窗口时，
// 需要释放或者说销毁GC，最开始曾打开display，在这里需要关闭和X server的连接，于是也就销毁了相关资源，关闭了窗口。

// 在这种循环的写法中，退出switch，并没有退出整个循环，XNextEvent仍然在显示窗口，所以不需要我们自己调用XFlush。




/*
 * this union is defined so Xlib can always use the same sized
 * event structure internally, to avoid memory fragmentation.
 */
//typedef union _XEvent {
//    int type;		// must not be changed; first element
//    XAnyEvent xany;
//    XKeyEvent xkey;
//    XButtonEvent xbutton;
//    XMotionEvent xmotion;
//    XCrossingEvent xcrossing;
//    XFocusChangeEvent xfocus;
//    XExposeEvent xexpose;
//    XGraphicsExposeEvent xgraphicsexpose;
//    XNoExposeEvent xnoexpose;
//    XVisibilityEvent xvisibility;
//    XCreateWindowEvent xcreatewindow;
//    XDestroyWindowEvent xdestroywindow;
//    XUnmapEvent xunmap;
//    XMapEvent xmap;
//    XMapRequestEvent xmaprequest;
//    XReparentEvent xreparent;
//    XConfigureEvent xconfigure;
//    XGravityEvent xgravity;
//    XResizeRequestEvent xresizerequest;
//    XConfigureRequestEvent xconfigurerequest;
//    XCirculateEvent xcirculate;
//    XCirculateRequestEvent xcirculaterequest;
//    XPropertyEvent xproperty;
//    XSelectionClearEvent xselectionclear;
//    XSelectionRequestEvent xselectionrequest;
//    XSelectionEvent xselection;
//    XColormapEvent xcolormap;
//    XClientMessageEvent xclient;
//    XMappingEvent xmapping;
//    XErrorEvent xerror;
//    XKeymapEvent xkeymap;
//    XGenericEvent xgeneric;
//    XGenericEventCookie xcookie;
//    long pad[24];
//} XEvent;




//XSendEvent

