/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
************************************************************************************************/

#include "cinput_device.h"
//#define _GNU_SOURCE
#include <sys/types.h>
//#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
//#include <sys/syscall.h>
#include <sys/time.h>
//#include <opencl-c.h>
//#include <GL/glu.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
//#include <Xlib.h>
#include <X11/Xlib.h>
//#include <gtk/gtk.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
//#include <GL/wglew.h>
//#include <GL/glew.h>
//typedef void	(*t_glFlush)(void);
//FILE  * out_log_file_ptr = fopen("./log/hook.log", "wb+");
//#include <>


//#include <xcb/xproto.h>
#define gettid() syscall(__NR_gettid)
//#include "cprotocol.h"
#include "cinput_device_event.h"
#include "rtc_base/logging.h"
#include "clog.h"
//#include "cinput_device_hook.h"
//#include "input_helper.h"
#include "ccfg.h"
#if defined(_MSC_VER)
#include <Windows.h>
#include "cwindow_util.h"
#endif // WIN

#if defined(__unix__)

//#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <xcb/xproto.h>
#include <list>
#include <X11/Xlib.h>
#include <unordered_map>

typedef std::mutex                    clock_type;
typedef std::lock_guard<std::mutex>  clock_guard;
static std::list<XEvent >               g_xevent;
static  clock_type                      g_xevent_lock;

static Display  *                              g_display_ptr = NULL;
static Window                           g_main_window = 0;
static unsigned  long                   g_count = 0;
static  uint32_t g_new_seria = 0;
static  uint32_t g_core_seria = 0;

static std::unordered_map<uint32_t, uint32_t> g_unix_key_table = {
 /*ESC*/	                                   { 27   , 9  },
 /*F1*/                                        { 112  , 67 },
 /*F2*/                                        { 113  , 68 },
 /*F3*/                                        { 114  , 69 },
 /*F4*/                                        { 115  , 70 },
 /*F5*/                                        { 116  , 71 },
 /*F6*/                                        { 117  , 72 },
 /*F7*/                                        { 118  , 73 },
 /*F8*/                                        { 119  , 74 },
 /*F9*/                                        { 120  , 75 },
 /*F10*/                                       { 121  , 76 },
 /*F11*/                                       { 122  , 95 },
 /*F12*/                                       { 123  , 96 },
 ///*F8*/                                        { 124  , 79 },
 /*Print Screen  SysRq*/                       {  44  ,  0 }, // 0 未知数字
 /*Scrool Look*/                               { 145  , 78 },
 /*Pause Break */                              {  19  , 127},
 /* ~ ·*/                                      { 192  , 49 },
 /* 1 */                                       {  49  , 10 },
 /* 2  */                                      {  50  , 11 },
 /* 3  */                                      {  51  , 12   },
 /* 4 */                                       {  52  , 13   },
 /* 5  */                                      {  53  , 14   },
 /* 6  */                                      {  54  , 15  },
 /* 7  */                                      {  55  , 16   },
 /* 8  */                                      {  56  , 17   },
 /* 9  */                                      {  57  , 18   },
 /* 0  */                                      {  48  , 19   },
 /* -  */                                      {  189 , 20   },
 /* +  */                                      {  187 , 21   },
 /* Backspace  */                              {  8   , 22   },

 /* Insert  */                                 {  45  , 118   },
 /* Home  */                                   {  36  , 110  },
 /* Page Up  */                                {  33  , 112   },
 
 /* Num Lock  */                               {   144,  77  },
 /* /  */                              		   {   111,  106  },
 /* *  */                                      {   106,  63 },
 /* -  */                                      {   109,  82  },
 
///////////////////////////////////////////////////////////////////////

 /* Tab  */                                    {   9   ,  23 },
 /* Q  */                                      {   81  ,  24 },
 /* W  */                                      {   87  ,  25 },
 /* E  */                                      {   69  ,  26 },
 /* R  */                                      {   82  ,  27 },
 /* T  */                                      {   84  ,  28 },
 /* Y  */                                      {   89  ,  29 },
 /* U  */                                      {   85  ,  30 },
 /* I  */                                      {   73  ,  31 },
 /* O  */                                      {   79  ,  32 },
 /* P  */                                      {   80  ,  33 },
 /* {  */                                      {  219  ,  34 },
 /* }  */                                      {  221  ,  35 },
 /* Enter  */                                  {   13  ,  36 },
 
 
 /* Delete  */                                 {   46  ,  119 },
 /* End  */                                    {   35  ,  115 },
 /* Pace Down  */                              {   34  ,  117 },
 
 /* 7  */                                      {   103 ,   79},
 /* 8  */                                      {   104 ,   80},
 /* 9  */                                      {   105 ,   81},
 /* +  */                                      {   107 ,   86},
 
 /////////////////////////////////////////////////////////////////////
 
 /* Caps Lock  */                            {   20  , 66},
 /* A  */                                    {   65  , 38},
 /* S  */                                    {   83  , 39},
 /* D  */                                    {   68  , 40},
 /* F  */                                    {   70  , 41},
 /* G  */                                    {   71  , 42},
 /* H  */                                    {   72  , 43},
 /* J  */                                    {   74  , 44},
 /* K  */                                    {   75  , 45},
 /* L  */                                    {   76  , 46},
 /* ;  */                                    {  186  , 47},
 /* '  */                                    {  222  , 48},
 /* \  */                                    {  220  , 51},
 
 
 /* 4  */                                    {  100  , 83},
 /* 5  */                                    {  101  , 84},
 /* 6  */                                    {  102  , 85},
 
 
 ////////////////////////////////////////////////////////////////////////
 
 /* Shift  */                            	 {   16  , 50},
 /* Z */                                     {   90  , 52},
 /* X  */                                    {   88  , 53},
 /* C  */                                    {   67  , 54},
 /* V  */                                    {   86  , 55},
 /* B  */                                    {   66  , 56},
 /* N  */                                    {   78  , 57},
 /* M  */                                    {   77  , 58},
 /* <  */                                    {  188  , 59},
 /* >  */                                    {  190  , 60},
 /* /  */                                    {  191  , 61},
 ///* Shift   */                               {   16  , 62 },
 
 /* 向上→  */                                {   38  , 111},
 
 
 /* 1  */                                    {   97  , 87},
 /* 2  */                                    {   98  , 88},
 /* 3  */                                    {   99  , 89},
 ///* Enter  */                                {   13  , 104},
 
 
 ////////////////////////////////////////////////////////////////////
 /* Ctrl  */                                 {   17  , 37/*105*/ },
// /* win  */                                  {   91  , },
 /* Alt  */                                  {   18  , 64/*108*/},
 /* 空格键  */                               {   32  , 65},
// /* Fn  */                                 {   20  , },
 /* ContextMenu  */                          {   93  , 135},
 /* 左向→  */                                {   37  , 113},
 /* 向下→  */                                {   40  , 116},
 /* 向右→  */                                {   39  , 114},
 /* 0  */                                    {   96  ,  90},
 /* .  */                                    {   190  , 91}, 
 
};



/*
state --> 


#define ShiftMask		(1<<0)
#define LockMask		(1<<1)
#define ControlMask		(1<<2)
#define Mod1Mask		(1<<3)
#define Mod2Mask		(1<<4)
#define Mod3Mask		(1<<5)
#define Mod4Mask		(1<<6)
#define Mod5Mask		(1<<7)


#define Button1Mask		(1<<8)
#define Button2Mask		(1<<9)
#define Button3Mask		(1<<10)
#define Button4Mask		(1<<11)
#define Button5Mask		(1<<12)

*/

//#define gettid() syscall(__NR_gettid)

typedef  int (*Hook_XNextEvent)( Display*	d 	/* display */, XEvent*		e/* event_return */ )   ;
static  Hook_XNextEvent   real_XNextEvent = NULL;
typedef int (*Hook_XSelectInput)(  Display*	display	/* display */, Window	w	/* w */, long		event_mask/* event_mask */ )  ;
static Hook_XSelectInput real_XSelectInput = NULL;
typedef   int  (*Hook_XPending)(  Display*	display	/* display */ )  ;
static Hook_XPending  real_XPending = NULL;

typedef int (*Hook_select) (int __nfds, fd_set *__restrict __readfds,
                    fd_set *__restrict __writefds,
                    fd_set *__restrict __exceptfds,
                    struct timeval *__restrict __timeout);
static Hook_select real_select = NULL;



//int select(int __nfds, fd_set *__restrict __readfds,
//fd_set *__restrict __writefds,
//        fd_set *__restrict __exceptfds,
//struct timeval *__restrict __timeout)
//{
//    if (!real_select)
//    {
//        real_select = reinterpret_cast<Hook_select>(dlsym(RTLD_NEXT, "select"));
//    }
////    poll
//    if (g_display_ptr)
//    {
//        using namespace chen;
//        ERROR_EX_LOG("select fd = %s, display fd = %u", __nfds, ConnectionNumber(g_display_ptr));
//    }
//    return real_select(__nfds, __readfds, __writefds, __exceptfds, __timeout);
//}
//ConnectionNumber











