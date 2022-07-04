// ////
// //// Created by chensong on 6/20/22.
// ////
// //
// //#include <X11/Xlib.h>
// //// 每一个Xlib 程序都必须包含这个头文件
// //#include <stdio.h>
// //#include <sys/select.h>
// //#include <stdlib.h>
// //#include <stdio.h>
// //#include <string.h>
// //#include <strings.h>
// //#include <unistd.h>
// //#include <regex.h>
// //#include <ctype.h>
// //#include <locale.h>
// //#include <stdarg.h>
// //
// //#include <X11/Xlib.h>
// //#include <X11/XKBlib.h>
// //#include <X11/Xatom.h>
// //#include <X11/Xresource.h>
// //#include <X11/Xutil.h>
// //#include <X11/extensions/XTest.h>
// //#include <X11/extensions/Xinerama.h>
// //#include <X11/extensions/XInput2.h>
// //#include <X11/keysym.h>
// //#include <X11/cursorfont.h>
// //
// //#include <xkbcommon/xkbcommon.h>
// //
// //// #include "xdo.h"
// //// #include "xdo_util.h"
// //// #include "xdo_version.h"
// //
// //int main(int argc, char *argv[])
// //{
// //
// //    // 首先打开与server 的连接。在你的程序可以使用display 之前，必须先建立一个和X server 的连接。这个连接建立以后，就可以使用Xlib 的函数或宏定义来获得display 的信息了。
// //// 参数为hardware display name，当设为NULL时，为默认的DISPLAY环境变量。这个函数返回一个指向Display类型的结构的指针，表明已与X server建立了连接，并且包含了这个X serer的所有信息。
// //// 这样我们就可以使用display之上的所有窗口了。
// //
// //    Display *display = XOpenDisplay(NULL);
// //
// //
// //
// //
// //
// //    int screen = DefaultScreen(display);
// //
// //// 得到display打开之后的窗口号。本例中，得到0。我理解是：尚未创建窗口，所以窗口号为0。不知道对不对。
// //// DefaultScreen是个宏，对应的函数为XDefaultScreen。两者作用相同。
// //
// //
// //
// //    int width = DisplayWidth(display, screen)/2;
// //
// //    int height = DisplayHeight(display, screen)/2;
// //
// //// 函数DisplayWidth得到窗口的宽，DisplayHeight得到窗口的高。窗口虽然没有创建，但是有关窗口的默认信息在display打开之时就已经从X server处获得了。本例，获得默认的尺寸为1024*768，就是显示器的尺寸。
// //
// //
// //
// //    int black_pixel = BlackPixel(display, screen);
// //
// //    int white_pixel = WhitePixel(display, screen);
// //
// //// X使用一种很复杂的颜色模型，每个颜色用一个整数表示。但是当机器不同，甚至程序不同时，一个整数不一定就代表固定的某个颜色。X能够保证的颜色只有两个：黑和白。用 BlackPixel和 WhitePixel可以得到这两个颜色的值。
// //
// //
// //
// //    Window win = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, 3, black_pixel, white_pixel);
// //
// //// 创建窗口的最普遍的函数是XCreateWindow和XCreateSimpleWindow（我目前还没有研究两者具体差别）。
// //// Window XcreateSimpleWindow(
// //// Display *display,
// //// Window parent,
// //// int x,
// //// int y,
// //// unsigned int width,
// //// unsigned int height,
// //// unsigned int border_width,
// //// unsigned long border,
// //// unsigned long background
// //// );
// //// 其中， RootWindow函数产生的是我们创建的窗口的父窗口（我还不太明白到底是哪个）；x, y为创建的窗口的起始坐标；width, height为窗口的尺寸；border_width为窗口边框的像素数；border为边框的颜色；background为窗口的背景色。
// //// 函数返回创建的窗口的ID，并使得X server产生一个CreateNotify事件。
// //
// //
// //
// //    XSelectInput(display, win, ExposureMask|StructureNotifyMask);
// //
// //// 我们知道，X是一个服务器-客户端的结构。由服务器向客户端发送事件信息，让客户端知道发生了什么事情，然后客户端告诉服务器它感兴趣的是什么事情，也就是说，客户端会对那些事件产生反应。用XSelectInput这个函数，就是告诉服务器，这个窗口会对那些消息，也就是事件有响应。这里讲的“事件”，比如又创建，画，改变大小等等。
// //// XselectInput(
// //// Display *display,
// //// Window w,
// //// long event_mask
// //// );
// //// 其中，w为希望对事件作出响应的窗口；event_mask为事件号。希望窗口会有那些响应，就在参数中加上这个事件对应的名字。
// //
// ///*10.*/ XMapWindow(display, win);
// //
// //// 用XCreateSimpleWindow创建窗口之后，窗口并不能显示出来，需要调用这个函数来画窗口让它显示。如果这个窗口有父窗口，那么在所有父窗口没有画出来之前，这个窗口即使用了这个函数，也是不能显示出来的。必须等所有父窗口都显示了，这个窗口才能画。（不太明白这一步）
// //// （没看明白在什么情况下）X server产生一个MapNotify事件。
// //
// ///*11.*/ GC gc = XCreateGC(display, win, 0, NULL);
// //
// //// X并不能记住要画的窗口的属性，那么每次要画窗口时都要把它的全部属性传给server，为了避免每次都传太多参数，X使用了一个结构Graphics Context，简称GC，存储图形操作的大部分属性，比如线的宽度，风格，背景色等等。
// //
// ///*12.*/
// //    while(1)
// //    {
// //
// //        XEvent event = {0};
// //
// //        XNextEvent(display, &event);
// //
// //        // test
// //
// //
// //        if(event.type == MapNotify)
// //        {
// //            //break;
// //
// //        }
// //
// //        XDrawLine(display, win, gc, 10, 160, 180, 20);
// //
// //        XFlush(display);
// //        Display *display__ = XOpenDisplay(NULL);
// //
// //
// //
// //
// //
// //        int screen___ = DefaultScreen(display__);
// //        printf("screen___ = %u\n", screen___);
// //        XCloseDisplay(display__);
// //        sleep(3);
// //    }
// //
// //// 接下来就是进入事件循环了。获得事件，处理或丢弃。上面讲到，调用XMapWindow函数后，X server会发出一个MapNotify事件给客户端，这时客户端就已经有相应操作了，就是画窗口，所以事件循环里并没有写出来。
// //
// //
// //
// //    return 0;
// //
// //}
// //
// //// 至此，整个程序结束。
// //
// //
// //
// //// 编译：g++ -lX11 -o x11_test x11_test.cxx
// //// 运行，屏幕上就显示一个最简单的窗口了。
// //
// //
// //
// //// 如果我们想看看MapNotify事件到底是怎么回事，就这样写：
// //// while(1)
// //// {
// //// XEvent event;
// //// XNextEvent(display, &event);
// //// if(event.type == MapNotify) break;
// //// }
// //// //sleep(3);
// //// return 0;
// //// 运行发现，窗口一闪而过。也就是说，客户端一接收到这个事件就显示窗口，一显示窗口就break了。把注释拿掉，则显示窗口3秒钟后才消失。
// //// 这里看到，接收到的事件由XNextEvent函数从消息队列里获得，把事件放到event.type里并从队列里删除该消息。
// //// 当队列为空也就是没有下一个事件被接收时，XNextEvent“flushes the output buffer”，
// //// 也就是窗口最终被显示，并且程序就一直停留在XNextEvent里直到有下一个事件，除非有跳出循环的语句。
// //
// //
// //
// //// 继续改程序：
// //// while(1)
// //// {
// //// XEvent event;
// //// XNextEvent(display, &event);
// //// if(event.type == MapNotify) break;
// //// }
// //// XDrawLine(display, win, gc, 10, 160, 180, 20);
// //// //XFlush(display);
// //// sleep(3);
// //// return 0;
// //// 跳出循环后，用XDrawLine画一条线。这时运行程序，窗口上并没有出现直线。因为窗口的信息改变了，
// //// 就需要XFlush函数来flush一下，让窗口重画。而之前之所以没有XFlush函数，
// //// 是因为XNextEvent函数隐式地调用XFlush了，而且调用后并没有改变窗口信息。
// //
// //
// //
// //// 继续改，添加事件：
// //// XSelectInput(display, win, ExposureMask|KeyPressMask|StructureNotifyMask);
// //// while(1)
// //// {
// //// XEvent event = {0};
// //// XNextEvent(display, &event);
// //// switch(event.type)
// //// {
// //// case ConfigureNotify:
// //// {
// //// width = event.xconfigure.width;
// //// height = event.xconfigure.height;
// //// break;
// //// }
// //// case Expose:
// //// {
// //// XSetForeground(display, gc, WhitePixel(display, screen));
// //// XFillRectangle(display, win, gc, 0, 0, width, height);
// //// XSetForeground(display, gc, BlackPixel(display, screen));
// //// XDrawString(display, win, gc, width/2, height/2, "XWindow", 7);
// //// break;
// //// }
// //// case KeyPress:
// //// {
// //// if(event.xkey.keycode == XKeysymToKeycode(display, XK_Escape))
// //// {
// //// XFreeGC(display, gc);
// //// XCloseDisplay(display);
// //// return 0;
// //// }
// //// }
// //// default: break;
// //// }
// //// }
// //// return 0;
// //// 事件类型StructureNotifyMask对应事件ConfigureNotify，即改变窗口状态，比如尺寸，位置等；
// //// ExposureMask对应事件Expose（我不知道怎么解释这个事件），本例中是在这里设置窗口颜色，画字符串并设置字符串颜色；
// //// KeyPressMask对应KeyPress，即键盘响应，本例是当Escape键按下时，退出窗口。退出窗口时，
// //// 需要释放或者说销毁GC，最开始曾打开display，在这里需要关闭和X server的连接，于是也就销毁了相关资源，关闭了窗口。
// //
// //// 在这种循环的写法中，退出switch，并没有退出整个循环，XNextEvent仍然在显示窗口，所以不需要我们自己调用XFlush。
// //
// //
// //
// //
// ///*
// // * this union is defined so Xlib can always use the same sized
// // * event structure internally, to avoid memory fragmentation.
// // */
// ////typedef union _XEvent {
// ////    int type;		// must not be changed; first element
// ////    XAnyEvent xany;
// ////    XKeyEvent xkey;
// ////    XButtonEvent xbutton;
// ////    XMotionEvent xmotion;
// ////    XCrossingEvent xcrossing;
// ////    XFocusChangeEvent xfocus;
// ////    XExposeEvent xexpose;
// ////    XGraphicsExposeEvent xgraphicsexpose;
// ////    XNoExposeEvent xnoexpose;
// ////    XVisibilityEvent xvisibility;
// ////    XCreateWindowEvent xcreatewindow;
// ////    XDestroyWindowEvent xdestroywindow;
// ////    XUnmapEvent xunmap;
// ////    XMapEvent xmap;
// ////    XMapRequestEvent xmaprequest;
// ////    XReparentEvent xreparent;
// ////    XConfigureEvent xconfigure;
// ////    XGravityEvent xgravity;
// ////    XResizeRequestEvent xresizerequest;
// ////    XConfigureRequestEvent xconfigurerequest;
// ////    XCirculateEvent xcirculate;
// ////    XCirculateRequestEvent xcirculaterequest;
// ////    XPropertyEvent xproperty;
// ////    XSelectionClearEvent xselectionclear;
// ////    XSelectionRequestEvent xselectionrequest;
// ////    XSelectionEvent xselection;
// ////    XColormapEvent xcolormap;
// ////    XClientMessageEvent xclient;
// ////    XMappingEvent xmapping;
// ////    XErrorEvent xerror;
// ////    XKeymapEvent xkeymap;
// ////    XGenericEvent xgeneric;
// ////    XGenericEventCookie xcookie;
// ////    long pad[24];
// ////} XEvent;
// //
// //
// //
// //
// ////XSendEvent
// //
// /* $Header: /mit/adedev/shX/src/talkbox/RCS/xtalk.c,v 1.1.1.1 91/03/21 17:00:33 swick Exp $ */