void show(const char * str, size_t len)
{
//    XCheckIfEvent
//    char buffer[10240] = {0};
//    (void) sprintf(buffer, "real_XNextEvent %s [time_now :%d-%d-%d %d:%d:%d.%ld]\n", str, 1900 + t->tm_year,
//                   1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec);
    using namespace chen;
    ERROR_EX_LOG("=================[%s]", std::string(str, len).c_str());
}
void show_hook_info(Display *display, const XEvent* e)
{
    char buffer[102400] = {0};

    switch (e->type)
    {
        case KeyPress:
        {
//            typedef struct {
//                int type;		/* of event */
//                unsigned long serial;	/* # of last request processed by server */
//                Bool send_event;	/* true if this came from a SendEvent request */
//                Display *display;	/* Display the event was read from */
//                Window window;	        /* "event" window it is reported relative to */
//                Window root;	        /* root window that the event occurred on */
//                Window subwindow;	/* child window */
//                Time time;		/* milliseconds */
//                int x, y;		/* pointer x, y coordinates in event window */
//                int x_root, y_root;	/* coordinates relative to root */
//                unsigned int state;	/* key or button mask */
//                unsigned int keycode;	/* detail */
//                Bool same_screen;	/* same screen flag */
//            } XKeyEvent;
//            g_main_window = e->xkey.window;
            int len = sprintf(buffer, "[KeyPress  type = %u][display = %p][serial = %u][send_event = %d][display = %p][window = %p][root = %u][subwindow = %p][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][keycode = %u][same_screen = = %d]\n",
                              e->xkey.type, display, e->xkey.serial, e->xkey.send_event, e->xkey.display, e->xkey.window, e->xkey.root, e->xkey.subwindow, e->xkey.x, e->xkey.y, e->xkey.x_root, e->xkey.y_root, e->xkey.state, e->xkey.keycode, e->xkey.same_screen);
            show(buffer, len);
            break;
        }
        case KeyRelease:
        {
            int len =  sprintf(buffer, "[KeyRelease  type = %u][display = %p][serial = %u][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][keycode = %u][same_screen = = %d]\n",
                               e->xkey.type, display, e->xkey.serial, e->xkey.send_event, e->xkey.display, e->xkey.window, e->xkey.root, e->xkey.subwindow, e->xkey.x, e->xkey.y, e->xkey.x_root, e->xkey.y_root, e->xkey.state, e->xkey.keycode, e->xkey.same_screen);
            show(buffer, len);
            break;
        }
        case ButtonPress:
        {
            //
//            typedef struct {
//                int type;		/* of event */
//                unsigned long serial;	/* # of last request processed by server */
//                Bool send_event;	/* true if this came from a SendEvent request */
//                Display *display;	/* Display the event was read from */
//                Window window;	        /* "event" window it is reported relative to */
//                Window root;	        /* root window that the event occurred on */
//                Window subwindow;	/* child window */
//                Time time;		/* milliseconds */
//                int x, y;		/* pointer x, y coordinates in event window */
//                int x_root, y_root;	/* coordinates relative to root */
//                unsigned int state;	/* key or button mask */
//                unsigned int button;	/* detail */
//                Bool same_screen;	/* same screen flag */
//            } XButtonEvent;

            int len =  sprintf(buffer, "[ButtonPress type = %u][display = %p][serial = %llu][send_event = %d][display = %p][window = %u][root = %u][time][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][button = %u][same_screen = %d]",
                               e->xbutton.type, display, e->xbutton.serial, e->xbutton.send_event, e->xbutton.display, e->xbutton.window, e->xbutton.root, e->xbutton.x, e->xbutton.y, e->xbutton.x_root, e->xbutton.y_root, e->xbutton.state, e->xbutton.button, e->xbutton.same_screen);
            show(buffer, len);
            break;
        }
        case ButtonRelease:
        {
            int len =  sprintf(buffer, "[ButtonRelease type = %u][display = %p][serial = %llu][send_event = %d][display = %p][window = %u][root = %u][time][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][button = %u][same_screen = %d]",
                               e->xbutton.type, display, e->xbutton.serial, e->xbutton.send_event, e->xbutton.display, e->xbutton.window, e->xbutton.root, e->xbutton.x, e->xbutton.y, e->xbutton.x_root, e->xbutton.y_root, e->xbutton.state, e->xbutton.button, e->xbutton.same_screen);
            show(buffer, len);
            break;
        }
        case MotionNotify:
        {

//            typedef struct {
//                int type;		/* of event */
//                unsigned long serial;	/* # of last request processed by server */
//                Bool send_event;	/* true if this came from a SendEvent request */
//                Display *display;	/* Display the event was read from */
//                Window window;	        /* "event" window reported relative to */
//                Window root;	        /* root window that the event occurred on */
//                Window subwindow;	/* child window */
//                Time time;		/* milliseconds */
//                int x, y;		/* pointer x, y coordinates in event window */
//                int x_root, y_root;	/* coordinates relative to root */
//                unsigned int state;	/* key or button mask */
//                char is_hint;		/* detail */
//                Bool same_screen;	/* same screen flag */
//            } XMotionEvent;
//            g_main_window = e->xcrossing.window;
//            e->xcrossing.window = g_main_window;
            int len =  sprintf(buffer, "[MotionNotify type = %u][ display = %p][serial = %u][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][is_hint = %c][same_screen = %d]\n",
                               e->xmotion.type, display, e->xmotion.serial, e->xmotion.send_event, e->xmotion.display, e->xmotion.window, e->xmotion.root, e->xmotion.subwindow, e->xmotion.x, e->xmotion.y, e->xmotion.x_root, e->xmotion.y_root, e->xmotion.state, e->xmotion.is_hint, e->xmotion.same_screen);

            show(buffer, len);
            break;
        }
        case EnterNotify:
        {
//            typedef struct {
//                int type;		/* of event */
//                unsigned long serial;	/* # of last request processed by server */
//                Bool send_event;	/* true if this came from a SendEvent request */
//                Display *display;	/* Display the event was read from */
//                Window window;	        /* "event" window reported relative to */
//                Window root;	        /* root window that the event occurred on */
//                Window subwindow;	/* child window */
//                Time time;		/* milliseconds */
//                int x, y;		/* pointer x, y coordinates in event window */
//                int x_root, y_root;	/* coordinates relative to root */
//                int mode;		/* NotifyNormal, NotifyGrab, NotifyUngrab */
//                int detail;
//                /*
//                 * NotifyAncestor, NotifyVirtual, NotifyInferior,
//                 * NotifyNonlinear,NotifyNonlinearVirtual
//                 */
//                Bool same_screen;	/* same screen flag */
//                Bool focus;		/* boolean focus */
//                unsigned int state;	/* key or button mask */
//            } XCrossingEvent;
//            e->xcrossing.window = 0u;
            int len =  sprintf(buffer, "[EnterNotify   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u]"
                                       "[x = %u, y = %u][x_root = %u, y_root = %u][mode = %u][detail = %u][same_screen = %d][focus = %d][state = %u]",
                               EnterNotify, e->xcrossing.type, e->xcrossing.serial, e->xcrossing.send_event, e->xcrossing.display, e->xcrossing.window,
                               e->xcrossing.root, e->xcrossing.subwindow, e->xcrossing.x, e->xcrossing.y, e->xcrossing.x_root, e->xcrossing.y_root, e->xcrossing.mode, e->xcrossing.detail,
                               e->xcrossing.same_screen, e->xcrossing.focus, e->xcrossing.state);
            show(buffer, len);
            break;
        }
        case LeaveNotify:
        {
            int len =  sprintf(buffer, "[EnterNotify   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u]"
                                       "[x = %u, y = %u][x_root = %u, y_root = %u][mode = %u][detail = %u][same_screen = %d][focus = %d][state = %u]",
                               EnterNotify, e->xcrossing.type, e->xcrossing.serial, e->xcrossing.send_event, e->xcrossing.display, e->xcrossing.window,
                               e->xcrossing.root, e->xcrossing.subwindow, e->xcrossing.x, e->xcrossing.y, e->xcrossing.x_root, e->xcrossing.y_root, e->xcrossing.mode, e->xcrossing.detail,
                               e->xcrossing.same_screen, e->xcrossing.focus, e->xcrossing.state);
            show(buffer, len);
            break;
        }
        case FocusIn:
        {
//            typedef struct {
//                int type;		/* FocusIn or FocusOut */
//                unsigned long serial;	/* # of last request processed by server */
//                Bool send_event;	/* true if this came from a SendEvent request */
//                Display *display;	/* Display the event was read from */
//                Window window;		/* window of event */
//                int mode;		/* NotifyNormal, NotifyWhileGrabbed,
//				   NotifyGrab, NotifyUngrab */
//                int detail;
//                /*
//                 * NotifyAncestor, NotifyVirtual, NotifyInferior,
//                 * NotifyNonlinear,NotifyNonlinearVirtual, NotifyPointer,
//                 * NotifyPointerRoot, NotifyDetailNone
//                 */
//            } XFocusChangeEvent;
            int len =  sprintf(buffer, "[FocusIn   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][mode = %u][detail = %u]",
                               FocusIn, e->xfocus.type, e->xfocus.serial, e->xfocus.send_event, e->xfocus.display, e->xfocus.window, e->xfocus.mode, e->xfocus.detail);
            show(buffer, len);
            break;
        }
        case FocusOut:
        {
            int len =  sprintf(buffer, "[FocusIn   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][mode = %u][detail = %u]",
                               FocusIn, e->xfocus.type, e->xfocus.serial, e->xfocus.send_event, e->xfocus.display, e->xfocus.window, e->xfocus.mode, e->xfocus.detail);
            show(buffer, len);
            break;
        }
        case KeymapNotify:
        {
            int len =  sprintf(buffer, "[KeymapNotify   = %d]", KeymapNotify);
            show(buffer, len);
            break;
        }
        case Expose:
        {
            int len =  sprintf(buffer, "Expose   = %d", Expose);
            show(buffer, len);
            break;
        }
        case GraphicsExpose:
        {
            int len =  sprintf(buffer, "GraphicsExpose   = %d", GraphicsExpose);
            show(buffer, len);
            break;
        }
        case NoExpose:
        {
            int len =  sprintf(buffer, "NoExpose   = %d", NoExpose);
            show(buffer, len);
            break;
        }
        case VisibilityNotify:
        {
            int len =  sprintf(buffer, "VisibilityNotify   = %d", VisibilityNotify);
            show(buffer, len);
            break;
        }
        case CreateNotify:
        {
            int len =  sprintf(buffer, "CreateNotify   = %d", CreateNotify);
            show(buffer, len);
            break;
        }
        case DestroyNotify:
        {
            int len =  sprintf(buffer, "DestroyNotify   = %d", DestroyNotify);
            show(buffer, len);
            break;
        }
        case UnmapNotify:
        {
            int len =  sprintf(buffer, "UnmapNotify   = %d", UnmapNotify);
            show(buffer, len);
            break;
        }
        case MapNotify:
        {
            int len =  sprintf(buffer, "MapNotify   = %d", MapNotify);
            show(buffer, len);
            break;
        }
        case MapRequest:
        {
            int len =  sprintf(buffer, "MapRequest   = %d", MapRequest);
            show(buffer, len);
            break;
        }
        case ReparentNotify:
        {
            int len =  sprintf(buffer, "ReparentNotify   = %d", ReparentNotify);
            show(buffer, len);
            break;
        }
        case ConfigureNotify:
        {
            int len =  sprintf(buffer, "ConfigureNotify   = %d", ConfigureNotify);
            show(buffer, len);
            break;
        }
        case ConfigureRequest:
        {
            int len =  sprintf(buffer, "ConfigureRequest   = %d", ConfigureRequest);
            show(buffer, len);
            break;
        }
        case GravityNotify:
        {
            int len =  sprintf(buffer, "GravityNotify   = %d", GravityNotify);
            show(buffer, len);
            break;
        }
        case ResizeRequest:
        {
            int len =  sprintf(buffer, "ResizeRequest   = %d", ResizeRequest);
            show(buffer, len);
            break;
        }
        case CirculateNotify:
        {
            int len =  sprintf(buffer, "CirculateNotify   = %d", CirculateNotify);
            show(buffer, len);
            break;
        }
        case CirculateRequest:
        {
            int len =  sprintf(buffer, "CirculateRequest   = %d", CirculateRequest);
            show(buffer, len);
            break;
        }
        case PropertyNotify:
        {
//            typedef struct {
//                int type;
//                unsigned long serial;	/* # of last request processed by server */
//                Bool send_event;	/* true if this came from a SendEvent request */
//                Display *display;	/* Display the event was read from */
//                Window window;
//                Atom atom;
//                Time time;
//                int state;		/* NewValue, Deleted */
//            } XPropertyEvent;
            int len =  sprintf(buffer, "[PropertyNotify = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][atom = %d][state = %u]", PropertyNotify, e->xproperty.type, e->xproperty.serial, e->xproperty.send_event, e->xproperty.display, e->xproperty.window, e->xproperty.atom, e->xproperty.state);
            show(buffer, len);
            break;
        }
        case SelectionClear:
        {
            int len =  sprintf(buffer, "SelectionClear   = %d", SelectionClear);
            show(buffer, len);
            break;
        }
        case SelectionRequest:
        {
            int len =  sprintf(buffer, "SelectionRequest   = %d", SelectionRequest);
            show(buffer, len);
            break;
        }
        case SelectionNotify:
        {
            int len =  sprintf(buffer, "SelectionNotify   = %d", SelectionNotify);
            show(buffer, len);
            break;
        }
        case ColormapNotify:
        {
            int len =  sprintf(buffer, "ColormapNotify   = %d", ColormapNotify);
            show(buffer, len);
            break;
        }
        case ClientMessage:
        {
            int len =  sprintf(buffer, "ClientMessage   = %d", ClientMessage);
            show(buffer, len);
            break;
        }
        case MappingNotify:
        {
            int len =  sprintf(buffer, "MappingNotify   = %d", MappingNotify);
            show(buffer, len);
            break;
        }
        case GenericEvent:
        {
            int len =  sprintf(buffer, "GenericEvent   = %d", GenericEvent);
            show(buffer, len);
            break;
        }
        case LASTEvent:
        {
            int len =  sprintf(buffer, "LASTEvent   = %d", LASTEvent);
            show(buffer, len);
            break;
        }
        default:
        {
            char buffer[1024] = {0};
            int len =  sprintf(buffer, "unknow type = %u\n", e->type);
            show(buffer, len);
            break;
        }
    }
}






//int XNextEvent( Display*	d	/* display */, XEvent*		e/* event_return */ )
//{
////    g_display_ptr = d;
//    if (!real_XNextEvent)
//    {
//        real_XNextEvent = reinterpret_cast<Hook_XNextEvent>(dlsym(RTLD_NEXT, "XNextEvent"));
//    }
//
//    int ret = real_XNextEvent(d, e);
//    show_hook_info(d, e);
//
//    return ret;
//}



//int  XSelectInput( Display* display		/* display */, Window	w 	/* w */, long	event_mask	/* event_mask */ )
//{
//    g_display_ptr = display;
//    if (!real_XSelectInput)
//    {
//        real_XSelectInput = reinterpret_cast<Hook_XSelectInput>(dlsym(RTLD_NEXT, "XSelectInput"));
//    }
//
////    g_main_window= w;
//    int size  = real_XSelectInput(display, w, event_mask);
//    using namespace  chen;
//    NORMAL_EX_LOG( "[real_XSelectInput][display = %p][window = %u][event_mask = %lu][ret = %u]\n", display, w, event_mask, size);
////    char buffer[102400] = {0};
////    (void)sprintf(buffer, "[real_XSelectInput][display = %p][window = %u][event_mask = %lu][ret = %u]\n", display, w, event_mask, size);
////    (void)write(1, buffer, strlen(buffer));
//    return size;
//}

//
//int XPending( Display*	display	/* display */ )
//{
//    g_display_ptr = display;
//    if (!real_XPending)
//    {
//        real_XPending = reinterpret_cast < Hook_XPending>(dlsym(RTLD_NEXT, "XPending"));
//    }
////    XPending
//    int size = real_XPending(display ) ;
////
////    {
////        clock_guard  lock(g_xevent_lock);
////        size += g_xevent.size();
////    }
////    char buffer[102400] = {0};
////
////    (void)sprintf(buffer, "[real_XPending][display = %p][size = %u] \n", display, size);
////    (void)write(1, buffer, strlen(buffer));
//
//    using namespace  chen;
////    NORMAL_EX_LOG("[real_XPending][display = %p][size = %u][g_count = %lu] \n", display, size, g_count++);
//
//    return size;
//}
#endif


namespace chen {
	int32_t  g_width = 150;
	int32_t  g_height = 150;
	using FKeyCodeType = uint8;
    	#ifdef _MSC_VER
	static HWND g_main_mouse_down_up = NULL;
	static HWND g_child_mouse_down_up = NULL;
    using FCharacterType = TCHAR;
#elif defined(__GNUC__) ||defined(__APPLE__)
	static void * g_main_mouse_down_up = NULL;
	static void * g_child_mouse_down_up = NULL;
    using FCharacterType = TCHAR;
#else
// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
    
	using FRepeatType = uint8;
	using FButtonType = uint8;
	using FPosType = uint16;
	using FDeltaType = int16;
	//using FTouchesType = TArray<FTouch>;
	using FControllerIndex = uint8;
	using FControllerButtonIndex = uint8;
	using FControllerAnalog = double;
	using FControllerAxis = uint8;