// /* file xtalk.c

// 	Written by : Yaser Doleh
// 	Kent State University
// 	email doleh@kent.edu
// 	March 15, 1980
// */

// //#include <X11/Intrinsic.h>
// //#include <X11/StringDefs.h>
// //#include <X11/Cardinals.h>
// //#include <X11/AsciiText.h>
// //#include <X11/Box.h>
// //#include <X11/Command.h>
// //#include <X11/Label.h>
// //#include <X11/Shell.h>
// //#include <X11/cursorfont.h>
// //#include <stdio.h>
// //#include <stdlib.h>
// //#define	MAXSIZE	4096
// //#define WIDTH	500
// //#define	HEIGHT	200
// //
// //#ifndef NO_BC
// //#define XawTextReplace	XtTextReplace
// //#define XawtextEdit	XttextEdit
// //#define XawTextBlock	XtTextBlock
// //#endif
// //
// //XtAppContext	xtalk;
// //Display		*dpy1, *dpy2;
// //Widget	toplevel1, toplevel2, top1, top2, box1, box2, send1, send2, rec1, rec2;
// //char	sbuf1[MAXSIZE], sbuf2[MAXSIZE], rbuf1[MAXSIZE], rbuf2[MAXSIZE];
// //
// //static XrmOptionDescRec options[] = {
// //        { "-geometry",	"xtalk-originator.geometry", XrmoptionSepArg, NULL }
// //};
// //
// //static void xtalk_exit()
// //{
// //    exit(0);
// //}
// //
// //static void Beep(w, event, params, nparams)
// //Widget w;
// //XEvent *event;
// //String *params;
// //Cardinal *nparams;
// //{
// //XBell(XtDisplay(w), 0);
// //}
// //
// //main(argc,argv)
// //int	argc;
// //char	*argv[];
// //{
// //Arg	args[10];
// //int	num_args;
// //static XtCallbackRec	callback[]={
// //        { xtalk_exit, NULL },
// //        { NULL, NULL },
// //};
// //
// //XtTranslations	trans;
// //static char trans_string[] = "<Key>: beep()\n";
// //
// //static XtActionsRec actionsList[] = {
// //        { "beep", Beep },
// //};
// //char title[128];
// //char *userName = (char*)getenv("USER");
// //
// //XtToolkitInitialize();
// //xtalk = XtCreateApplicationContext();
// //dpy1 = XtOpenDisplay(xtalk, NULL, NULL, "XTalk",
// //                     options, XtNumber(options), &argc, argv);
// //if (dpy1 == NULL) {
// //fprintf( stderr, "%s: couldn't open local display\n", argv[0] );
// //exit( 0 );
// //}
// //if (argc == 2) {
// //dpy2 = XtOpenDisplay(xtalk, argv[1], NULL, "XTalk",
// //                     options, XtNumber(options), &argc, argv);
// //if (dpy2 == NULL) {
// //fprintf( stderr, "%s: couldn't open remote display %s\n",
// //argv[0], argv[1] );
// //exit( 0 );
// //}
// //}
// //else {
// //fprintf( stderr, "Usage: %s other-display [toolkitoptions]\n",
// //argv[0] );
// //exit( 0 );
// //}
// //
// //toplevel1 = XtAppCreateShell("xtalk", "XTalk", applicationShellWidgetClass,
// //                             dpy1, NULL, 0);
// //
// //toplevel2 = XtAppCreateShell("xtalk", "XTalk", applicationShellWidgetClass,
// //                             dpy2, NULL, 0);
// //
// //sprintf( title, "xtalk to %.32s", DisplayString(dpy2) );
// //XtSetArg( args[0], XtNtitle, title );
// //top1 = XtCreatePopupShell("xtalk-originator", topLevelShellWidgetClass,
// //                          toplevel1, args, ONE);
// //
// //sprintf( title, "xtalk from %.32s at %.32s",
// //(userName != NULL) ? userName : "unknown user",
// //DisplayString(dpy1)
// //);
// //top2 = XtCreatePopupShell("xtalk-recipient", topLevelShellWidgetClass,
// //                          toplevel2, args, ONE);
// //
// //box1 = XtCreateManagedWidget("box1", boxWidgetClass, top1, NULL, 0);
// //box2 = XtCreateManagedWidget("box2", boxWidgetClass, top2, NULL, 0);
// //XtSetArg( args[0], XtNlabel, DisplayString(dpy2) );
// //XtCreateManagedWidget("user1", labelWidgetClass, box1, args, ONE);
// //sprintf( title, "%.32s@%.32s",
// //(userName != NULL) ? userName : "unknown user",
// //DisplayString(dpy1)
// //);
// //XtSetArg( args[0], XtNlabel, title );
// //XtCreateManagedWidget("user2", labelWidgetClass, box2, args, ONE);
// //XtSetArg( args[0], XtNlabel, " Place pointer in top window and type ");
// //XtCreateManagedWidget("label1", labelWidgetClass, box1, args, 1);
// //XtCreateManagedWidget("label2", labelWidgetClass, box2, args, 1);
// //
// //XtSetArg( args[0], XtNlabel, " EXIT ");
// //XtSetArg( args[1], XtNcallback, callback);
// //XtCreateManagedWidget("exit1", commandWidgetClass, box1, args, 2);
// //XtCreateManagedWidget("exit2", commandWidgetClass, box2, args, 2);
// //
// //XtSetArg( args[0], XtNstring, sbuf1);
// //XtSetArg( args[1], XtNwidth, WIDTH);
// //XtSetArg( args[2], XtNheight, HEIGHT);
// //XtSetArg( args[3], XtNeditType, XawtextEdit);
// //XtSetArg( args[4], XtNlength, MAXSIZE);
// //send1 = XtCreateManagedWidget("send1", asciiStringWidgetClass,
// //                              box1, args, FIVE);
// //
// //XtSetArg( args[0], XtNstring, rbuf1);
// //rec1 = XtCreateManagedWidget("rec1", asciiStringWidgetClass,
// //                             box1, args, FIVE);
// //
// //XtSetArg( args[0], XtNstring, sbuf2);
// //send2 = XtCreateManagedWidget("send2", asciiStringWidgetClass,
// //                              box2, args, FIVE);
// //
// //XtSetArg( args[0], XtNstring, rbuf2);
// //rec2 = XtCreateManagedWidget("rec2", asciiStringWidgetClass,
// //                             box2, args, FIVE);
// //
// //{
// ///* work around bug in XmuCvtStringToCursor */
// //Cursor cursor1, cursor2;
// //Arg newArgs[1];
// //Boolean doneSend = False, doneRcv = False;
// //XtSetArg(newArgs[0], XtNcursor, XCreateFontCursor(dpy2, XC_xterm));
// //XtSetArg( args[0], XtNcursor, &cursor1 );
// //XtGetValues( send1, args, ONE );
// //XtSetArg( args[0], XtNcursor, &cursor2 );
// //XtGetValues( send2, args, ONE );
// //if (cursor1 == cursor2) {
// //XtSetValues( send2, newArgs, ONE );
// //doneSend = True;
// //}
// //XtGetValues( rec2, args, ONE );
// //if (cursor1 == cursor2) {
// //XtSetValues( rec2, newArgs, ONE );
// //doneRcv = True;
// //}
// //if (!doneSend || !doneRcv) {
// //XtGetValues( rec1, args, ONE );
// //if (cursor1 != cursor2) {
// //XtSetArg( args[0], XtNcursor, &cursor1 );
// //if (!doneSend) {
// //XtGetValues( send2, args, ONE );
// //if (cursor1 == cursor2)
// //XtSetValues( send2, newArgs, ONE );
// //}
// //if (!doneRcv) {
// //XtGetValues( rec2, args, ONE );
// //if (cursor1 == cursor2)
// //XtSetValues( rec2, newArgs, ONE );
// //}
// //}
// //}
// //}
// //XtRealizeWidget(top1);
// //XtRealizeWidget(top2);
// //
// //XtAppAddActions(xtalk, actionsList, XtNumber(actionsList));
// //trans = XtParseTranslationTable(trans_string);
// //XtOverrideTranslations(rec1, trans);
// //XtOverrideTranslations(rec2, trans);
// //
// //XtPopup(top1, XtGrabNone);
// //XtPopup(top2, XtGrabNone);
// //
// //while(1) {
// //XEvent	event;
// //XawTextBlock	text;
// //
// //XtAppNextEvent(xtalk, &event);
// //XtDispatchEvent(&event);
// //if (event.type == KeyPress
// //|| event.type == KeyRelease
// //|| event.type == ButtonPress
// //|| event.type == ButtonRelease)
// //{
// //text.firstPos = 0;
// //text.ptr = sbuf1;
// //text.length = strlen(sbuf1);
// //XawTextReplace(rec2, 0, text.length, &text);
// //text.firstPos = 0;
// //text.ptr = sbuf2;
// //text.length = strlen(sbuf2);
// //XawTextReplace(rec1, 0, text.length, &text);
// //}
// //}
// //
// //
// //
// //}



// // #include <stdlib.h>
// // #include <stdio.h>

// // #include <sys/ipc.h>
// // #include <sys/shm.h>

// // #include <xcb/xcb.h>
// // #include <xcb/shm.h>
// // #include <xcb/xcb_image.h>


// // #define WID 512
// // #define HEI 512



// // int main(){
// //     xcb_connection_t*       connection;
// //     xcb_window_t            window;
// //     xcb_screen_t*           screen;
// //     xcb_gcontext_t          gcontext;
// //     xcb_generic_event_t*    event;

// //     uint32_t value_mask;
// //     uint32_t value_List[2];

// //     //connect to the X server and get screen

// //     connection = xcb_connect(NULL,NULL);
// //     screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

// //     //create a window

// //     value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
// //     value_List[0] = screen->black_pixel;
// //     value_List[1] = XCB_EVENT_MASK_EXPOSURE;

// //     window = xcb_generate_id(connection);

// //     xcb_create_window(
// //         connection,screen->root_depth,window,screen->root,WID,HEI,XCB_WINDOW_CLASS_input_OUTPUT,screen->root_visual,value_mask,value_List
// //     );

// //     //create a graphic context

// //     value_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
// //     value_List[0] = screen->black_pixel;
// //     value_List[1] = 0;

// //     gcontext = xcb_generate_id(connection);
// //     xcb_create_gc(connection, gcontext, value_List);