	//static long long timeA = systemtime();
	//std::chrono::milliseconds
	//cout << timeA << endl;

#if defined(_MSC_VER)
	////PostMessage
#define MESSAGE(g_wnd, message_id, param1, param2) MSG msg;  \
														  msg.hwnd = g_wnd;			\
												          msg.message = message_id;	\
														  msg.wParam = param1;		\
														  msg.lParam = param2;		\
			std::chrono::steady_clock::time_point cur_time_ms = std::chrono::steady_clock::now();     \
											long long ms = static_cast<long long >(cur_time_ms.time_since_epoch().count() / 1000000); \
												 msg.time = static_cast<DWORD>(ms);	\
														  msg.pt.x = g_width; msg.pt.y = g_height; \
														::TranslateMessage(&msg);         \
													long long ret_dispatch =	 ::DispatchMessage(&msg); 
													//NORMAL_EX_LOG("move cur_ms = %u, [ret_dispatch = %s]", ms, std::to_string(ret_dispatch).c_str());


//#define MESSAGE(g_wnd, message_id, param1, param2) PostMessage(g_wnd, message_id, param1, param2);
	//使用全局变量操作的哈 
#define SET_POINT() POINT pt; pt.x = g_width; pt.y = g_height;


#define WINDOW_MAIN()		HWND mwin = FindMainWindow()
#define WINDOW_CHILD()	HWND childwin = MainChildPoint(mwin, pt)
#define WINDOW_BNTTON_DOWN(v)  uint32 active_type = WM_LBUTTONDOWN;					 \
	switch (vec.button)																 \
	{                                                                             	 \
		case EMouseButtons::Left:													 \
		{																			 \
			active_type = WM_LBUTTONDOWN;											 \
			break;																	 \
		}																			 \
		case EMouseButtons::Middle:													 \
		{																			 \
			active_type = WM_MBUTTONDOWN;											 \
																					 \
			break;																	 \
		}																			 \
		case EMouseButtons::Right:													 \
		{																			 \
			active_type = WM_RBUTTONDOWN;											 \
			break;																	 \
		}																			 \
		default:																	 \
							 														 \
			break;																	 \
	}

#define WINDOW_BNTTON_UP(v)  uint32 active_type = WM_LBUTTONUP; switch (vec.button)   \
	{                                                                             	 \
		case EMouseButtons::Left:													 \
		{																			 \
			active_type = WM_LBUTTONUP;											 \
			break;																	 \
		}																			 \
		case EMouseButtons::Middle:													 \
		{																			 \
			active_type = WM_MBUTTONUP;											 \
																					 \
			break;																	 \
		}																			 \
		case EMouseButtons::Right:													 \
		{																			 \
			active_type = WM_RBUTTONUP;											 \
			break;																	 \
		}																			 \
		default:																	 \
							 														 \
			break;																	 \
	}
#else

#endif //#if defined(_MSC_VER)
#define REGISTER_INPUT_DEVICE(type, handler_ptr) if (false == m_input_device.insert(std::make_pair(type, handler_ptr)).second){	 ERROR_EX_LOG("[type = %s][handler_ptr = %s]", #type, #handler_ptr);	return false;}

#define REGISTER_KEYDOWN(key, value) m_keydown_map.insert(std::make_pair(key, value));
    static bool hook_input_device_logger(unsigned int level, const char *format, ...)
    {
//        bool status = false;
//	char buffer[1024 * 3] = {0};
//
//        va_list args;
//        switch (level) {
//            case LOG_LEVEL_INFO:
//            {
//                va_start(args, format);
//
//                status =  vsnprintf(buffer, 1024 * 3, format, args) >= 0;
//                va_end(args);
//                break;
//            }
//
//            case LOG_LEVEL_WARN:
//            {
//                va_start(args, format);
//                status =  vsnprintf(buffer, 1024 * 3, format, args) >= 0;
//                va_end(args);
//                break;
//            }
//            case LOG_LEVEL_ERROR:
//            {
//                va_start(args, format);
//                status =  vsnprintf(buffer, 1024 * 3, format, args) >= 0;
//                va_end(args);
//                break;
//            }
//            default:
//            {//ERROR_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)
////                WARNING_EX_LOG(f);
//                break;
//            }
//        }

//        NORMAL_EX_LOG("input device [buffer = %s]", buffer);


        return true;
    }




    void mouse_move(int x_cd,int y_cd)
    {
//        xcb_generic_event_t
//        xcb_button_press_event_t
//        xcb_button_press_event_t
//        XCB_GE_GENERIC
//        xcb_generic_event_t
//        XCB_EVENT_MASK_BUTTON_RELEASE
//        Display *display = XOpenDisplay(NULL);
        Window fromroot, tmpwin, root;
        int x, y, tmp;
        unsigned int utmp;

        root = DefaultRootWindow(g_display_ptr);
//        xcb_send_event
        XQueryPointer(g_display_ptr, root, &fromroot, &tmpwin, &x, &y, &tmp, &tmp, &utmp);// receive the mouse position
        if(g_display_ptr == NULL)
        {
            fprintf(stderr, "Errore nell'apertura del Display !!!\n");
            exit(EXIT_FAILURE);
        }
        XWarpPointer(g_display_ptr, None, root, 0, 0, 0, 0, x_cd, y_cd); //move point to the position
        XFlush(g_display_ptr);
        XCloseDisplay(g_display_ptr);
    }
    void press_left_button()
    {
//        Display *display = XOpenDisplay(NULL);
        XEvent event;
        memset(&event,0,sizeof(XEvent));
        event.type = ButtonPress;
        event.xbutton.button = Button1;
        event.xbutton.same_screen = True;


/* get info about current pointer position */
        XQueryPointer(g_display_ptr, RootWindow(g_display_ptr, DefaultScreen(g_display_ptr)),
                      &event.xbutton.root, &event.xbutton.window,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);


        event.xbutton.subwindow = event.xbutton.window;


/* walk down through window hierachy to find youngest child */
        while (event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;
            XQueryPointer(g_display_ptr, event.xbutton.window,\
&event.xbutton.root, &event.xbutton.subwindow,\
&event.xbutton.x_root, &event.xbutton.y_root,\
&event.xbutton.x, &event.xbutton.y,\
&event.xbutton.state);
        }
/* send ButtonPress event to youngest child */
        if (XSendEvent(g_display_ptr, PointerWindow, True, 0xfff, &event) == 0)
            printf("XSendEvent Failed\n");

        XFlush(g_display_ptr);
        XCloseDisplay(g_display_ptr);
    }

    void press_right_button()
    {
//        Display *display = XOpenDisplay(NULL);
        XEvent event;
        memset(&event,0,sizeof(XEvent));
        event.type = ButtonPress;
        event.xbutton.button = Button2;
        event.xbutton.same_screen = True;


/* get info about current pointer position */
        XQueryPointer(g_display_ptr, RootWindow(g_display_ptr, DefaultScreen(g_display_ptr)),
                      &event.xbutton.root, &event.xbutton.window,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);


        event.xbutton.subwindow = event.xbutton.window;


/* walk down through window hierachy to find youngest child */
        while (event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;
            XQueryPointer(g_display_ptr, event.xbutton.window,\
&event.xbutton.root, &event.xbutton.subwindow,\
&event.xbutton.x_root, &event.xbutton.y_root,\
&event.xbutton.x, &event.xbutton.y,\
&event.xbutton.state);
        }
/* send ButtonPress event to youngest child */
        if (XSendEvent(g_display_ptr, PointerWindow, True, 0xfff, &event) == 0)
            printf("XSendEvent Failed\n");

        XFlush(g_display_ptr);

/* sleep for a little while */
        usleep(10000);


/* set up ButtonRelease event */
        event.type = ButtonRelease;
        event.xbutton.state = 0x100;


/* send ButtonRelease event to youngest child */
        if (XSendEvent(g_display_ptr, PointerWindow, True, 0xfff, &event) == 0)
            printf("XSendEvent Failed\n");

        XFlush(g_display_ptr);
        XCloseDisplay(g_display_ptr);

    }
    void release_button(int Bflags)//判断释放的是鼠标的左键还是右键
    {
//        Display *display = XOpenDisplay(NULL);
        XEvent event;
        memset(&event,0,sizeof(XEvent));
        switch(Bflags){
            case 1:event.xbutton.button = Button1;break;
            case 2:event.xbutton.button = Button2;break;
        }
        event.type = ButtonRelease;
        event.xbutton.state = 0x100;


/* get info about current pointer position */
        XQueryPointer(g_display_ptr, RootWindow(g_display_ptr, DefaultScreen(g_display_ptr)),
                      &event.xbutton.root, &event.xbutton.window,
                      &event.xbutton.x_root, &event.xbutton.y_root,
                      &event.xbutton.x, &event.xbutton.y,
                      &event.xbutton.state);


        event.xbutton.subwindow = event.xbutton.window;


/* walk down through window hierachy to find youngest child */
        while (event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;
            XQueryPointer(g_display_ptr, event.xbutton.window,\
            &event.xbutton.root, &event.xbutton.subwindow,\
            &event.xbutton.x_root, &event.xbutton.y_root,\
            &event.xbutton.x, &event.xbutton.y,\
            &event.xbutton.state);
                    }
/* send ButtonPress event to youngest child */
        if (XSendEvent(g_display_ptr, PointerWindow, True, 0xfff, &event) == 0)
            printf("XSendEvent Failed\n");

        XFlush(g_display_ptr);
        XCloseDisplay(g_display_ptr);
    }