// //     //map the window onto the screen

// //     xcb_map_window(connection,window);
// //     xcb_flush(connection);


// //     //Shm test
// //     xcb_shm_query_version_reply_t*  reply;
// //     xcb_shm_segment_info_t          info;

// //     reply = xcb_shm_query_version_reply(
// //         connection,xcb_shm_query_version(connection),NulL
// //     );

// //     if(!reply || !reply->shared_pixmaps){
// //         printf("Shm error...\n");
// //         exit(0);
// //     }

// //     info.shmid   = shmget(IPC_PRIVATE,WID*HEI*4,IPC_CREAT | 0777);
// //     info.shmaddr = shmat(0, info.shmid, 0);

// //     info.shmseg = xcb_generate_id(connection);
// //     xcb_shm_attach(connection,info.shmseg,info.shmid,0);
// //     shmctl(info.shmid,IPC_RMID,0);

// //     uint8_t* data = info.shmaddr;

// //     xcb_pixmap_t pix = xcb_generate_id(connection);
// //     xcb_shm_create_pixmap(
// //         connection,pix,0
// //     );

// //     int i = 0;
// //     while(1)
// //     {
// //         usleep(10000);

// //         data[i] = 0xFFFFFF;
// //         i++;

// //         xcb_copy_area(  connection,HEI );