    void set_global_display(Display* display_ptr)
    {
        g_display_ptr = display_ptr;
    }


static uint32 g_key_state = 0;


///	cinput_device   g_input_device_mgr;
	cinput_device::cinput_device() 
		:  m_input_device()
		,  m_int_point(){}
	cinput_device::~cinput_device() {}

 
	//static bool g_move_init = false;
	bool cinput_device::init()
	{
        SYSTEM_LOG(" input device hook set log  ...");
//        hook_set_logger_proc(hook_input_device_logger);

        SYSTEM_LOG(" input device register mouse key ...");
		REGISTER_INPUT_DEVICE(RequestQualityControl, &cinput_device::OnKeyChar);
		REGISTER_INPUT_DEVICE(KeyDown, &cinput_device::OnKeyDown);
		REGISTER_INPUT_DEVICE(KeyUp, &cinput_device::OnKeyUp);
//		REGISTER_INPUT_DEVICE(KeyPress, &cinput_device::OnKeyPress);
		////////////////////////////////////////////////////////////
		REGISTER_INPUT_DEVICE(MouseEnter, &cinput_device::OnMouseEnter);
		REGISTER_INPUT_DEVICE(MouseLeave, &cinput_device::OnMouseLeave);
		REGISTER_INPUT_DEVICE(MouseDown, &cinput_device::OnMouseDown);
		REGISTER_INPUT_DEVICE(MouseUp, &cinput_device::OnMouseUp);
		REGISTER_INPUT_DEVICE(MouseMove, &cinput_device::OnMouseMove);
		REGISTER_INPUT_DEVICE(MouseWheel, &cinput_device::OnMouseWheel);
		REGISTER_INPUT_DEVICE(MouseDoubleClick, &cinput_device::OnMouseDoubleClick);
        SYSTEM_LOG("  input device init ok !!!");
		return true;
	}
	void cinput_device::Destroy()
	{
		m_input_device.clear();
        SYSTEM_LOG("input device register mouse destroy ok !!!");
		m_all_consumer.clear();
//        unload_input_device();
        SYSTEM_LOG("input device unload destroy ok !!!");

	}
	bool cinput_device::set_point(uint32 x, uint32 y)
	{
		m_int_point.X = x;
		m_int_point.Y = y;
		return true;
	}
    bool cinput_device::set_main_window(Window window)
    {
        g_main_window = window;
        return true;
    }
	bool cinput_device::OnMessage(const std::string & consumer_id, const webrtc::DataBuffer& Buffer)
	{
		//NORMAL_LOG("consumer_id = %s", consumer_id.c_str());
		m_mouse_id = consumer_id;
		const uint8* Data = Buffer.data.data();
		uint32 Size = static_cast<uint32>(Buffer.data.size());

		GET(uint8, MsgType);
		
		M_INPUT_DEVICE_MAP::iterator iter =  m_input_device.find(MsgType);
		if (iter == m_input_device.end())
		{
			//RTC_LOG(LS_ERROR) << "input_device not find id = " << MsgType;
			//log --->  not find type 
			ERROR_EX_LOG("input_device msg_type = %u not find failed !!!", MsgType);
			return false;
		}
		
		/*if (MsgType != MouseEnter)
		{
			std::map<std::string, std::map<uint32, cmouse_info>>::const_iterator consumer_iter = m_all_consumer.find(consumer_id);
			if (consumer_iter == m_all_consumer.end())
			{
				WARNING_EX_LOG("consumer_id = %s not Enter !!!", consumer_id.c_str());
				return false;
			}

		} */
		//NORMAL_EX_LOG("move type =%d", MsgType);
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds microseconds;
		uint32_t elapse = 0;
		 (this->*(iter->second))(Data, Size);
		 cur_time_ms = std::chrono::steady_clock::now();
		 dur = cur_time_ms - pre_time;
		 microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dur);
		 elapse = static_cast<uint32_t>(microseconds.count());
		 if (elapse > 900)
		 {
			 WARNING_EX_LOG("input_device  microseconds = %lu", microseconds);
		 }
		 return true;
		//return true;
	}


	bool cinput_device::OnRequestQualityControl(const uint8*& Data, uint32 size)
	{
		// 这边需要启动引擎推送视频流
		return true;
	}
	/**
	* 输入字符
	*/
	bool cinput_device::OnKeyChar(const uint8*& Data,   uint32 size)
	{
		NORMAL_LOG("KeyChar");
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_CHAR, Character, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_CHAR, Character, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}

		return true;
	}
	/**
	* 按下键
	*/
	bool cinput_device::OnKeyDown(const uint8*& Data,  uint32 Size)
	{
		 
		GET(FKeyCodeType, KeyCode);
		GET(FRepeatType, Repeat);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key down: %d, repeat: %d"), KeyCode, Repeat);
		// log key down -> repeat keyCode Repeat 
		FEvent KeyDownEvent(EventType::KEY_DOWN);
		KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u", KeyCode, Repeat);
#if defined(_MSC_VER)
		WINDOW_MAIN();
		// TODO@chensong 2022-01-20  keydown -> keycode -> repeat 
		/*if (mwin)
		{
			::PostMessageW(mwin, WM_KEYDOWN, KeyCode, Repeat != 0);
		}*/
		
		
		SET_POINT();
		WINDOW_CHILD();
		if (childwin)
		{
			//PostMessageW
	        /*
			这个 KeyDown与 KeyUP 是有讲究哈 ^_^
			发送1次按键结果出现2次按键的情况
发送一次WM_KEYDOWN及一次WM_KEYUP结果出现了2次按键，原因是最后一个参数lParam不规范导致，此参数0到15位为该键在键盘上的重复次数，经常设为1，即按键1次；16至23位为键盘的扫描码，通过API函数MapVirtualKey可以得到；24位为扩展键，即某些右ALT和CTRL；29一般为0；30位-[原状态]已按下为1否则0（KEYUP要设为1）；31位-[状态切换]（KEYDOWN设为0，KEYUP要设为1）。


资料显示第30位对于keydown在和shift等结合的时候通常要设置为1，未经验证。


			*/
			MESSAGE(childwin, WM_KEYDOWN, KeyCode, 0);
		}
		else if (mwin)
		{
			MESSAGE(mwin, WM_KEYDOWN, KeyCode, 0);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
#elif defined(__unix__)
//        static uiohook_event event  ;
//        event.type = EVENT_KEY_PRESSED;
//        event.mask = 0X200;
//        event.data.keyboard.keychar = CHAR_UNDEFINED;
//        unsigned char  code =  XKeysymToKeycode(helper_disp, KeyCode);
//        unsigned char code = XKeysymToKeycode(helper_disp, XStringToKeysym(KeyCode));
//        NORMAL_EX_LOG("code = %u", code);
//        event.data.keyboard.keycode = code;
//        event.data.keyboard.keychar = KeyCode;// VC_ESCAPE;
//        event.data.keyboard.rawcode = KeyCode;
//        hook_post_event(&event);

	////////////////////////////////////////////////////
	//typedef struct {
    //           int type;       /* KeyPress or KeyRelease */
    //           unsigned long serial;   /* # of last request processed by server */
    //           Bool send_event;        /* true if this came from a SendEvent request */
    //           Display *display;       /* Display the event was read from */
    //           Window window;  /* ``event'' window it is reported relative to */
    //           Window root;    /* root window that the event occurred on */
    //           Window subwindow;       /* child window */
    //           Time time;      /* milliseconds */
    //           int x, y;       /* pointer x, y coordinates in event window */
    //           int x_root, y_root;     /* coordinates relative to root */
    //           unsigned int state;     /* key or button mask */
    //           unsigned int keycode;   /* detail */
    //           Bool same_screen;       /* same screen flag */
    //   } XKeyEvent;

	///////////////////////////////////////////////////
        XEvent  Xkey;
        Xkey.xkey.type = KeyPress;
        Xkey.xkey.send_event = 0;
        Xkey.xkey.same_screen = 1;
		// key 32 ==> 65
        Xkey.xkey.keycode = /*65*/  g_unix_key_table[KeyCode]; //KeyCode;
        Xkey.xkey.state  = g_key_state;
		Xkey.xkey.display = g_display_ptr;
		Xkey.xkey.window = g_main_window;

		/*
		#define ShiftMask		(1<<0)
#define LockMask		(1<<1)
#define ControlMask		(1<<2)
#define Mod1Mask		(1<<3)
		
		*/
		if (16 == KeyCode )
		{
			g_key_state |= ShiftMask;
		}
		else if (20 == KeyCode )
		{
			g_key_state |= LockMask;
		}
		else if (17 == KeyCode)
		{
			g_key_state |= ControlMask;
		}
		else if (18 == KeyCode)
		{
			g_key_state |= Mod1Mask;
		}

		// if ()
        if (g_main_window && g_display_ptr /*&& KeyCode == 32*/)
        {
//            XTestFakeButtonEvent(g_display_ptr, -1, True, CurrentTime);
            XSendEvent(g_display_ptr, g_main_window, True, KeyPress, &Xkey);
//            XSync(g_display_ptr, true);
            XFlush(g_display_ptr);
        }
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		//KeyDownEvent.GetKeyDown();
		//ProcessEvent(KeyDownEvent);
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_KEYDOWN, KeyCode, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_KEYDOWN, KeyCode, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
		return true;
	}

	/**
	* 松开键
	*/
	bool cinput_device::OnKeyUp(const uint8*& Data,   uint32 Size)
	{
		GET(FKeyCodeType, KeyCode);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key up: %d"), KeyCode);
		// log key up -> KeyCode
		FEvent KeyUpEvent(EventType::KEY_UP);
		KeyUpEvent.SetKeyUp(KeyCode);
		NORMAL_LOG("OnKeyUp==KeyCode = %u", KeyCode);
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		
		
		SET_POINT();
		WINDOW_CHILD();
		if (childwin)
		{
			MESSAGE(childwin, WM_KEYUP, KeyCode, 1);
		}
		else if (mwin)
		{
			MESSAGE(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
#elif defined(__unix__)
//        static uiohook_event event;
//        event.type = EVENT_KEY_RELEASED;
//        event.mask = 0X200;
//        event.data.keyboard.keychar = CHAR_UNDEFINED;
//        unsigned char  code =  XKeysymToKeycode(helper_disp, KeyCode);
//        unsigned char  code =  XKeysymToKeycode(helper_disp, KeyCode);
////        unsigned char code = XKeysymToKeycode(helper_disp, XStringToKeysym(KeyCode));
//        NORMAL_EX_LOG("code = %u", code);
//        event.data.keyboard.keycode = code;
//        event.data.keyboard.keychar = KeyCode;// VC_ESCAPE;
//        event.data.keyboard.rawcode = KeyCode;
//        hook_post_event(&event);
        XEvent  Xkey;
        Xkey.xkey.type = KeyRelease;
        Xkey.xkey.send_event = 0;
        Xkey.xkey.same_screen = 1;
        Xkey.xkey.keycode = /*65*/ g_unix_key_table[KeyCode]; //KeyCode;
        Xkey.xkey.state  = g_key_state;
		Xkey.xkey.display = g_display_ptr;
		Xkey.xkey.window = g_main_window;
		if (16 == KeyCode )
		{
			g_key_state &= ~ShiftMask;
		}
		else if (20 == KeyCode )
		{
			g_key_state &= ~LockMask;
		}
		else if (17 == KeyCode)
		{
			g_key_state &= ~ControlMask;
		}
		else if (18 == KeyCode)
		{
			g_key_state &= ~Mod1Mask;
		}

        if (g_main_window && g_display_ptr /*&& KeyCode == 32*/)
        {
//            XTestFakeButtonEvent(g_display_ptr, -1, True, CurrentTime);
            XSendEvent(g_display_ptr, g_main_window, True, KeyRelease, &Xkey);
//            XSync(g_display_ptr, true);
            XFlush(g_display_ptr);
        }
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		//ProcessEvent(KeyUpEvent);
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
		return true;
	}
	/**
	*keydown：按下键盘键
	*	keypress：紧接着keydown事件触发（只有按下字符键时触发）
	*	keyup：释放键盘键
	*/
	bool cinput_device::OnKeyPress(const uint8*& Data,   uint32 Size)
	{
		// TODO@chensong 2022-01-20  KeyPress -->>>>> net

		GET(FCharacterType, Character);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key up: %d"), KeyCode);
		// log key up -> KeyCode222
		FEvent KeyUpEvent(EventType::KEY_PRESS);
		KeyUpEvent.SetKeyUp(Character);
		NORMAL_LOG("OnKeyPress==KeyCode = %u", Character);
#if defined(_MSC_VER)
		WINDOW_MAIN();


		
		SET_POINT();
		WINDOW_CHILD();
		if (childwin)
		{
			MESSAGE(childwin, WM_CHAR, Character, 1);
		}
		else if (mwin)
		{
			//MESSAGE(mwin, WM_PR, Character, 0);
			MESSAGE(mwin, WM_CHAR, Character, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
#elif defined(__unix__)
//        static uiohook_event event;
//        event.type = EVENT_KEY_TYPED;
//        event.mask = 0x00;
//        event.data.keyboard.keychar = Character;
//        event.data.keyboard.keycode = Character;// VC_ESCAPE;
//        event.data.keyboard.keycode = Character;
//        hook_post_event(&event);
        XEvent  Xkey;
        Xkey.xkey.type = KeyPress;
        Xkey.xkey.send_event = 0;
        Xkey.xkey.same_screen = 1;
        Xkey.xkey.keycode = Character;
        Xkey.xkey.state  = g_key_state;
		if (16 == Character)
		{
			g_key_state |= ShiftMask;
		}
		else if (20 == Character)
		{
			g_key_state |= LockMask;
		}
		else if (17 == Character)
		{
			g_key_state |= ControlMask;
		}
		else if (18 == Character)
		{
			g_key_state |= Mod1Mask;
		}
        if (g_main_window && g_display_ptr)
        {
//            XTestFakeButtonEvent(g_display_ptr, -1, True, CurrentTime);
            XSendEvent(g_display_ptr, g_main_window, True, KeyPress, &Xkey);
//            XSync(g_display_ptr, true);
            XFlush(g_display_ptr);
        }
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		//ProcessEvent(KeyUpEvent);
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
		return true;
	}

	bool cinput_device::OnMouseEnter(const uint8*& Data,   uint32 size)
	{
		// TODO@chensong 2022-01-20  OnMouseEnter -->>>>> net 
		/*if (!m_all_consumer.insert(std::make_pair(m_mouse_id, std::map<uint32, cmouse_info>())).second)
		{
			WARNING_EX_LOG("mouse enter insert [mouse_id = %s] failed !!!", m_mouse_id.c_str());
			return false;
		}*/
        // [EnterNotify   = 7][type = 8][serial = 778][send_event = 0][display = 0x55e2607cf330][window = 0x2e00009][root = 1029][subwindow = (nil)][x = 973, y = 521][x_root = 1068, y_root = 606][mode = 0][detail = 3][same_screen = 1][focus = 1][state = 16]
#if defined(__unix__)
         XEvent  xEnter;
            xEnter.xcrossing.type = EnterNotify;
            xEnter.xcrossing.send_event = 0;
            xEnter.xcrossing.same_screen = 1;
            xEnter.xcrossing.focus = 1;
            xEnter.xcrossing.state  = 16;
            xEnter.xcrossing.detail  = 0;
            xEnter.xcrossing.mode = 0;
        if (g_main_window && g_display_ptr)
        {
//            XTestFakeButtonEvent(g_display_ptr, -1, True, CurrentTime);
            XSendEvent(g_display_ptr, g_main_window, True, EnterNotify, &xEnter);
//            XSync(g_display_ptr, true);
            XFlush(g_display_ptr);
        }
        NORMAL_EX_LOG("----------------------------------");


#endif

        return true;
		 
	}

	/** 
	*鼠标离开控制范围？
	*/
	bool cinput_device::OnMouseLeave(const uint8*& Data,   uint32 size)
	{
		// TODO@chensong 2022-01-20  OnMouseLeave 
#if defined(__unix__)
        XEvent  xEnter;
        xEnter.xcrossing.type = LeaveNotify;
        xEnter.xcrossing.send_event = 0;
        xEnter.xcrossing.same_screen = 1;
        xEnter.xcrossing.focus = 1;
        xEnter.xcrossing.state  = 16;
        xEnter.xcrossing.detail  = 0;
        xEnter.xcrossing.mode = 0;
        if (g_main_window && g_display_ptr)
        {
//            XTestFakeButtonEvent(g_display_ptr, -1, True, CurrentTime);
            XSendEvent(g_display_ptr, g_main_window, True, LeaveNotify, &xEnter);
//            XSync(g_display_ptr, true);
            XFlush(g_display_ptr);
        }
//        {
//            clock_guard lock(g_xevent_lock);
////            g_xevent.push_back(xEnter);
//        }

        NORMAL_EX_LOG("----------------------------------");

#endif

        //
//		std::map<std::string, std::map<uint32, cmouse_info>>::iterator iter =  m_all_consumer.find(m_mouse_id);
		/*if (iter == m_all_consumer.end())
		{
			WARNING_EX_LOG("mouse leave  not find  [mouse_id = %s] failed !!!", m_mouse_id.c_str());
			return false;
		}
		iter->second.clear();
		m_all_consumer.erase(iter);*/

		//std::map<uint32, cmouse_info>& cmouse_info = iter->second;
		 
		return true; 
	}
 
	


	/**
	* 鼠标按下 
	* right <-> left 
	*/
	bool cinput_device::OnMouseDown(const uint8*& Data,   uint32 Size)
	{

		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseDown at (%d, %d), button %d"), PosX, PosY, Button);
		// log mousedown -> log posX , poxY -> Button 
		//NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		//ProcessEvent(MouseDownEvent);
		/*
		g_width = PosX;
		g_height = PosY;
		*/
		
		 g_width = PosX ;
		 g_height = PosY ;
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY );
		//g_move_init = true;
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		g_main_mouse_down_up = mwin;
		SET_POINT();
		//WINDOW_CHILD();
		//WINDOW_BNTTON_DOWN(vec);
		/*if (mwin)
		{
		::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));
		}*/

		if (g_main_mouse_down_up)
		{
			MESSAGE(g_main_mouse_down_up, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		}
#elif defined(__unix__)
//        static uiohook_event event;
//        event.type = EVENT_MOUSE_PRESSED;
//        event.data.mouse.button = active_type; //MOUSE_BUTTON1;
//        event.data.mouse.x = PosX;
//        event.data.mouse.y = PosY;

//        hook_post_event(&event);

//        [ButtonPress type = 4][display = 0x5602f953e330][serial = 823][send_event = 0][display = 0x5602f953e330][window = 0x4200009][root = 1029][time][x = 893, y = 71][x_root = 1059, y_root = 229][state = 16][button = 1][same_screen = 1]
//
//        [ButtonRelease type = 5][display = 0x5602f953e330][serial = 832][send_event = 0][display = 0x5602f953e330][window = 0x4200009][root = 1029][time][x = 893, y = 71][x_root = 1059, y_root = 229][state = 272][button = 1][same_screen = 1]
        XEvent  xButton;
        xButton.xbutton.type = ButtonPress;
        xButton.xbutton.x = PosX;
        xButton.xbutton.y = PosY;
        xButton.xbutton.send_event = 0;
        xButton.xbutton.same_screen = True;
        xButton.xbutton.button = active_type;
       
		
 		 if (active_type == Button1)
        {
            g_key_state |= Button1Mask;
        }
        else if (Button2 == active_type)
        {
            g_key_state |= Button2Mask;
        }
        else if (Button3 == active_type)
        {
            g_key_state |= Button3Mask;
        }
        else
        {
            WARNING_EX_LOG("ative_type not find --->");
            return false;
        }
		 
		xButton.xbutton.state = g_key_state;
	  //  xButton.xbutton.state = 16;  // TODO@chensong 20220822  这个变量很神奇 目前只要是16 就可以哈、 新大陆你们自己玩吧
        xButton.xbutton.time = CurrentTime;
        xButton.xbutton.window = g_main_window;
//        {
//            clock_guard lock(g_xevent_lock);
////            g_xevent.push_back(xButton);
//        }

        xButton.xbutton.window = g_main_window;
        if (g_main_window && g_display_ptr)
        {
//            XTestFakeButtonEvent(g_display_ptr, -1, True, CurrentTime);
            XSendEvent(g_display_ptr, g_main_window, True, ButtonPress, &xButton);
//            XSync(g_display_ptr, true);
            XFlush(g_display_ptr);
        }

#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		return true;
	
	}

	/** 
	*松开鼠标
	* 
	*/
	bool cinput_device::OnMouseUp(const uint8*& Data,   uint32 Size)
	{
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseUp at (%d, %d), button %d"), PosX, PosY, Button);
		// log mouseup posx, posy, button 
		//NORMAL_EX_LOG("Button = %u, PosX = %d, PoxY = %d ", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d", PosX, PosY );
		FEvent MouseDownEvent(EventType::MOUSE_UP);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32  active_type;
		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		
		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height,  active_type, PosX, PosY );
		//ProcessEvent(MouseDownEvent);
		//g_move_init = false;
		#if defined(_MSC_VER)
		/*WINDOW_MAIN();
		SET_POINT();
		WINDOW_CHILD();*/
		//WINDOW_BNTTON_UP(vec);
		
		if (g_main_mouse_down_up)
		{
			{
				MESSAGE(g_main_mouse_down_up, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));
			}//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
			
			
		}
		/*if (mwin)
		{
			::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM( PosX, PosY));
		}*/
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		}
#elif defined(__unix__)
//        static uiohook_event event;
//        event.type = EVENT_MOUSE_RELEASED;
//        event.data.mouse.button = active_type; //MOUSE_BUTTON1;
//        event.data.mouse.x = PosX;
//        event.data.mouse.y = PosY;
        XEvent  xButton;
        xButton.xbutton.type = ButtonRelease;
        xButton.xbutton.x = PosX;
        xButton.xbutton.y = PosY;
//        xButton.xbutton.send_event = 0;
        xButton.xbutton.same_screen = True;
        xButton.xbutton.button = active_type;
        xButton.xbutton.time = CurrentTime;
        if (active_type == Button1)
        {
            g_key_state &= ~Button1Mask ;
        }
        else if (Button2 == active_type)
        {
            g_key_state &= ~Button2Mask ;
        }
        else if (Button3 == active_type)
        {
            g_key_state &= ~Button3Mask ;
        }
		 
        xButton.xbutton.state = g_key_state;
        xButton.xbutton.window = g_main_window;

        if (g_main_window && g_display_ptr)
        {
//            XTestFakeButtonEvent(g_display_ptr, -1, False, CurrentTime);
            XSendEvent(g_display_ptr, g_main_window, False, ButtonRelease, &xButton);
//            XSync(g_display_ptr, true);
            XFlush(g_display_ptr);
        }
//        {
//            clock_guard lock(g_xevent_lock);
////            g_xevent.push_back(xButton);
//        }
//        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		return true;
	}

	/** 
	*鼠标移动
	*/
	bool cinput_device::OnMouseMove(const uint8*& Data,   uint32 Size)
	{
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		GET(FDeltaType, DeltaX);
		GET(FDeltaType, DeltaY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseMove to (%d, %d), delta (%d, %d)"), PosX, PosY, DeltaX, DeltaY);
		// log mousemove to posx, posy, [DeltaX, DeltaY]
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d, DeltaY = %d", PosX, PosY, DeltaY);
		
		//RTC_LOG(LS_INFO) << "mousemove -->  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		_UnquantizeAndDenormalize(PosX, PosY);
		_UnquantizeAndDenormalize(DeltaX, DeltaY);
		//RTC_LOG(LS_INFO) << "mousemove <==>  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d, DeltaY = %d", g_width, g_height, PosX, PosY, DeltaY);

		FEvent MouseMoveEvent(EventType::MOUSE_MOVE);
		MouseMoveEvent.SetMouseDelta(PosX, PosY, DeltaX, DeltaY);
		
		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		
		#if defined(_MSC_VER)
		//WINDOW_MAIN();
		//WINDOW_BNTTON_UP(vec);
		
		if (g_main_mouse_down_up)
		{
			
			MESSAGE(g_main_mouse_down_up, WM_MOUSEMOVE, MAKEWPARAM(DeltaX, DeltaY), MAKEWPARAM(PosX, PosY));
			
		}
		else
		{
			// log -> error 
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
#elif defined(__unix__)
//        static uiohook_event event;
//        event.type = EVENT_MOUSE_MOVED;
//        event.data.mouse.button = MOUSE_NOBUTTON; //MOUSE_BUTTON1;
//        event.data.mouse.x = PosX;
//        event.data.mouse.y = PosY;
        {
//            XEvent event;
//            event.xclient.type = ClientMessage;
//            event.xclient.message_type = XInternAtom(g_display_ptr, "_NET_WM_MOVERESIZE", False);
//            event.xclient.display = g_display_ptr;
//            event.xclient.window = g_main_window;
//            event.xclient.format = 32;
//            event.xclient.data.l[0] = PosX;
//            event.xclient.data.l[1] = PosY;
//            event.xclient.data.l[2] = 8;
////            event.xclient.data.l[3] =
//            event.xclient.data.l[4] = 1;
//            XSendEvent(g_display_ptr, g_main_window, False, SubstructureNotifyMask | SubstructureRedirectMask, &event);
//                XSync(g_display_ptr, true);
        }
            XEvent xmouse;
            xmouse.xmotion.type = MotionNotify;
            xmouse.xmotion.x = PosX;
            xmouse.xmotion.y = PosY;
            xmouse.xmotion.x_root = PosX;
            xmouse.xmotion.y_root = PosY;
            xmouse.xmotion.send_event = 0;
            xmouse.xmotion.same_screen = True;
            xmouse.xmotion.window = g_main_window;
            xmouse.xmotion.time = CurrentTime;



            if (g_display_ptr && g_main_window)
            {
//               BadRequest  BadValue
              Status status =  XSendEvent(g_display_ptr, g_main_window, True, MotionNotify, &xmouse);
//                XSync(g_display_ptr, true);
              int xflush =  XFlush(g_display_ptr);
                NORMAL_EX_LOG("ButtonPress [status = %u][xflush = %u]", status, xflush);

            }
            else
            {
                WARNING_EX_LOG("XSendEvent g_display_ptr == %p][ g_main_window = %p]", g_display_ptr, g_main_window);
            }
//            if (g_display_ptr )
//            {
//                event.xbutton.type = KeyRelease;
////                event.xbutton.state =  0x100;
//                Status status =   XSendEvent(g_display_ptr, InputFocus,True, 0xfff, &event);
//                XFlush(g_display_ptr);
//                NORMAL_EX_LOG("XSendEvent [status = %u][x = %u, y = %u]", status, x, y);
//            }
//        }

//        hook_post_event(&event);


#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		//ProcessEvent(MouseMoveEvent);
		return true;
	}
	/** 鼠标离开控制范围？
	* 鼠标双击
	*/
	bool cinput_device::OnMouseDoubleClick(const uint8*& Data,   uint32 Size)
	{
		//WM_LBUTTONDBLCLK
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseDown at (%d, %d), button %d"), PosX, PosY, Button);
		// log mousedown -> log posX , poxY -> Button 
		//NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		//ProcessEvent(MouseDownEvent);
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d", g_width, g_height, PosX, PosY);

		//NORMAL_EX_LOG("active_type = %d, PosX = %d, PoxY = %d", active_type, PosX, PosY );
#if defined(_MSC_VER)
		WINDOW_MAIN();

		SET_POINT();
		WINDOW_CHILD();
		//WINDOW_BNTTON_DOWN(vec);
		/*if (mwin)
		{
		::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));
		}*/
		if (childwin)
		{
			MESSAGE(childwin, WM_LBUTTONDBLCLK, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));//::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		}
		else if (mwin)
		{
			MESSAGE(mwin, WM_LBUTTONDBLCLK, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		}
#elif defined(__linux__)
//        static uiohook_event event;
//        event.type = EVENT_MOUSE_CLICKED;
//        event.data.mouse.button = active_type; //MOUSE_BUTTON1;
//        event.data.mouse.x = PosX;
//        event.data.mouse.y = PosY;

//        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		 
		return true;
	}

	/** 
	* 鼠标的滚轮滚动 
	*/
	bool cinput_device::OnMouseWheel(const uint8*& Data,   uint32 Size)
	{
		GET(FDeltaType, Delta);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseWheel, delta %d"), Delta);
		// mouseWheel delta -> 
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseWheelEvent(EventType::MOUSE_WHEEL);
		MouseWheelEvent.SetMouseWheel(Delta, PosX, PosY);
		//ProcessEvent(MouseWheelEvent);
		/*g_width = PosX;
		g_height = PosY;*/
		 g_width = PosX ;
		 g_height = PosY ;
        NORMAL_EX_LOG(" [Delta = %u][PosX = %d] [PoxY = %d]", Delta, PosX, PosY);
		#if defined(_MSC_VER)
		WINDOW_MAIN();

		if (mwin)
		{
			//::PostMessage(mwin, WM_MOUSEWHEEL, MAKEWPARAM(0, Delta) /* ascii码 */, MAKELPARAM(PosX, PosY));
			MESSAGE(mwin, WM_MOUSEWHEEL, MAKEWPARAM(0, Delta) /* ascii码 */, MAKELPARAM(PosX, PosY));
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log error 
			return false;
		}
#elif defined(__linux__)

        {
//            xcb_get_extension_data
            XEvent  xButton;
            xButton.xbutton.type = ButtonPress;
            xButton.xbutton.x = PosX;
            xButton.xbutton.y = PosY;
            xButton.xbutton.x_root = PosX;
            xButton.xbutton.y_root = PosY;
            xButton.xbutton.send_event = False;
            xButton.xbutton.same_screen = True;
            xButton.xbutton.button = Delta > 0 ? Button4 : Button5;
//            xButton.xbutton.button = Button4;
            xButton.xbutton.time = CurrentTime;
            xButton.xbutton.window = g_main_window;
            if (g_main_window && g_display_ptr)
            {
                Status status = XSendEvent(g_display_ptr, g_main_window, True, ButtonPress , &xButton);
                int xflush = XFlush(g_display_ptr);
                NORMAL_EX_LOG("ButtonPress [status = %u][xflush = %u]", status, xflush);
            }

            XEvent  xWheel;
            xWheel.xbutton.type = ButtonRelease;
            xWheel.xbutton.display = g_display_ptr;
            xWheel.xbutton.x = PosX;
            xWheel.xbutton.y = PosY;
            xWheel.xbutton.x_root = PosX;
            xWheel.xbutton.y_root = PosY;
            xWheel.xbutton.send_event = False;
            xWheel.xbutton.same_screen = True;
            xWheel.xbutton.button = Delta > 0 ? Button4 : Button5;
//            xWheel.xbutton.button = Button4;
            xWheel.xbutton.time = CurrentTime;
            xWheel.xbutton.window = g_main_window;
            if (g_main_window && g_display_ptr)
            {
                Status status =     XSendEvent(g_display_ptr, g_main_window, False, ButtonRelease , &xWheel);
                int xflush = XFlush(g_display_ptr);
                NORMAL_EX_LOG("BUttonRelease [status = %u][xflush = %u]", status, xflush);
            }
        }

#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		return true;
	}

    bool cinput_device::init_keydown()
    {
//        m_keydown_map.insert()
//        REGISTER_KEYDOWN(0	, NUT);
//        REGISTER_KEYDOWN(1	, SOH);
//        REGISTER_KEYDOWN(2	, STX);
//        REGISTER_KEYDOWN(3	, ETX);
//        REGISTER_KEYDOWN(4	, EOT);
//        REGISTER_KEYDOWN(5	, ENQ);
//        REGISTER_KEYDOWN(6	, ACK);
//        REGISTER_KEYDOWN(7	, BEL);
//        REGISTER_KEYDOWN(8	, BS);
//        REGISTER_KEYDOWN(9	, HT);
//        REGISTER_KEYDOWN(10	, LF);
//        REGISTER_KEYDOWN(11	, VT);
//        REGISTER_KEYDOWN(12	, FF);
//        REGISTER_KEYDOWN(13	, CR);
//        REGISTER_KEYDOWN(14	, SO);
//        REGISTER_KEYDOWN(15	, SI);
//        REGISTER_KEYDOWN(16	, DLE);
//        REGISTER_KEYDOWN(17	, DCI);
//        REGISTER_KEYDOWN(18	, DC2);
//        REGISTER_KEYDOWN(19	, DC3);
//        REGISTER_KEYDOWN(20	, DC4);
//        REGISTER_KEYDOWN(21	, NAK);
//        REGISTER_KEYDOWN(22	, SYN);
//        REGISTER_KEYDOWN(23	, TB);
//        REGISTER_KEYDOWN(24	, CAN);
//        REGISTER_KEYDOWN(25	, EM);
//        REGISTER_KEYDOWN(26	, SUB);
//        REGISTER_KEYDOWN(27	, ESC);
//        REGISTER_KEYDOWN(32, (space);
//        REGISTER_KEYDOWN(	33,	!);
//        REGISTER_KEYDOWN(	34,	");
//        REGISTER_KEYDOWN(	35,	#);
//        REGISTER_KEYDOWN(	36,	$);
//        REGISTER_KEYDOWN(	37,	%);
//        REGISTER_KEYDOWN(	38,	&);
//        REGISTER_KEYDOWN(	39,	,);
//        REGISTER_KEYDOWN(	40,	();
//        REGISTER_KEYDOWN(	41,	));
//        REGISTER_KEYDOWN(	42,	*);
//        REGISTER_KEYDOWN(	43,	+);
//        REGISTER_KEYDOWN(	44,	,);
//        REGISTER_KEYDOWN(	45,	-);
//        REGISTER_KEYDOWN(	46,	.);
//        REGISTER_KEYDOWN(	47,	/);
//        REGISTER_KEYDOWN(	48,	VC_0);
//        REGISTER_KEYDOWN(	49,	VC_1);
//        REGISTER_KEYDOWN(	50,	VC_2);
//        REGISTER_KEYDOWN(	51,	VC_3);
//        REGISTER_KEYDOWN(	52,	VC_4);
//        REGISTER_KEYDOWN(	53,	VC_5);
//        REGISTER_KEYDOWN(	54,	VC_6);
//        REGISTER_KEYDOWN(	55,	VC_7);
//        REGISTER_KEYDOWN(	56,	VC_8);
//        REGISTER_KEYDOWN(	57,	VC_9);
//        REGISTER_KEYDOWN(	58,	:);
//        REGISTER_KEYDOWN(	59,	;);
//        REGISTER_KEY鼠标离开控制范围？DOWN(64,	@);
//        REGISTER_KEYDOWN(65,	A);
//        REGISTER_KEYDOWN(66,	B);
//        REGISTER_KEYDOWN(67,	C);
//        REGISTER_KEYDOWN(68,	D);
//        REGISTER_KEYDOWN(69,	E);
//        REGISTER_KEYDOWN(70,	F);
//        REGISTER_KEYDOWN(71,	G);
//        REGISTER_KEYDOWN(72,	H);
//        REGISTER_KEYDOWN(73,	I);
//        REGISTER_KEYDOWN(74,	J);
//        REGISTER_KEYDOWN(75,	K);
//        REGISTER_KEYDOWN(76,	L);
//        REGISTER_KEYDOWN(77,	M);
//        REGISTER_KEYDOWN(78,	N);
//        REGISTER_KEYDOWN(79,	O);
//        REGISTER_KEYDOWN(80,	P);
//        REGISTER_KEYDOWN(81,	Q);
//        REGISTER_KEYDOWN(82,	R);
//        REGISTER_KEYDOWN(83,	S);
//        REGISTER_KEYDOWN(84,	T);
//        REGISTER_KEYDOWN(85,	U);
//        REGISTER_KEYDOWN(86,	V);
//        REGISTER_KEYDOWN(87,	W);
//        REGISTER_KEYDOWN(88,	X);
//        REGISTER_KEYDOWN(89,	Y);
//        REGISTER_KEYDOWN(90,	Z);
//        REGISTER_KEYDOWN(91,	[);
//        REGISTER_KEYDOWN(96,	、);
//        REGISTER_KEYDOWN(97,	a);
//        REGISTER_KEYDOWN(98,	b);
//        REGISTER_KEYDOWN(99,	c);
//        REGISTER_KEYDOWN(100,	d);
//        REGISTER_KEYDOWN(101,	e);
//        REGISTER_KEYDOWN(102,	f);
//        REGISTER_KEYDOWN(103,	g);
//        REGISTER_KEYDOWN(104,	h);
//        REGISTER_KEYDOWN(105,	i);
//        REGISTER_KEYDOWN(106,	j);
//        REGISTER_KEYDOWN(107,	k);
//        REGISTER_KEYDOWN(108,	l);
//        REGISTER_KEYDOWN(109,	m);
//        REGISTER_KEYDOWN(110,	n);
//        REGISTER_KEYDOWN(111,	o);
//        REGISTER_KEYDOWN(112,	p);
//        REGISTER_KEYDOWN(113,	q);
//        REGISTER_KEYDOWN(114,	r);
//        REGISTER_KEYDOWN(115,	s);
//        REGISTER_KEYDOWN(116,	t);
//        REGISTER_KEYDOWN(117,	u);
//        REGISTER_KEYDOWN(118,	v);
//        REGISTER_KEYDOWN(119,	w);
//        REGISTER_KEYDOWN(120,	x);
//        REGISTER_KEYDOWN(121,	y);
//        REGISTER_KEYDOWN(122,	z);
//        REGISTER_KEYDOWN(123,	{);

        return true;
    }
	// XY positions are the ratio (0.0..1.0) along a viewport axis, quantized
	// into an uint16 (0..65536). This allows the browser viewport and player
	// viewport to have a different size.
	void cinput_device::_UnquantizeAndDenormalize(uint16& InOutX, uint16& InOutY)
	{
		InOutX = InOutX / 65536.0f * m_int_point.X;
		InOutY = InOutY / 65536.0f * m_int_point.Y;
	}
	
	// XY deltas are the ratio (-1.0..1.0) along a viewport axis, quantized
	// into an int16 (-32767..32767). This allows the browser viewport and
	// player viewport to have a different size.
	void cinput_device::_UnquantizeAndDenormalize(int16& InOutX, int16& InOutY)
	{ 
		InOutX = InOutX / 32767.0f * m_int_point.X;
		InOutY = InOutY / 32767.0f * m_int_point.Y;
	}
}