// //         xcb_flush(connection);
// //     }

// //     xcb_shm_detach(connection,info.shmseg);
// //     shmdt(info.shmaddr);

// //     xcb_free_pixmap(connection,pix);

// //     xcb_destroy_window(connection,window);
// //     xcb_disconnect(connection);

// //     return 0;
// // }


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "get_processid.h"
// #include <xcb/xcb.h>
// #include <thread>


// #include <X11/Xlib.h>
// #include <X11/Xatom.h>
// #include <iostream>
// #include <list>
// #include <stdlib.h>
// #include <unistd.h>



// class WindowsMatchingPid
// {
// public:
//     WindowsMatchingPid(Display *display, Window wRoot, unsigned long pid)
//         : _display(display),
//           _pid(pid)
//     {
//         // Get the PID property atom.
//         _atomPID = XInternAtom(display, "_NET_CLIENT_LIST", true);
//         if(_atomPID == None)
//         {
//             std::cout << "No such atom" << std::endl;
//             return;
//         }
//         std::cout << "==============================>" << std::endl;
//         search(wRoot);
//     }

//     const std::list<Window> &result() const { return _result; }
//     void show()
//     {

//         for (const Window & win : _result)
//         {
//             std::cout << "==win = " << win << std::endl;
//         }

//     }
// private:
//     unsigned long  _pid;
//     Atom           _atomPID;
//     Display       *_display;
//     std::list<Window>   _result;

//     void
//     search(Window w)
//     {
//         // Get the PID for the current Window.
//         Atom           type;
//         int            format;
//         unsigned long  nItems;
//         unsigned long  bytesAfter;
//         unsigned char *propPID = 0;
//         if(Success == XGetWindowProperty(
//                     _display,
//                     w,
//                     _atomPID,
//                     0,
//                     1,
//                     False,
//                     XA_CARDINAL,
//                     &type,
//                     &format,
//                     &nItems,
//                     &bytesAfter,
//                     &propPID))
//         {
//             // std::cout << "success !!! " << std::endl;
//             if(propPID != 0)
//             {
//                 // If the PID matches, add this window to the result set.
//                 if(_pid == *((unsigned long *)propPID))
//                 {
//                     std::cout  << "pushback iiii " << std::endl;
//                     _result.push_back(w);
//                 }

//                 XFree(propPID);
//             }
//         }

//         // Recurse into child windows.
//         Window    wRoot;
//         Window    wParent;
//         Window   *wChild;
//         unsigned  nChildren;
//         if(0 != XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren))
//         {
//             for(unsigned i = 0; i < nChildren; i++)
//                 search(wChild[i]);
//         }

//        // XFree(propPID);
//     }
// };


// //getpid()
// unsigned long get_win_id_from_pid(int pid)
// {
//     std::cout << "Searching for windows associated with PID " << pid << std::endl;

//     // Start with the root window.
//     Display *display = XOpenDisplay(0);

//     WindowsMatchingPid match(display, XDefaultRootWindow(display), pid);

//     // Print the result.
//     const std::list<Window> &result = match.result();

//     long win_id = 0;
//  match.show();
//     // for(std::list<Window>::const_iterator pos = result.begin(); pos != result.end(); pos++)
//     // {
//     //     std::cout << "Window #" << (unsigned long)(*pos) << std::endl;
//     //     win_id = (unsigned long)(*pos);

//     //     //break;
//     // }
// // #else
// //    
// // #endif
//  win_id = (long)(*result.begin());
//     std::cout << "Window id: "<< win_id << std::endl;
//     return win_id;
// }


// void cur_process_window_id()
// {
//  //sleep(3);
//             printf("get winid !!!!\n");
//            // get_win_id_from_pid();
//    unsigned long win_id =  get_win_id_from_pid(getppid());
//    printf("win id = %u\n", win_id);
// // }
// }


// /*
//   * 下边是编译命令
//   * gcc x11.c -o output -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
//   */
 
// #include <X11/Xlib.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
 
// int test_wwdddmain(void)
// {
//     Display *display;
//     Window window;
//     XEvent event;
//     char *msg = "你好 ,这是一个测试窗口!";
//     int s;
 
//     /* 与Xserver建立连接 */
//     display = XOpenDisplay(NULL);
//     if (display == NULL)
//     {
//         fprintf(stderr, "Cannot open display\n");
//         exit(1);
//     }
 
        
//     s = DefaultScreen(display);
 
//     /* 创建一个窗口 */
//     window = XCreateSimpleWindow(display, RootWindow(display, s), 10, 10, 200, 200, 1,
//                            BlackPixel(display, s), WhitePixel(display, s));
 
//     /* 选择一种感兴趣的事件进行监听 */
//     XSelectInput(display, window, ExposureMask | KeyPressMask);
 
//     /* 显示窗口 */
//     XMapWindow(display, window);
  
//     /* 事件遍历 */
//     for (;;)
//     {
//         XNextEvent(display, &event);
 
//         /* 绘制窗口或者重新绘制 */
//         if (event.type == Expose)
//         {
//             XFillRectangle(display, window, DefaultGC(display, s), 20, 20, 10, 10);
//             XDrawString(display, window, DefaultGC(display, s), 50, 50, msg, strlen(msg));
//         }
//          std::thread myThread(cur_process_window_id);
//   myThread.join();//分离线程，不阻塞
//         /* 当检测到键盘按键,退出消息循环 */
//         if (event.type == KeyPress)
//             break;
//     }
 
//     /* 关闭与Xserver服务器的连接 */
//     XCloseDisplay(display);
 
//     return 0;
//  }

// int test_window_old_main(void) {
//     xcb_connection_t    *pConn;
//     xcb_screen_t        *pScreen;
//     xcb_window_t        window;
//     xcb_gcontext_t      foreground;
//     xcb_gcontext_t      background;
//     xcb_generic_event_t *pEvent;
//     uint32_t        mask = 0;
//     uint32_t        values[2];
//     uint8_t         isQuit = 0;

//     char title[] = "Hello, Engine!";
//     char title_icon[] = "Hello, Engine! (iconified)";

//     /* 第一步：创建连接 */
//     //  建立与X服务器的连接
//     pConn = xcb_connect(0, 0);

//     /* 第二步：获取屏幕 */        
//     // xcb_get_setup函数用于从X服务器获取数据，获取的数据包括服务器支持的图像格式，
//     // 可显示的屏幕列表，可用的视觉效果列表，服务器的最大请求长度等等
//     // xcb_setup_roots_iterator函数只查到原型，没有函数的说明，从函数名和使用方式
//     // 上看，应该是查找数据用的。
//     pScreen = xcb_setup_roots_iterator(xcb_get_setup(pConn)).data;

//     /* 第三步：创建上下文 */
//     // 先获取根窗口
//     window = pScreen->root;

//     // 创建前景上下文（黑色）
//     foreground = xcb_generate_id(pConn);  // 生成上下文的ID
//     mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;  // 上下文的用途，前景&需要事件
//     values[0] = pScreen->black_pixel; // 填充颜色（黑色）
//     values[1] = 0; // 结束标志
//     xcb_create_gc(pConn, foreground, window, mask, values); // 创建上下文

//     // 创建背景上下文（白色）
//     background = xcb_generate_id(pConn); // 生成上下文ID
//     mask = XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES; // 上下文用途，前景&需要事件
//     values[0] = pScreen->white_pixel; // 填充颜色（白色）
//     values[1] = 0; // 结束标志
//     xcb_create_gc(pConn, background, window, mask, values); // 创建上下文

//     /* 第四步：创建窗口 */
//     window = xcb_generate_id(pConn); // 创建窗口ID
//     mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK; // 覆盖BackPixmap，需要指定的事件
//     values[0] = pScreen->white_pixel; // 白色填充
//     values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS; // 需要EXPOSE事件和按键事件
//     xcb_create_window (pConn,                   // 连接
//                        XCB_COPY_FROM_PARENT,    // 深度值
//                        window,                  // 窗口ID
//                        pScreen->root,           // 父窗口，屏幕的根窗口
//                        20, 20,                  // x，y坐标
//                        640, 480,                // 宽度，高度
//                        10,                      // 边缘宽度
//                        XCB_WINDOW_CLASS_INPUT_OUTPUT, // 要么是0，要么是一些指定的值
//                        pScreen->root_visual,    // 视觉效果，暂时不知道是啥玩意
//                        mask, values);           // 需要的功能与值设定

//     // 设置窗口名
//     xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window,
//                 XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
//                 strlen(title), title);

//     // 设置窗口图标
//     xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window,
//                 XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8,
//                 strlen(title_icon), title_icon);

//     /* 第五步：关联窗口和连接 */
//     xcb_map_window(pConn, window);

//     xcb_flush(pConn); // 刷新
//     std::thread myThread(cur_process_window_id);
//   myThread.detach();//分离线程，不阻塞
//     // std::thread([]()
//     //     {
           
//     //         cur_process_window_id();
//     //     }).deatch();
//     /* 第六步：处理事件 */
//     while((pEvent = xcb_wait_for_event(pConn)) && !isQuit) {
//         switch(pEvent->response_type & ~0x80) {
//         case XCB_EXPOSE: // 绘制或重绘窗口
//             {       
//             xcb_rectangle_t rect = { 20, 20, 60, 80 };
//             xcb_poly_fill_rectangle(pConn, window, foreground, 1, &rect); // 绘制一块矩形区域
//             xcb_flush(pConn); // 刷新
//             }
//             break;
//         case XCB_KEY_PRESS: // 按键
//             isQuit = 1;
//             break;
//         }
//         free(pEvent);
//     }

//     xcb_disconnect(pConn); // 断开连接

//     return 0;
// }





#include <X11/Xutil.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/composite.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/composite.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s windowId\n", argv[0]);
        return EXIT_FAILURE;
    }
   Display * disp = XOpenDisplay(NULL);
    xcb_window_t req_win_id = strtoul(argv[1], NULL, 0);
    xcb_connection_t *connection = XGetXCBConnection(disp);//xcb_connect(NULL, NULL);
    xcb_generic_error_t *err = NULL, *err2 = NULL;

    xcb_composite_query_version_cookie_t comp_ver_cookie = xcb_composite_query_version(connection, 0, 2);
    xcb_composite_query_version_reply_t *comp_ver_reply = xcb_composite_query_version_reply(connection, comp_ver_cookie, &err);
    if (comp_ver_reply)
    {
        if (comp_ver_reply->minor_version < 2) {
            fprintf(stderr, "query composite failure: server returned v%d.%d\n", comp_ver_reply->major_version, comp_ver_reply->minor_version);
            free(comp_ver_reply);
            return EXIT_FAILURE;
        }
        free(comp_ver_reply);
    }
    else if (err)
    {
        fprintf(stderr, "xcb error: %d\n", err->error_code);
        free(err);
        return EXIT_FAILURE;
    }

    const xcb_setup_t *setup = xcb_get_setup(connection);
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen = screen_iter.data;
    // request redirection of window
    xcb_composite_redirect_window(connection, req_win_id, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
    int win_h, win_w, win_d;

    xcb_get_geometry_cookie_t gg_cookie = xcb_get_geometry(connection, req_win_id);
    xcb_get_geometry_reply_t *gg_reply = xcb_get_geometry_reply(connection, gg_cookie, &err);
    if (gg_reply) {
        win_w = gg_reply->width;
        win_h = gg_reply->height;
        win_d = gg_reply->depth;
        free(gg_reply);
    } else {
        if (err) {
            fprintf(stderr, "get geometry: XCB error %d\n", err->error_code);
            free(err);
        }
        return EXIT_FAILURE;
    }

    // create a pixmap
    xcb_pixmap_t win_pixmap = xcb_generate_id(connection);
    xcb_void_cookie_t name_cookie = xcb_composite_name_window_pixmap(connection, req_win_id, win_pixmap);

     err = NULL;
    if ((err = xcb_request_check(connection, name_cookie)) != NULL) 
    {
        printf("xcb_composite_name_window_pixmap failed\n");
        
        return -1;
    }
    xcb_map_window(connection, req_win_id);

    xcb_flush(connection);
    FILE *out_file_ptr = fopen("./chensong.yuv", "wb+");


std::chrono::steady_clock::time_point cur_time_ms;
        std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration dur;
        std::chrono::milliseconds ms;
        uint32_t elapse = 0;

    while (true)
    {
        static uint64_t frame_count = 0;
        // get the image
        pre_time = std::chrono::steady_clock::now();
        xcb_get_image_cookie_t gi_cookie = xcb_get_image(connection, XCB_IMAGE_FORMAT_Z_PIXMAP, win_pixmap, 0, 0, win_w, win_h, (uint32_t)(~0UL));
        xcb_get_image_reply_t *gi_reply = xcb_get_image_reply(connection, gi_cookie, &err);
        if (gi_reply) 
        {
            int data_len = xcb_get_image_data_length(gi_reply);
            static bool show  = false;
            if (!show)
            {
                fprintf(stderr, "data_len = %d\n", data_len);
                fprintf(stderr, "visual = %u\n", gi_reply->visual);
                fprintf(stderr, "depth = %u\n", gi_reply->depth);
                fprintf(stderr, "size = %dx%d\n", win_w, win_h);
                show = true;
            }
            

            

            uint8_t *data = xcb_get_image_data(gi_reply);
                cur_time_ms = std::chrono::steady_clock::now();
                dur = cur_time_ms - pre_time;
                ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
                elapse = static_cast<uint32_t>(ms.count());
                printf("get [frame_count = %u] image ms = %u\n", ++frame_count, elapse);
           fwrite(data, data_len, 1, out_file_ptr);
           fflush(out_file_ptr);
           
            free(gi_reply);
        }
    }



    fclose(out_file_ptr);
       out_file_ptr = NULL;
    return EXIT_SUCCESS;
}











/*
  * 下边是编译命令
  * gcc x11.c -o output -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
  */
 
// #include <X11/Xlib.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
//  #include <unistd.h>
// int main(void)
// {
//     Display *display;
//     Window window;
//     XEvent event;
//     char *msg = "你好 ,这是一个测试窗口!";
//     int s;
//     printf("pid =%u\n", getppid());
//     /* 与Xserver建立连接 */
//     display = XOpenDisplay(NULL);
//     if (display == NULL)
//     {
//         fprintf(stderr, "Cannot open display\n");
//         exit(1);
//     }
 
        
//     s = DefaultScreen(display);
 
//     /* 创建一个窗口 */
//     window = XCreateSimpleWindow(display, RootWindow(display, s), 10, 10, 200, 200, 1,
//                            BlackPixel(display, s), WhitePixel(display, s));
 
//     /* 选择一种感兴趣的事件进行监听 */
//     XSelectInput(display, window, ExposureMask | KeyPressMask);
 
//     /* 显示窗口 */
//     XMapWindow(display, window);
 
//     /* 事件遍历 */
//     for (;;)
//     {
//         XNextEvent(display, &event);
 
//         /* 绘制窗口或者重新绘制 */
//         if (event.type == Expose)
//         {
//             XFillRectangle(display, window, DefaultGC(display, s), 20, 20, 10, 10);
//             XDrawString(display, window, DefaultGC(display, s), 50, 50, msg, strlen(msg));
//         }
//         /* 当检测到键盘按键,退出消息循环 */
//         if (event.type == KeyPress)
//             break;
//     }
 
//     /* 关闭与Xserver服务器的连接 */
//     XCloseDisplay(display);
 
//     return 0;
//  }