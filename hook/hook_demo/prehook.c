#define _GNU_SOURCE
#include <sys/types.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
//#include <opencl-c.h>
#include <GL/glu.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
//#include <Xlib.h>
#include <X11/Xlib.h>
#include <EGL/egl.h>
#include <GL/glx.h>
//#include "test.h"
//#include <gtk/gtk.h>

//#include <GL/wglew.h>
// #include <GL/glew.h>
//typedef void	(*t_glFlush)(void);
//FILE  * out_log_file_ptr = fopen("./log/hook.log", "wb+");
//#include <>


#include <xcb/xproto.h>
#define gettid() syscall(__NR_gettid)
//static void (*real_glFlush)(void   ) = NULL;
//static void (*glTexImage2DFuncPtr)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*) = NULL;

static void (*hook_glDrawArrays)(GLenum mode, GLint first, GLsizei count) = NULL;
static FILE  *out_gl_ptr = NULL;
static unsigned char * rgba_ptr = NULL;
static int TICK_TIME = 1000;
void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
      if (!hook_glDrawArrays)
    {
        hook_glDrawArrays = dlsym(RTLD_NEXT, "glDrawArrays");
    }

    



 struct timeval tTimeVal;
    gettimeofday(&tTimeVal, NULL);
    struct tm *tTM = localtime(&tTimeVal);
    char buffer[1024] = {0};
    (void)sprintf(buffer, "[hook_glDrawArrays][%s][%d] ======================>[%04d-%02d-%02d %02d:%02d:%02d.%03ld.%03ld]\n", __FUNCTION__, __LINE__,  tTM->tm_year +1900, tTM->tm_mon +1, tTM->tm_mday, tTM->tm_hour, tTM->tm_min, tTM->tm_sec, tTimeVal.tv_usec/ 1000, tTimeVal.tv_usec %1000);
   //(void)write(1, buffer, strlen(buffer));



    hook_glDrawArrays(mode, first, count);
}

static  void (*hook_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*) = NULL;

void glTextImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
      if (!hook_glTexImage2D)
    {
        hook_glTexImage2D = dlsym(RTLD_NEXT, "glTextImage2D");
    }
    char buffer[1024] = {0};
    (void)sprintf(buffer, "[hook_glTexImage2D] ======================>\n" );
    (void)write(1, buffer, strlen(buffer));
    hook_glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}
/**
 *
 *
 * #define KeyPress		2
case KeyRelease		3
case ButtonPress		4
case ButtonRelease		5
case MotionNotify		6
case EnterNotify		7
case LeaveNotify		8
case FocusIn			9
case FocusOut		10
case KeymapNotify		11
case Expose			12
case GraphicsExpose		13
case NoExpose		14
case VisibilityNotify	15
case CreateNotify		16
case DestroyNotify		17
case UnmapNotify		18
case MapNotify		19
case MapRequest		20
case ReparentNotify		21
case ConfigureNotify		22
case ConfigureRequest	23
case GravityNotify		24
case ResizeRequest		25
case CirculateNotify		26
case CirculateRequest	27
case PropertyNotify		28
case SelectionClear		29
case SelectionRequest	30
case SelectionNotify		31
case ColormapNotify		32
case ClientMessage		33
case MappingNotify		34
case GenericEvent		35
case LASTEvent		36
 */
//XNextEvent
static unsigned  long g_new_seria = 1000;
static unsigned  long g_count = 0;
void show(const char * str, size_t len)
{
//    char buffer[10240] = {0};
//    (void) sprintf(buffer, "real_XNextEvent %s [treal_glutSwapBuffersime_now :%d-%d-%d %d:%d:%d.%ld]\n", str, 1900 + t->tm_year,
//                   1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec);
    (void) write(1, str, len);
    (void) write(1, "\n", 1);
}
// void show_hook_info(Display *display, const XEvent* e)
// {
//     char buffer[102400] = {0};

//     switch (e->type)
//     {
//         case KeyPress:
//         {
// //            typedef struct {
// //                int type;		/* of event */
// //                unsigned long serial;	/* # of last request processed by server */
// //                Bool send_event;	/* true if this came from a SendEvent request */
// //                Display *display;	/* Display the event was read from */
// //                Window window;	        /* "event" window it is reported relative to */
// //                Window root;	        /* root window that the event occurred on */
// //                Window subwindow;	/* child window */
// //                Time time;		/* milliseconds */
// //                int x, y;		/* pointer x, y coordinates in event window */
// //                int x_root, y_root;	/* coordinates relative to root */
// //                unsigned int state;	/* key or button mask */
// //                unsigned int keycode;	/* detail */
// //                Bool same_screen;	/* same screen flag */
// //            } XKeyEvent;
//             int len = sprintf(buffer, "[KeyPress  type = %u][display = %p][serial = %u][send_event = %d][display = %p][window = %p][root = %u][subwindow = %p][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][keycode = %u][same_screen = = %d]\n",
//                           e->xkey.type, display, e->xkey.serial, e->xkey.send_event, e->xkey.display, e->xkey.window, e->xkey.root, e->xkey.subwindow, e->xkey.x, e->xkey.y, e->xkey.x_root, e->xkey.y_root, e->xkey.state, e->xkey.keycode, e->xkey.same_screen);
//             show(buffer, len);
//             break;
//         }
//         case KeyRelease:
//         {
//             int len =  sprintf(buffer, "[KeyRelease  type = %u][display = %p][serial = %u][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][keycode = %u][same_screen = = %d]\n",
//                           e->xkey.type, display, e->xkey.serial, e->xkey.send_event, e->xkey.display, e->xkey.window, e->xkey.root, e->xkey.subwindow, e->xkey.x, e->xkey.y, e->xkey.x_root, e->xkey.y_root, e->xkey.state, e->xkey.keycode, e->xkey.same_screen);
//             show(buffer, len);
//             break;
//         }
//         case ButtonPress:
//         {
//             //
// //            typedef struct {
// //                int type;		/* of event */
// //                unsigned long serial;	/* # of last request processed by server */
// //                Bool send_event;	/* true if this came from a SendEvent request */
// //                Display *display;	/* Display the event was read from */
// //                Window window;	        /* "event" window it is reported relative to */
// //                Window root;	        /* root window that the event occurred on */
// //                Window subwindow;	/* child window */
// //                Time time;		/* milliseconds */
// //                int x, y;		/* pointer x, y coordinates in event window */
// //                int x_root, y_root;	/* coordinates relative to root */
// //                unsigned int state;	/* key or button mask */
// //                unsigned int button;	/* detail */
// //                Bool same_screen;	/* same screen flag */
// //            } XButtonEvent;
//             int len =  sprintf(buffer, "[ButtonPress type = %u][display = %p][serial = %llu][send_event = %d][display = %p][window = %u][root = %u][time][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][button = %u][same_screen = %d]",
//                           e->xbutton.type, display, e->xbutton.serial, e->xbutton.send_event, e->xbutton.display, e->xbutton.window, e->xbutton.root, e->xbutton.x, e->xbutton.y, e->xbutton.x_root, e->xbutton.y_root, e->xbutton.state, e->xbutton.button, e->xbutton.same_screen);
//             show(buffer, len);
//             break;
//         }
//         case ButtonRelease:
//         {
//             int len =  sprintf(buffer, "[ButtonRelease type = %u][display = %p][serial = %llu][send_event = %d][display = %p][window = %u][root = %u][time][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][button = %u][same_screen = %d]",
//                           e->xbutton.type, display, e->xbutton.serial, e->xbutton.send_event, e->xbutton.display, e->xbutton.window, e->xbutton.root, e->xbutton.x, e->xbutton.y, e->xbutton.x_root, e->xbutton.y_root, e->xbutton.state, e->xbutton.button, e->xbutton.same_screen);
//             show(buffer, len);
//             break;
//         }
//         case MotionNotify:
//         {

// //            typedef struct {
// //                int type;		/* of event */
// //                unsigned long serial;	/* # of last request processed by server */
// //                Bool send_event;	/* true if this came from a SendEvent request */
// //                Display *display;	/* Display the event was read from */
// //                Window window;	        /* "event" window reported relative to */
// //                Window root;	        /* root window that the event occurred on */
// //                Window subwindow;	/* child window */
// //                Time time;		/* milliseconds */
// //                int x, y;		/* pointer x, y coordinates in event window */
// //                int x_root, y_root;	/* coordinates relative to root */
// //                unsigned int state;	/* key or button mask */
// //                char is_hint;		/* detail */
// //                Bool same_screen;	/* same screen flag */
// //            } XMotionEvent;

//             int len =  sprintf(buffer, "[MotionNotify type = %u][ display = %p][serial = %u][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][is_hint = %c][same_screen = %d]\n",
//                           e->xmotion.type, display, e->xmotion.serial, e->xmotion.send_event, e->xmotion.display, e->xmotion.window, e->xmotion.root, e->xmotion.subwindow, e->xmotion.x, e->xmotion.y, e->xmotion.x_root, e->xmotion.y_root, e->xmotion.state, e->xmotion.is_hint, e->xmotion.same_screen);

//            show(buffer, len);
//             break;
//         }
//         case EnterNotify:
//         {
// //            typedef struct {
// //                int type;		/* of event */
// //                unsigned long serial;	/* # of last request processed by server */
// //                Bool send_event;	/* true if this came from a SendEvent request */
// //                Display *display;	/* Display the event was read from */
// //                Window window;	        /* "event" window reported relative to */
// //                Window root;	        /* root window that the event occurred on */
// //                Window subwindow;	/* child window */
// //                Time time;		/* milliseconds */
// //                int x, y;		/* pointer x, y coordinates in event window */
// //                int x_root, y_root;	/* coordinates relative to root */
// //                int mode;		/* NotifyNormal, NotifyGrab, NotifyUngrab */
// //                int detail;
// //                /*
// //                 * NotifyAncestor, NotifyVirtual, NotifyInferior,
// //                 * NotifyNonlinear,NotifyNonlinearVirtual
// //                 */
// //                Bool same_screen;	/* same screen flag */
// //                Bool focus;		/* boolean focus */
// //                unsigned int state;	/* key or button mask */
// //            } XCrossingEvent;
//             int len =  sprintf(buffer, "[EnterNotify   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u]"
//                                        "[x = %u, y = %u][x_root = %u, y_root = %u][mode = %u][detail = %u][same_screen = %d][focus = %d][state = %u]",
//                                        EnterNotify, e->xcrossing.type, e->xcrossing.serial, e->xcrossing.send_event, e->xcrossing.display, e->xcrossing.window,
//                                        e->xcrossing.root, e->xcrossing.subwindow, e->xcrossing.x, e->xcrossing.y, e->xcrossing.x_root, e->xcrossing.y_root, e->xcrossing.mode, e->xcrossing.detail,
//                                        e->xcrossing.same_screen, e->xcrossing.focus, e->xcrossing.state);
//             show(buffer, len);
//             break;
//         }
//         case LeaveNotify:
//         {
//             int len =  sprintf(buffer, "[EnterNotify   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][root = %u][subwindow = %u]"
//                                        "[x = %u, y = %u][x_root = %u, y_root = %u][mode = %u][detail = %u][same_screen = %d][focus = %d][state = %u]",
//                                EnterNotify, e->xcrossing.type, e->xcrossing.serial, e->xcrossing.send_event, e->xcrossing.display, e->xcrossing.window,
//                                e->xcrossing.root, e->xcrossing.subwindow, e->xcrossing.x, e->xcrossing.y, e->xcrossing.x_root, e->xcrossing.y_root, e->xcrossing.mode, e->xcrossing.detail,
//                                e->xcrossing.same_screen, e->xcrossing.focus, e->xcrossing.state);
//             show(buffer, len);
//             break;
//         }
//         case FocusIn:
//         {
// //            typedef struct {
// //                int type;		/* FocusIn or FocusOut */
// //                unsigned long serial;	/* # of last request processed by server */
// //                Bool send_event;	/* true if this came from a SendEvent request */
// //                Display *display;	/* Display the event was read from */
// //                Window window;		/* window of event */
// //                int mode;		/* NotifyNormal, NotifyWhileGrabbed,
// //				   NotifyGrab, NotifyUngrab */
// //                int detail;
// //                /*
// //                 * NotifyAncestor, NotifyVirtual, NotifyInferior,
// //                 * NotifyNonlinear,NotifyNonlinearVirtual, NotifyPointer,
// //                 * NotifyPointerRoot, NotifyDetailNone
// //                 */
// //            } XFocusChangeEvent;
//             int len =  sprintf(buffer, "[FocusIn   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][mode = %u][detail = %u]",
//                                FocusIn, e->xfocus.type, e->xfocus.serial, e->xfocus.send_event, e->xfocus.display, e->xfocus.window, e->xfocus.mode, e->xfocus.detail);
//             show(buffer, len);
//             break;
//         }
//         case FocusOut:
//         {
//             int len =  sprintf(buffer, "[FocusIn   = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][mode = %u][detail = %u]",
//                                FocusIn, e->xfocus.type, e->xfocus.serial, e->xfocus.send_event, e->xfocus.display, e->xfocus.window, e->xfocus.mode, e->xfocus.detail);
//             show(buffer, len);
//             break;
//         }
//         case KeymapNotify:
//         {
//             int len =  sprintf(buffer, "[KeymapNotify   = %d]", KeymapNotify);
//             show(buffer, len);
//             break;
//         }
//         case Expose:
//         {
//             int len =  sprintf(buffer, "Expose   = %d", Expose);
//             show(buffer, len);
//             break;
//         }
//         case GraphicsExpose:
//         {
//             int len =  sprintf(buffer, "GraphicsExpose   = %d", GraphicsExpose);
//             show(buffer, len);
//             break;
//         }
//         case NoExpose:
//         {
//             int len =  sprintf(buffer, "NoExpose   = %d", NoExpose);
//             show(buffer, len);
//             break;
//         }
//         case VisibilityNotify:
//         {
//             int len =  sprintf(buffer, "VisibilityNotify   = %d", VisibilityNotify);
//             show(buffer, len);
//             break;
//         }
//         case CreateNotify:
//         {
//             int len =  sprintf(buffer, "CreateNotify   = %d", CreateNotify);
//             show(buffer, len);
//             break;
//         }
//         case DestroyNotify:
//         {
//             int len =  sprintf(buffer, "DestroyNotify   = %d", DestroyNotify);
//             show(buffer, len);
//             break;
//         }
//         case UnmapNotify:
//         {
//             int len =  sprintf(buffer, "UnmapNotify   = %d", UnmapNotify);
//             show(buffer, len);
//             break;
//         }
//         case MapNotify:
//         {
//             int len =  sprintf(buffer, "MapNotify   = %d", MapNotify);
//             show(buffer, len);
//             break;
//         }
//         case MapRequest:
//         {
//             int len =  sprintf(buffer, "MapRequest   = %d", MapRequest);
//             show(buffer, len);
//             break;
//         }
//         case ReparentNotify:
//         {
//             int len =  sprintf(buffer, "ReparentNotify   = %d", ReparentNotify);
//             show(buffer, len);
//             break;
//         }
//         case ConfigureNotify:
//         {
//             int len =  sprintf(buffer, "ConfigureNotify   = %d", ConfigureNotify);
//             show(buffer, len);
//             break;
//         }
//         case ConfigureRequest:
//         {
//             int len =  sprintf(buffer, "ConfigureRequest   = %d", ConfigureRequest);
//             show(buffer, len);
//             break;
//         }
//         case GravityNotify:
//         {
//             int len =  sprintf(buffer, "GravityNotify   = %d", GravityNotify);
//             show(buffer, len);
//             break;
//         }
//         case ResizeRequest:
//         {
//             int len =  sprintf(buffer, "ResizeRequest   = %d", ResizeRequest);
//             show(buffer, len);
//             break;
//         }
//         case CirculateNotify:
//         {
//             int len =  sprintf(buffer, "CirculateNotify   = %d", CirculateNotify);
//             show(buffer, len);
//             break;
//         }
//         case CirculateRequest:
//         {
//             int len =  sprintf(buffer, "CirculateRequest   = %d", CirculateRequest);
//             show(buffer, len);
//             break;
//         }
//         case PropertyNotify:
//         {
// //            typedef struct {
// //                int type;
// //                unsigned long serial;	/* # of last request processed by server */
// //                Bool send_event;	/* true if this came from a SendEvent request */
// //                Display *display;	/* Display the event was read from */
// //                Window window;
// //                Atom atom;
// //                Time time;
// //                int state;		/* NewValue, Deleted */
// //            } XPropertyEvent;
//             int len =  sprintf(buffer, "[PropertyNotify = %d][type = %u][serial = %lu][send_event = %d][display = %p][window = %u][atom = %d][state = %u]", PropertyNotify, e->xproperty.type, e->xproperty.serial, e->xproperty.send_event, e->xproperty.display, e->xproperty.window, e->xproperty.atom, e->xproperty.state);
//             show(buffer, len);
//             break;
//         }
//         case SelectionClear:
//         {
//             int len =  sprintf(buffer, "SelectionClear   = %d", SelectionClear);
//             show(buffer, len);
//             break;
//         }
//         case SelectionRequest:
//         {
//             int len =  sprintf(buffer, "SelectionRequest   = %d", SelectionRequest);
//             show(buffer, len);
//             break;
//         }
//         case SelectionNotify:
//         {
//             int len =  sprintf(buffer, "SelectionNotify   = %d", SelectionNotify);
//             show(buffer, len);
//             break;
//         }
//         case ColormapNotify:
//         {
//             int len =  sprintf(buffer, "ColormapNotify   = %d", ColormapNotify);
//             show(buffer, len);
//             break;
//         }
//         case ClientMessage:
//         {
//             int len =  sprintf(buffer, "ClientMessagreal_glutSwapBufferse   = %d", ClientMessage);
//             show(buffer, len);
//             break;
//         }
//         case MappingNotify:
//         {
//             int len =  sprintf(buffer, "MappingNotify   = %d", MappingNotify);
//             show(buffer, len);
//             break;
//         }
//         case GenericEvent:
//         {
//             int len =  sprintf(buffer, "GenericEvent   = %d", GenericEvent);
//             show(buffer, len);
//             break;
//         }
//         case LASTEvent:
//         {
//             int len =  sprintf(buffer, "LASTEvent   = %d", LASTEvent);
//             show(buffer, len);
//             break;
//         }
//         default:
//         {
//             char buffer[1024] = {0};
//             int len =  sprintf(buffer, "unknow type = %u\n", e->type);
//             show(buffer, len);
//             break;
//         }
//     }
// }



// void call_time_out(const char * str)
// {
//     struct timeval tv;
//     struct timezone tz;
//     struct tm * t;
//     gettimeofday(&tv, &tz);
//     t = localtime(&tv.tv_sec);
//     char buffer[10240] = {0};
//     (void) sprintf(buffer, "real_XNextEvent %s [time_now :%d-%d-%d %d:%d:%d.%ld][g_count = %lu]\n", str, 1900 + t->tm_year,
//                    1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec, g_count++);
//     (void) write(1, buffer, strlen(buffer));
// }
// typedef  int (*Hook_XNextEvent)( Display*	d 	/* display */, XEvent*		e/* event_return */ )   ;
// static  Hook_XNextEvent   real_XNextEvent = NULL;
//   int XNextEvent( Display*	d	/* display */, XEvent*		e/* event_return */ )
//   {
//       if (!real_XNextEvent)
//     {
//         real_XNextEvent = dlsym(RTLD_NEXT, "XNextEvent");
//     }

//       int ret = real_XNextEvent(d, e);
//       {
// //          e->xbutton.serial + g_new_seria + 1;
// ////          e->xbutton.window = 0; //  []
// //          e->xbutton.y_root = 0;
// //          e->xbutton.x_root = 0;
// ////          e->xbutton.root = 0;
// ////          int x = e->xbutton.x_root - e->xbutton.x;
// ////          int y = e->xbutton.y_root - e->xbutton.y;
// ////          e->xbutton.x =   200;
// ////          e->xbutton.y = 200;
// ////          e->xbutton.x_root += e->xbutton.x;
// ////          e->xbutton.y_root += e->xbutton.y;
//         if (ButtonRelease == e->type || ButtonPress == e->type)
//         {
//             e->xbutton.state = 0;
//             e->xbutton.x_root = 0;
//             e->xbutton.y_root = 0;
//             e->xbutton.root = 0;
//             e->xbutton.send_event = 0;
//             e->xbutton.same_screen = 0;
//         }
//           show_hook_info(d, e);


//       }
//       char buffer[102400] = {0};
//       (void)sprintf(buffer, "[XNextEvent][display = %p][ ret = %u][g_count = %lu]\n", d,    ret, g_count++);
//       (void)write(1, buffer, strlen(buffer));
//     return ret;
//   }

//   typedef int (*Hook_XSelectInput)(  Display*	display	/* display */, Window	w	/* w */, long		event_mask/* event_mask */ )  ;

//   static Hook_XSelectInput real_XSelectInput = NULL;

//   int  XSelectInput( Display* display		/* display */, Window	w 	/* w */, long	event_mask	/* event_mask */ )
//   {
//       if (!real_XSelectInput)
//       {
//           real_XSelectInput = dlsym(RTLD_NEXT, "XSelectInput");
//       }

//       int size  = real_XSelectInput(display, w, event_mask);
//       char buffer[102400] = {0};
//       (void)sprintf(buffer, "[real_XSelectInput][display = %p][window = %u][event_mask = %lu][ret = %u][g_count = %lu]\n", display, w, event_mask, size, g_count++);
//       (void)write(1, buffer, strlen(buffer));
//       return size;
//   }
// typedef   int  (*Hook_XPending)(  Display*	display	/* display */ )  ;
// static Hook_XPending  real_XPending = NULL;

// int XdPending( Display*	display	/* display */ )
// {
//         if (!real_XPending)
//         {
//             real_XPending = dlsym(RTLD_NEXT, "XPending");
//         }

//         int size = real_XPending(display );
// //        char buffer[102400] = {0};
// //
// //        (void)sprintf(buffer, "[real_XPending][display = %p][size = %u][g_count = %lu] \n", display, size, g_count++);
// //        (void)write(1, buffer, strlen(buffer));

// //    XPeekEvent
//         return size;
// }

// typedef   int (*Hook_XPeekEvent)(  Display*		  display  , XEvent*		  event_return   );
// static Hook_XPeekEvent real_XPeekEvent = NULL;

// int XPeekEvent(  Display*		  display , XEvent*		  event_return   )
// {
//     if (!real_XPeekEvent)
//     {
//         real_XPeekEvent = dlsym(RTLD_NEXT, "XPeekEvent");
//     }
// //    XGetEventData
//     int size = real_XPeekEvent(display , event_return);
//     char buffer[102400] = {0};

//     (void)sprintf(buffer, "[real_XPeekEvent][display = %p][size = %u][g_count = %lu] \n", display, size, g_count++);
//     (void)write(1, buffer, strlen(buffer));

// //    XPeekEvent
//     return size;
// }
// typedef    Bool (*Hook_XGetEventData)( Display*			  dpy  ,  XGenericEventCookie*	  cookie );
// static Hook_XGetEventData real_XGetEventData= NULL;

// Bool ddXGetEventData( Display*			  dpy  ,  XGenericEventCookie*	  cookie )
// {
// //    XGetEventData
//     if (!real_XGetEventData)
//     {
//         real_XGetEventData = dlsym(RTLD_NEXT, "XGetEventData");
//     }
// //    XGetEventData
//     Bool size = real_XGetEventData(dpy , cookie);
//     char buffer[102400] = {0};

//     (void)sprintf(buffer, "[real_XGetEventData][display = %p][size = %d][g_count = %lu] \n", dpy, size, g_count++);
//     (void)write(1, buffer, strlen(buffer));

// //    XPeekEvent
//     return size;
// }
// typedef int (*Hook_XMoveWindow)( Display*		 display  , Window		  w  , int			  x  , int			  y   );
// static Hook_XMoveWindow real_XMoveWindow = NULL;
// int XMoveWindow( Display*		 display  , Window		  w  , int			  x  , int			  y   )
// {
//     if (!real_XMoveWindow)
//     {
//         real_XMoveWindow = dlsym(RTLD_NEXT, "XMoveWindow");
//     }
// //    XGetEventData
//     int size = real_XMoveWindow(display , w, x, y);
//     char buffer[102400] = {0};

//     (void)sprintf(buffer, "[real_XMoveWindow][display = %p][size = %d][g_count = %lu] \n", display, size, g_count++);
//     (void)write(1, buffer, strlen(buffer));
// //    XIfEvent()
// //    XPeekEvent
//     return size;
// }

// typedef   int (*Hook_XIfEvent)( Display*		  display  , XEvent*		 event_return  ,
//         Bool (*predicate) ( Display*			  display , XEvent*			 event  , XPointer			  arg  )		 , XPointer		  arg);
// static Hook_XIfEvent real_XIfEvent = NULL;

// int  XIfEvent( Display*		  display  , XEvent*		 event_return  ,
//                       Bool (*predicate) ( Display*			  display , XEvent*			 event  , XPointer			  arg  )		 , XPointer		  arg)
// {
//     if (!real_XIfEvent)
//     {
//         real_XIfEvent = dlsym(RTLD_NEXT, "XIfEvent");
//     }
// //    XGetEventData
//     int size = real_XIfEvent(display , event_return, predicate, arg);
//     char buffer[102400] = {0};

//     (void)sprintf(buffer, "[real_XIfEvent][display = %p][size = %d][g_count = %lu] \n", display, size, g_count++);
//     (void)write(1, buffer, strlen(buffer));
// //    XIfEvent()
// //    XPeekEvent
//     return size;
// }

//  gtk_widget_add_event();
//  gtk_widget_add_event();
//
static void (*real_glFinish)(void   ) = NULL;
static void (*real_glFlush)(void   ) = NULL;

typedef int EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;
typedef void* EGLConfig;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLSurface;


//static EGLBoolean (  * real_eglSwapBuffers)(EGLDisplay,EGLSurface) = NULL;
static void (  * real_eglSwapBuffers)(Display *d, GLXDrawable e) = NULL;
static EGLBoolean (  * real_eglSwapInterval)(EGLDisplay,EGLint) = NULL;

static EGLBoolean (* real_eglMakeCurrent)(EGLDisplay,EGLSurface,EGLSurface,EGLContext) = NULL;

static GLenum  (*real_glewInit) (void) = NULL;


static  void     (*real_glutSwapBuffers)( void ) = NULL;


  void      glutSwapBuffers( void )
{

    if (!real_glutSwapBuffers)
   {
       real_glutSwapBuffers = dlsym(RTLD_NEXT, "glutSwapBuffers");
   }
   char buffer[100];
   (void) sprintf(buffer, "%lu glutSwapBuffers   == %p\n", gettid(),   real_glutSwapBuffers);
   (void) write(1, buffer, strlen(buffer));
   return real_glutSwapBuffers();
}
// static void (*real_glutSwapBuffers)(void) = NULL;

GLenum  glewInit (void)
{
   if (!real_glewInit)
   {
       real_glewInit = dlsym(RTLD_NEXT, "glewInit");
   }
   char buffer[100];
   (void) sprintf(buffer, "%lu glewInit   == %p\n", gettid(),   real_glewInit);
   (void) write(1, buffer, strlen(buffer));
   return real_glewInit();
}

void  glFinish(void)
{

   if (!real_glFinish)
   {
       real_glFinish = dlsym(RTLD_NEXT, "glFinish");
   }


   char buffer[100];
   (void) sprintf(buffer, "%lu real_glFinish   == %p\n", gettid(),   real_glFinish);
   (void) write(1, buffer, strlen(buffer));
   real_glFinish();
}

void  glFlush(void)
{

   if (!real_glFlush)
   {
       real_glFlush = dlsym(RTLD_NEXT, "glFlush");
   }


   char buffer[100];
   (void) sprintf(buffer, "%lu glFinish   == %p\n", gettid(),   real_glFlush);
   (void) write(1, buffer, strlen(buffer));
   real_glFlush();
}
static inline int gl_error(const char *func, const char *str)
{
	GLenum error = glGetError();
	if (error != 0) {
        char buffer[100];
        (void) sprintf(buffer, "%s: %s: %lu\n", func, str, error);
    (void) write(1, buffer, strlen(buffer));
		 
		return 1;
	}

	return 0;
}
static void   (*hook_glDrawPixels)(
         GLsizei width,
         GLsizei height,
         GLenum  format,
         GLenum  type,
   const GLvoid  *pixels
) = NULL;


void   glXDrawPixels(
         GLsizei width,
         GLsizei height,
         GLenum  format,
         GLenum  type,
   const GLvoid  *pixels
)
{
    if (!hook_glDrawPixels)
   {
       hook_glDrawPixels = dlsym(RTLD_NEXT, "glXDrawPixels");
   }


    char buffer[100];
   (void) sprintf(buffer, "%lu [hook_glDrawPixels   == %p][width = %u][height = %u]\n", gettid(),   hook_glDrawPixels, width, height);
   (void) write(1, buffer, strlen(buffer));
    hook_glDrawPixels(width, height, format, type, pixels);
}

// static EGLSurface (*hook_eglCreateWindowSurface)(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list) = NULL;
// EGLSurface  eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
// {

// }


static int init = 0;

///////////
GLuint fbo;
////////////////
GLuint pbos;
GLuint textures;


static inline int gl_init_fbo(void)
{
	glGenFramebuffers(1, &fbo);
	return !gl_error("gl_init_fbo", "failed to initialize FBO");
}

// static GLXWindow ( * hook_glXCreateWindow) (Display *dpy, GLXFBConfig config, Window win, const int *attrib_list) = NULL;

// GLXWindow glXCreateWindow(Display *dpy, GLXFBConfig config, Window win, const int *attrib_list)
// {
//     if (!hook_glXCreateWindow)
//     {
//         hook_glXCreateWindow = dlsym(RTLD_NEXT, "glXCreateWindow");
//     }

//     char buffer[1024] = {0};
    
// (void)sprintf(buffer, "[hook_glXCreateWindow]  ======================>\n" );
//     (void)write(1, buffer, strlen(buffer));
//     for (int i = 0; i < sizeof(attrib_list)/ sizeof(int*); ++i )
//     {
//     (void)sprintf(buffer, "[hook_glXCreateWindow] [i=%u][= %u]======================>\n", i , attrib_list[i]);
//     (void)write(1, buffer, strlen(buffer));
//     }

// }



GLuint texture;

    int width = 980;
    int height = 1900;
static void gl_copy_backbuffer(GLuint dst)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	 gl_error("gl_copy_backbuffer", "failed to bind FBO");

	glBindTexture(GL_TEXTURE_2D, dst);
	 gl_error("gl_copy_backbuffer", "failed to bind texture");

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			       GL_TEXTURE_2D, dst, 0);
	gl_error("gl_copy_backbuffer", "failed to set frame buffer");

	glReadBuffer(GL_BACK);

 
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	gl_error("gl_copy_backbuffer", "failed to set draw buffer");

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	gl_error("gl_copy_backbuffer", "failed to blit");
}
//EGLBoolean glXSwapBuffers(EGLDisplay d,EGLSurface e)


// 定义纹理id和帧缓冲id
GLuint new_texture, new_framebuffer;
void glXSwapBuffers(Display *d, GLXDrawable e)
{
    if (!real_eglSwapBuffers)
   {
       real_eglSwapBuffers = dlsym(RTLD_NEXT, "glXSwapBuffers");
   }


    if (!out_gl_ptr)
    {
        out_gl_ptr = fopen("./gl_out.rgba", "wb+");
    }
 

//   {
//     unsigned int width = -1, height = -1;
//   eglQuerySurface(d, e, EGL_WIDTH, &width);
//   eglQuerySurface(d, e, EGL_HEIGHT, &height);
//       char buffer[1024];
//    (void) sprintf(buffer, "%lu [real_eglSwapBuffers    == %p][width = %u][height = %u]\n", gettid(),   real_eglSwapInterval, width, height);
//    (void) write(1, buffer, strlen(buffer));
//   }



    int size = width * height * 4;
    // if (0 == pbos)
    // {
    //     pbos = 1;
    //     GLint last_pbo;
	//     GLint last_tex;
    //     glGenBuffers(NUM_BUFFERS, pbos);
    //     if (gl_error("gl_shmem_init_buffers", "failed to generate buffers")) {
    //         return false;
    //     }

    //     glGenTextures(NUM_BUFFERS,  textures);
    //     if (gl_error("gl_shmem_init_buffers", "failed to generate textures")) {
    //         return false;
    //     }

    //     glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &last_pbo);
    //     if (gl_error("gl_shmem_init_buffers",
    //             "failed to save pixel pack buffer")) {
    //         return false;
    //     }

    //     glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_tex);
    //     if (gl_error("gl_shmem_init_buffers", "failed to save texture")) {
    //         return false;
    //     }
    //     ///////////////////
    //     {

    //         glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos );
    //         if (gl_error("gl_shmem_init_data", "failed to bind pbo")) {
    //             return false;
    //         }

    //         glBufferData(GL_PIXEL_PACK_BUFFER, size, 0, GL_STREAM_READ);
    //         if (gl_error("gl_shmem_init_data", "failed to set pbo data")) {
    //             return false;
    //         }

    //         glBindTexture(GL_TEXTURE_2D, textures);
    //         if (gl_error("gl_shmem_init_data", "failed to set bind texture")) {
    //             return false;
    //         }

    //         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    //         if (gl_error("gl_shmem_init_data", "failed to set texture data"))
    //          {
    //             return false;
    //         }
    //     }

    //     ///////////
    //     glBindBuffer(GL_PIXEL_PACK_BUFFER, last_pbo);
	//     glBindTexture(GL_TEXTURE_2D, last_tex);
    // }


//     static int shtex_init = 0;
//     if (0 == shtex_init)
//     {
//         shtex_init = 1;
//         glGenTextures(1, & texture);
// 	    gl_error("gl_shtex_init_gl_tex", "failed to generate texture");
		 
//         gl_init_fbo();

//     }

//     {
//         GLint last_fbo;
// 	    GLint last_tex;
//         GLint last_pbo;
//         glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fbo);
//          gl_error("gl_shtex_capture", "failed to get last fbo");

//         glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_tex);
//          gl_error("gl_shtex_capture", "failed to get last texture") ;

//         glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &last_pbo);
// 	    gl_error("gl_shmem_capture", "failed to get last pbo");
//         // copy backbuffer
// gl_copy_backbuffer(texture);
//          glBindTexture(GL_TEXTURE_2D, last_tex);
//           glBindBuffer(GL_PIXEL_PACK_BUFFER, last_pbo);
// 	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, last_fbo);
  
//     }
  
    if (!rgba_ptr)
    {
        rgba_ptr = malloc(sizeof(unsigned char ) * width * height * 4);
    }
    memset(rgba_ptr, 0, width * height * 4);

    // {


    //     int next_tex;
    //     GLint last_fbo;
    //     GLint last_tex;
    //     GLint last_pbo;

    //     glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &last_fbo);
    //     if (gl_error("gl_shmem_capture", "failed to get last fbo")) {
    //         return;
    //     }

    //     glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_tex);
    //     if (gl_error("gl_shmem_capture", "failed to get last texture")) {
    //         return;
    //     }

    //     glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &last_pbo);
    //     if (gl_error("gl_shmem_capture", "failed to get last pbo")) {
    //         return;
    //     }
    //     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos);
	// 	if (gl_error("gl_shmem_capture_queue_copy",
	// 		     "failed to bind pbo")) {
	// 		return;
	// 	}
    //     GLvoid *buffer = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	// 	if (buffer) {
			 
	// 		shmem_copy_data(i, buffer);
	// 	}
    //     ////////////////////////////////////////////////////////////////////

            static int init_f = 0;
            if (0 == init_f)
            {
                init_f = 1;
                // 初始化纹理
                glGenTextures(1, &new_texture);
                glBindTexture(GL_TEXTURE_RECTANGLE, new_texture);
                glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

                // 初始化帧缓冲
                glGenFramebuffers(1, &new_framebuffer);
            }

            glReadBuffer(GL_BACK);
            gl_error("glReadBuffer", "glReadBuffer failed !!!");
            // glGenTextures(1, &_textureInfo.texture);
            // glGenTextures(1, & texture);
            //  glBindTexture(GL_TEXTURE_2D, texture);
            glBindFramebuffer(GL_FRAMEBUFFER, new_framebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, new_texture, 0);

            // 渲染到帧缓冲
            // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            //             gl_error("glBindFramebuffer", "glBindFramebuffer failed !!!");
            //             // 渲染代码省略

            // // 将帧缓冲的数据拷贝到纹理
            // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            gl_error("glBindFramebuffer", "glBindFramebuffer GL_TEXTURE_2D failed !!!");
            glBindTexture(GL_TEXTURE_RECTANGLE, new_texture);
            gl_error("glBindTexture", "glBindTexture GL_TEXTURE_2D failed !!!");
            glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, 0, 0, width, height);
          //glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, rgba_ptr);
//            glCopyTexSubImage2D();
           // glCopyTexSubImage2D(GL_TEXTURE_2D, GL_UNSIGNED_BYTE, width, height, 0, 0, width, height);
        //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
           gl_error("glCopyTexSubImage2D", "glCopyTexSubImage2D failed !!!");
            // glBindTexture(GL_TEXTURE_2D, 0);
            gl_error("glGenTextures", "glGenTextures GL_TEXTURE_2D failed !!!");
        //if (out_gl_ptr)
        {
          //  fwrite(rgba_ptr, size, 1, out_gl_ptr);
           // fflush(out_gl_ptr);
        }
    // }

   
   

   struct timeval tTimeVal;
    gettimeofday(&tTimeVal, NULL);
    struct tm *tTM = localtime(&tTimeVal);
    char buffer[1024] = {0};
    (void)sprintf(buffer, "[real_eglSwapBuffers][%s][%d] ======================>[%04d-%02d-%02d %02d:%02d:%02d.%03ld.%03ld]\n",  __FUNCTION__, __LINE__, tTM->tm_year +1900, tTM->tm_mon +1, tTM->tm_mday, tTM->tm_hour, tTM->tm_min, tTM->tm_sec, tTimeVal.tv_usec/ 1000, tTimeVal.tv_usec %1000);
    (void)write(1, buffer, strlen(buffer));
     real_eglSwapBuffers(d, e);
//        {

//      int width;
//     int height;
//     width = glutGet(GLUT_WINDOW_WIDTH);
//     height = glutGet(GLUT_WINDOW_HEIGHT);
//     // eglQuerSyurface(d,e,EGL_WIDTH,&width);
//     // eglQuerySurface(d,e,EGL_HEIGHT,&height);
//     char buffer[100];
//    (void) sprintf(buffer, "%lu [real_eglSwapBuffers    == %p][width = %u][height = %u]\n", gettid(),   real_eglSwapBuffers, width, height);
//    (void) write(1, buffer, strlen(buffer));
//    }
}

EGLBoolean   eglSwapInterval(EGLDisplay d,EGLint i)
{
  if (!real_eglSwapInterval)
   {
       real_eglSwapInterval = dlsym(RTLD_NEXT, "eglSwapInterval");
   }
   char buffer[100];
   (void) sprintf(buffer, "%lu real_eglSwapInterval   == %p\n", gettid(),   real_eglSwapInterval);
   (void) write(1, buffer, strlen(buffer));
   return real_eglSwapInterval(d, i);
}

// void glutSwapBuffers(void)
// {
//      if (!real_glutSwapBuffers)
//    {
//        real_glutSwapBuffers = dlsym(RTLD_NEXT, "glutSwapBuffers");
//    }


//     struct timeval tTimeVal;
//     gettimeofday(&tTimeVal, NULL);
//     struct tm *tTM = localtime(&tTimeVal);
//     char buffer[1024] = {0};
//     (void)sprintf(buffer, "[real_glutSwapBuffers] ======================>[%04d-%02d-%02d %02d:%02d:%02d.%03ld.%03ld]\n",  tTM->tm_year +1900, tTM->tm_mon +1, tTM->tm_mday, tTM->tm_hour, tTM->tm_min, tTM->tm_sec, tTimeVal.tv_usec/ 1000, tTimeVal.tv_usec %1000);
//     (void)write(1, buffer, strlen(buffer));
//      real_glutSwapBuffers();
// }


// EGLBoolean glXMakeCurrent(EGLDisplay d,EGLSurface eg1 ,EGLSurface eg2,EGLContext egc)
// {
//  if (!real_eglMakeCurrent)
//    {
//        real_eglMakeCurrent = dlsym(RTLD_NEXT, "glXMakeCurrent");
//    }


//    char buffer[100];
//    (void) sprintf(buffer, "%lu real_eglMakeCurrent   == %p\n", gettid(),   real_eglMakeCurrent);
//    (void) write(1, buffer, strlen(buffer));
//   return real_eglMakeCurrent(d, eg1, eg2, egc);
// }
// typedef void	(*t_glFlush)(void);


// // for hooking
// t_glFlush		old_glFlush = NULL;

// __attribute__((constructor))
// static void gl_hook_loaded(void)
// {

//     void *handle = NULL;
// 	char *ptr, soname[2048];
// 	if((ptr = getenv("LIBVIDEO")) == NULL) 
//     {
// 		strncpy(soname, "libGL.so.1", sizeof(soname));
// 	} else {
// 		strncpy(soname, ptr, sizeof(soname));
// 	}
//     handle = dlopen(soname, RTLD_LAZY);
// 	if( handle == NULL) 
//     {
// 		//ga_error("dlopen '%s' failed: %s\n", soname, strerror(errno));
// 		//exit(-1);
//          char buffer[100];
//         (void) sprintf(buffer, "dlopen '%s' failed: \n", soname );
//         (void) write(1, buffer, strlen(buffer));
// 	}
//     else 
//     {
//          char buffer[100];
//         (void) sprintf(buffer, "dlopen '%s' open:  \n", soname );
//         (void) write(1, buffer, strlen(buffer));
//         old_glFlush = (t_glFlush) dlsym(handle, "glFlush");

//     }

//     //if (!old_glFlush)
//     {
//         char buffer[100];
//         (void) sprintf(buffer, "%lu -------gl_hook_loaded   == %p\n", gettid(),   old_glFlush);
//         (void) write(1, buffer, strlen(buffer));
//     }

// 	// for hooking
// 	// old_glFlush = (t_glFlush) dlsym(handle, "glFlush");
// 	// ga_error("hook-gl: hooked.\n");
// 	// // indirect hook
// 	// if((ptr = getenv("HOOKVIDEO")) == NULL)
// 	// 	goto quit;
// 	// strncpy(soname, ptr, sizeof(soname));
// 	// if((handle = dlopen(soname, RTLD_LAZY)) != NULL) {
// 	// 	hook_lib_generic(soname, handle, "glFlush", (void*) hook_glFlush);
// 	// }
// 	// ga_error("hook-gl: hooked into %s.\n", soname);
// }

//
//static void  (*real_glGenTextures)( GLsizei n, GLuint * textures) = NULL;
//
//static void  * (*real_malloc)(size_t size) = NULL;
//
//
//
//static void (*real_free)(void *ptr) = NULL;
//
//
//void     glGenTextures( GLsizei in,GLuint * textures)
//{
//    if (!real_glGenTextures)
//    {
//        real_glGenTextures = dlsym(RTLD_NEXT, "glGenTextures");
//    }
//
//    /*
//     * GLAPI void GLAPIENTRY glFinish( void );
//
//GLAPI void GLAPIENTRY glFlush( void );
//     //GLAPI void GLAPIENTRY glGenTextures( GLsizei n, GLuint *textures );
//
//
//GLAPI void GLAPIENTRY glFlush( void );
//     */
////    glGenTextures
//     char buffer[100];
//      (void) sprintf(buffer, "%lu glGenTextures (%u) == %p\n", gettid(), (unsigned int)in, real_glGenTextures);
//      (void) write(1, buffer, strlen(buffer));
//    real_glGenTextures(in, textures);
//}
//
//
//void * malloc(size_t size) {
//    // use dlsym to find next malloc in dynamic libraries, ie. malloc in std library
//    if (!real_malloc)
//    {
//        real_malloc = dlsym(RTLD_NEXT, "malloc");
//    }
//
//    void * p = real_malloc(size);
//
//    // 在glibc2.23版本中不能直接使用printf，因为printf会调用malloc，形成递归调用
//    // 在glibc 2.17中测试是可行的。
//    //printf ("%lu malloc (%u) == 0x%08x\n", gettid(), (unsigned int) size, p);
//
//    // 使用write是为了解决递归调用的问题
//   // char buffer[100];
//  //  (void) sprintf(buffer, "%lu malloc (%u) == %p\n", gettid(), (unsigned int)size, p);
//  //  (void) write(1, buffer, strlen(buffer));
//    return p;
//}
//
//
//void free(void *ptr)
//{
//    if (!real_free)
//    {
//        real_free = dlsym(RTLD_NEXT, "free");
//    }
//
//
//        real_free(ptr);
//
//
//
//    // 输出结果，使用write防止递归调用
////    char buffer[100];
////    (void) sprintf(buffer, "%lu free == %p\n", gettid(), ptr);
////    (void) write(1, buffer, strlen(buffer));
//}
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////OPENGL DMOE///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  X11 /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//static uint8_t * (*real_xcb_get_image_data) (const xcb_get_image_reply_t *R) = NULL;
//
//
//uint8_t *xcb_get_image_data (const xcb_get_image_reply_t *R)
//{
//    if (!real_xcb_get_image_data)
//    {
//        real_xcb_get_image_data = dlsym(RTLD_NEXT, "xcb_get_image_data");
//    }
//    char buffer[100];
//    (void) sprintf(buffer, "%lu xcb_get_image_data   == %p\n", gettid(),   real_xcb_get_image_data);
//    (void) write(1, buffer, strlen(buffer));
//    return real_xcb_get_image_data(R);
//}


//
//static int * (*real_open) (const char *__file, int __oflag, ...) = NULL;
//
//int open(const char *__file, int __oflag, ...)
//{
//    if (!real_open)
//    {
//        real_open = dlsym(RTLD_NEXT, "open");
//    }
//
//    int ret_file = real_open(__file, __oflag);
////    if (out_log_file_ptr)
////    {
////        fprintf(out_log_file_ptr, "[pid = %lu][file = %s] [real_open   == %p][ret_file = %u]\n", gettid(), __file,  real_open, ret_file);
////        fflush(out_log_file_ptr);
////    }
//
//    char buffer[10240];
//    (void) sprintf(buffer, "[pid = %lu][file = %s] [real_open   == %p][ret_file = %u]\n", gettid(), __file,  real_open, ret_file);
//    (void) write(1, buffer, strlen(buffer));
//    return ret_file;
//    //open()ls
//    return 0;
//}
//
//
//static uint8_t * (*real_read) (int __fd, void *__buf, size_t __nbytes) = NULL;
//
//
//
////ssize_t read (int __fd, void *__buf, size_t __nbytes)
//ssize_t read (int __fd, void *__buf, size_t __nbytes)
//{
//    if (!real_read)
//    {
//        real_read = dlsym(RTLD_NEXT, "read");
//    }
////    char buffer[10240];
////    (void) sprintf(buffer, "[pid = %lu][__fd = %u] [real_read == %p] \n", gettid(), __fd,  real_read );
////    (void) write(1, buffer, strlen(buffer));
////    close()
//    return real_read(__fd, __buf, __nbytes);
////    return 0;
//}
//
//
//
//static ssize_t *  (*real_write) (int __fd, const void *__buf, size_t __n) = NULL;
//
//ssize_t write (int __fd, const void *__buf, size_t __n)
//{
//    if (!real_write)
//    {
//        real_write = dlsym(RTLD_NEXT, "write");
//    }
//    char buffer[1024];
//    (void) sprintf(buffer, "[pid = %lu][__fd = %u] [real_write == %p] \n", gettid(), __fd,  real_write );
//    (void) write(1, buffer, strlen(buffer));
////    close()
//
//
////    fread()
//    return real_write(__fd , __buf, __n);
//}
//
//
//static   int *(*real_close) (int __fd) = NULL;
//
//int close(int __fd)
//{
//    if (!real_close)
//    {
//        real_close = dlsym(RTLD_NEXT, "close");
//    }
//    char buffer[1024];
//    (void) sprintf(buffer, "[pid = %lu][__fd = %u] [real_close == %p] \n", gettid(), __fd,  real_close );
//    (void) write(1, buffer, strlen(buffer));
////    close()
//
//
////    fread()
//    return real_close(__fd );
//}
//
//
//
//static size_t* (*real_fwrite) (const void *__restrict __ptr, size_t __size,
//                      size_t __n, FILE *__restrict __s) = NULL;
//
//
//size_t fwrite (const void *__restrict __ptr, size_t __size,
//               size_t __n, FILE *__restrict __s)
//{
//    if (!real_fwrite)
//    {
//        real_fwrite = dlsym(RTLD_NEXT, "fwrite");
//    }
//    char buffer[1024];
//    (void) sprintf(buffer, "[pid = %lu][__fd = %u] [real_fwrite == %p] \n", gettid(), __fd,  real_fwrite );
//    (void) write(1, buffer, strlen(buffer));
////    close()
//
//    return real_fwrite(__ptr , __size, __n, __s);
//}
//
//static size_t * (*real_fread) (void *__restrict __ptr, size_t __size,
//                                size_t __n, FILE *__restrict __stream) = NULL;
//
//
//size_t fread (void *__restrict __ptr, size_t __size,
//              size_t __n, FILE *__restrict __stream)
//{
//    if (!real_fread)
//    {
//        real_fread = dlsym(RTLD_NEXT, "fread");
//    }
////    char buffer[1024];
////    (void) sprintf(buffer, "[pid = %lu][__stream = %u] [real_fread == %p] \n", gettid(), __stream,  real_read );
////    (void) write(1, buffer, strlen(buffer));
////    close()
////    fwrite()
////    fclose()
//    return real_fread(__ptr, __size, __n, __stream);
//}
//
//
//static int *(*real_fclose) (FILE *__stream) = NULL;
//int fclose (FILE *__stream)
//{
//    if (!real_fclose)
//    {
//        real_fclose = dlsym(RTLD_NEXT, "fclose");
//    }
////    char buffer[1024];
////    (void) sprintf(buffer, "[pid = %lu][__stream = %u] [real_fclose == %p] \n", gettid(), __stream,  real_fclose );
////    (void) write(1, buffer, strlen(buffer));
////    close()
//
////    write()
////    fread()
//    return real_fclose(__stream );
//}
//static int  *(*real_SDL_PollEvent)(SDL_Event * event) = NULL;
//
//int SDL_PollEvent(SDL_Event *event)
//{
//
////    static int (*func)(SDL_Event *event);
//    if (!real_SDL_PollEvent)
//    {
//        real_SDL_PollEvent = dlsym(RTLD_NEXT, "SDL_PollEvent");
//    }
//    char buffer[1024];
//    (void) sprintf(buffer, "[pid = %lu]  [real_SDL_PollEvent == %p] \n", gettid(),  real_SDL_PollEvent );
//    (void) write(1, buffer, strlen(buffer));
////    SDL_PollEvent
//
//
//    return real_SDL_PollEvent(event);
//
////    if (result) {
////        handle_key_event(event);
////    }
////    return (result);
//}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef  XFontStruct* (*Hook_XLoadQueryFont)( Display*		 display  , _Xconst char*	  name   );
static Hook_XLoadQueryFont real_XLoadQueryFont = NULL;

XFontStruct*  XLoadQueryFont(Display* display, _Xconst char* name)
{
    if (!real_XLoadQueryFont)
    {
        real_XLoadQueryFont = dlsym(RTLD_NEXT, "XLoadQueryFont");
    }
    //    XGetEventData
    XFontStruct * size = real_XLoadQueryFont(display, name);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XLoadQueryFont][display = %p][name = %s][g_count = %lu] \n", display, name, g_count++);
    (void)write(1, buffer, strlen(buffer));

    //    XPeekEvent
    return size;
}
 
typedef  XFontStruct* (*Hook_XQueryFont)( Display*		display , XID			 font_ID  );
static Hook_XQueryFont  real_XQueryFont = NULL;

XFontStruct*  XQueryFont(Display* display, XID			 font_ID)
{
    if (!real_XQueryFont)
    {
        real_XQueryFont = dlsym(RTLD_NEXT, "XQueryFont");
    }
    //    XGetEventData
    XFontStruct* size = real_XQueryFont(display, font_ID);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XQueryFont][display = %p][XID = %u][g_count = %lu] \n", display, font_ID, g_count++);
    (void)write(1, buffer, strlen(buffer));

    //    XPeekEvent
    return size;
}


typedef  XTimeCoord* (*Hook_XGetMotionEvents)(  Display*  display  , Window	  w  , Time	  start  , Time  stop  , int*		 nevents_return  );
static Hook_XGetMotionEvents  real_XGetMotionEvents = NULL;


XTimeCoord*  XGetMotionEvents(Display* display, Window	  w, Time	  start, Time  stop, int* nevents_return)
{
    if (!real_XGetMotionEvents)
    {
        real_XGetMotionEvents = dlsym(RTLD_NEXT, "XGetMotionEvents");
    }
    //    XGetEventData
    XTimeCoord* size = real_XGetMotionEvents(display, w, start, stop, nevents_return);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XGetMotionEvents][display = %p] [g_count = %lu] \n", display,   g_count++);
    (void)write(1, buffer, strlen(buffer));

    //    XPeekEvent
    return size;
}

typedef  XModifierKeymap* (*Hook_XDeleteModifiermapEntry)( XModifierKeymap*  modmap  , KeyCode  keycode_entry , int  modifier );
static Hook_XDeleteModifiermapEntry real_XDeleteModifiermapEntry = NULL;

XModifierKeymap* XDeleteModifiermapEntry(XModifierKeymap* modmap, KeyCode  keycode_entry, int  modifier)
{
    if (!real_XDeleteModifiermapEntry)
    {
        real_XDeleteModifiermapEntry = dlsym(RTLD_NEXT, "XDeleteModifiermapEntry");
    }
    //    XGetEventData
    XModifierKeymap* size = real_XDeleteModifiermapEntry(modmap, keycode_entry, modifier);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XDeleteModifiermapEntry] [g_count = %lu] \n",   g_count++);
    (void)write(1, buffer, strlen(buffer));

    //    XPeekEvent
    return size;
}

typedef  XModifierKeymap* (*Hook_XGetModifierMapping)( Display*	   display  );
static Hook_XGetModifierMapping real_XGetModifierMapping = NULL;
XModifierKeymap* XGetModifierMapping(Display* display)
{
    if (!real_XGetModifierMapping)
    {
        real_XGetModifierMapping = dlsym(RTLD_NEXT, "XGetModifierMapping");
    }
     
    XModifierKeymap* size = real_XGetModifierMapping(display);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XGetModifierMapping] [g_count = %lu] \n", g_count++);
    (void)write(1, buffer, strlen(buffer));
     
    return size;
}


typedef  XModifierKeymap* (*Hook_XInsertModifiermapEntry)( XModifierKeymap*	modmap  ,  KeyCode		  keycode_entry  , int			  modifier  );
static Hook_XInsertModifiermapEntry real_XInsertModifiermapEntry = NULL;

XModifierKeymap* XInsertModifiermapEntry(XModifierKeymap* modmap, KeyCode keycode_entry, int			  modifier)
{
    if (!real_XInsertModifiermapEntry)
    {
        real_XInsertModifiermapEntry = dlsym(RTLD_NEXT, "XInsertModifiermapEntry");
    }

    XModifierKeymap* size = real_XInsertModifiermapEntry(modmap, keycode_entry, modifier);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XInsertModifiermapEntry] [g_count = %lu] \n", g_count++);
    (void)write(1, buffer, strlen(buffer));

    return size;
}

typedef  XModifierKeymap* (*Hook_XNewModifiermap)( int	  max_keys_per_mod  );
static Hook_XNewModifiermap real_XNewModifiermap = NULL;
XModifierKeymap* XNewModifiermap(int	  max_keys_per_mod)
{
    if (!real_XNewModifiermap)
    {
        real_XNewModifiermap = dlsym(RTLD_NEXT, "XNewModifiermap");
    }

    XModifierKeymap* size = real_XNewModifiermap(max_keys_per_mod);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XNewModifiermap] [g_count = %lu] \n", g_count++);
    (void)write(1, buffer, strlen(buffer));

    return size;
}

typedef  XImage* (*Hook_XCreateImage)( Display* display  , Visual*	 visual  , unsigned int  depth  , int format , int offset  , 
    char* data  , unsigned int width  ,  unsigned int height  ,  int bitmap_pad  , int bytes_per_line  );
static Hook_XCreateImage real_XCreateImage = NULL;
XImage* XCreateImage(Display* display, Visual* visual, unsigned int  depth, int format, int offset,
    char* data, unsigned int width, unsigned int height, int bitmap_pad, int bytes_per_line)
{
    if (!real_XCreateImage)
    {
        real_XCreateImage = dlsym(RTLD_NEXT, "XCreateImage");
    }

    XImage* size = real_XCreateImage(display, visual, depth, format, offset, data, width, height, bitmap_pad, bytes_per_line);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XCreateImage] [g_count = %lu] \n", g_count++);
    (void)write(1, buffer, strlen(buffer));

    return size;
}
typedef  Status (*Hook_XInitImage)(XImage* image   );
static Hook_XInitImage real_XInitImage = NULL;



Status XInitImage(XImage* image)
{
    if (!real_XInitImage)
    {
        real_XInitImage = dlsym(RTLD_NEXT, "XInitImage");
    }

    Status size = real_XInitImage(image);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XInitImage] [g_count = %lu] \n", g_count++);
    (void)write(1, buffer, strlen(buffer));

    return size;
}

typedef  XImage* (*Hook_XGetImage)( Display* display , Drawable  d  , int x , int  y ,
    unsigned int width , unsigned int height , unsigned long plane_mask ,  int format );
static Hook_XGetImage real_XGetImage = NULL;

XImage* XGetImage(Display* display, Drawable  d, int x, int  y,
    unsigned int width, unsigned int height, unsigned long plane_mask, int format)
{
    if (!real_XGetImage)
    {
        real_XGetImage = dlsym(RTLD_NEXT, "XGetImage");
    }

    XImage* size = real_XGetImage(display, d, x, y, width, height, plane_mask, format);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XGetImage] [g_count = %lu] \n", g_count++);
    (void)write(1, buffer, strlen(buffer));

    return size;
}


typedef XImage* (*Hook_XGetSubImage)( Display* display , Drawable d , int x , int y ,unsigned int width, unsigned int height,
    unsigned long plane_mask ,  int format ,  XImage*  dest_image  , int  dest_x  ,int  dest_y  );
static Hook_XGetSubImage real_XGetSubImage = NULL;

XImage* XGetSubImage(Display* display, Drawable d, int x, int y, unsigned int width, unsigned int height,
    unsigned long plane_mask, int format, XImage* dest_image, int  dest_x, int  dest_y)
{
    if (!real_XGetSubImage)
    {
        real_XGetSubImage = dlsym(RTLD_NEXT, "XGetSubImage");
    }

    XImage* size = real_XGetSubImage(display, d, x, y, width, height, plane_mask, format, dest_image, dest_x, dest_y);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XGetSubImage] [g_count = %lu] \n", g_count++);
    (void)write(1, buffer, strlen(buffer));

    return size;
}

/*
 * X function declarations.
 */
//extern Display* XOpenDisplay(
//    _Xconst char*	/* display_name */
//);
//
//extern void XrmInitialize(
//    void
//);
//
//extern char* XFetchBytes(
//    Display*		/* display */,
//    int*		/* nbytes_return */
//);
//extern char* XFetchBuffer(
//    Display*		/* display */,
//    int*		/* nbytes_return */,
//    int			/* buffer */
//);
//extern char* XGetAtomName(
//    Display*		/* display */,
//    Atom		/* atom */
//);
//extern Status XGetAtomNames(
//    Display*		/* dpy */,
//    Atom*		/* atoms */,
//    int			/* count */,
//    char**		/* names_return */
//);
//extern char* XGetDefault(
//    Display*		/* display */,
//    _Xconst char*	/* program */,
//    _Xconst char*	/* option */
//);
//extern char* XDisplayName(
//    _Xconst char*	/* string */
//);
//extern char* XKeysymToString(
//    KeySym		/* keysym */
//);
//
//extern int (*XSynchronize(
//    Display*		/* display */,
//    Bool		/* onoff */
//))(
//    Display*		/* display */
//    );
//extern int (*XSetAfterFunction(
//    Display*		/* display */,
//    int (*) (
//        Display*	/* display */
//        )		/* procedure */
//))(
//    Display*		/* display */
//    );
//extern Atom XInternAtom(
//    Display*		/* display */,
//    _Xconst char*	/* atom_name */,
//    Bool		/* only_if_exists */
//);
//extern Status XInternAtoms(
//    Display*		/* dpy */,
//    char**		/* names */,
//    int			/* count */,
//    Bool		/* onlyIfExists */,
//    Atom*		/* atoms_return */
//);
//extern Colormap XCopyColormapAndFree(
//    Display*		/* display */,
//    Colormap		/* colormap */
//);
//extern Colormap XCreateColormap(
//    Display*		/* display */,
//    Window		/* w */,
//    Visual*		/* visual */,
//    int			/* alloc */
//);
//extern Cursor XCreatePixmapCursor(
//    Display*		/* display */,
//    Pixmap		/* source */,
//    Pixmap		/* mask */,
//    XColor*		/* foreground_color */,
//    XColor*		/* background_color */,
//    unsigned int	/* x */,
//    unsigned int	/* y */
//);
//extern Cursor XCreateGlyphCursor(
//    Display*		/* display */,
//    Font		/* source_font */,
//    Font		/* mask_font */,
//    unsigned int	/* source_char */,
//    unsigned int	/* mask_char */,
//    XColor _Xconst*	/* foreground_color */,
//    XColor _Xconst*	/* background_color */
//);
//extern Cursor XCreateFontCursor(
//    Display*		/* display */,
//    unsigned int	/* shape */
//);
//extern Font XLoadFont(
//    Display*		/* display */,
//    _Xconst char*	/* name */
//);
//extern GC XCreateGC(
//    Display*		/* display */,
//    Drawable		/* d */,
//    unsigned long	/* valuemask */,
//    XGCValues*		/* values */
//);
//extern GContext XGContextFromGC(
//    GC			/* gc */
//);
//extern void XFlushGC(
//    Display*		/* display */,
//    GC			/* gc */
//);
//extern Pixmap XCreatePixmap(
//    Display*		/* display */,
//    Drawable		/* d */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    unsigned int	/* depth */
//);
//extern Pixmap XCreateBitmapFromData(
//    Display*		/* display */,
//    Drawable		/* d */,
//    _Xconst char*	/* data */,
//    unsigned int	/* width */,
//    unsigned int	/* height */
//);
//extern Pixmap XCreatePixmapFromBitmapData(
//    Display*		/* display */,
//    Drawable		/* d */,
//    char*		/* data */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    unsigned long	/* fg */,
//    unsigned long	/* bg */,
//    unsigned int	/* depth */
//);
//extern Window XCreateSimpleWindow(
//    Display*		/* display */,
//    Window		/* parent */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    unsigned int	/* border_width */,
//    unsigned long	/* border */,
//    unsigned long	/* background */
//);
//extern Window XGetSelectionOwner(
//    Display*		/* display */,
//    Atom		/* selection */
//);
//extern Window XCreateWindow(
//    Display*		/* display */,
//    Window		/* parent */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    unsigned int	/* border_width */,
//    int			/* depth */,
//    unsigned int	/* class */,
//    Visual*		/* visual */,
//    unsigned long	/* valuemask */,
//    XSetWindowAttributes*	/* attributes */
//);
//extern Colormap* XListInstalledColormaps(
//    Display*		/* display */,
//    Window		/* w */,
//    int*		/* num_return */
//);
//extern char** XListFonts(
//    Display*		/* display */,
//    _Xconst char*	/* pattern */,
//    int			/* maxnames */,
//    int*		/* actual_count_return */
//);
//extern char** XListFontsWithInfo(
//    Display*		/* display */,
//    _Xconst char*	/* pattern */,
//    int			/* maxnames */,
//    int*		/* count_return */,
//    XFontStruct**	/* info_return */
//);
//extern char** XGetFontPath(
//    Display*		/* display */,
//    int*		/* npaths_return */
//);
//extern char** XListExtensions(
//    Display*		/* display */,
//    int*		/* nextensions_return */
//);
//extern Atom* XListProperties(
//    Display*		/* display */,
//    Window		/* w */,
//    int*		/* num_prop_return */
//);
//extern XHostAddress* XListHosts(
//    Display*		/* display */,
//    int*		/* nhosts_return */,
//    Bool*		/* state_return */
//);
//_X_DEPRECATED
//extern KeySym XKeycodeToKeysym(
//    Display*		/* display */,
//#if NeedWidePrototypes
//    unsigned int	/* keycode */,
//#else
//    KeyCode		/* keycode */,
//#endif
//    int			/* index */
//);
//extern KeySym XLookupKeysym(
//    XKeyEvent*		/* key_event */,
//    int			/* index */
//);
//extern KeySym* XGetKeyboardMapping(
//    Display*		/* display */,
//#if NeedWidePrototypes
//    unsigned int	/* first_keycode */,
//#else
//    KeyCode		/* first_keycode */,
//#endif
//    int			/* keycode_count */,
//    int*		/* keysyms_per_keycode_return */
//);
//extern KeySym XStringToKeysym(
//    _Xconst char*	/* string */
//);
//extern long XMaxRequestSize(
//    Display*		/* display */
//);
//extern long XExtendedMaxRequestSize(
//    Display*		/* display */
//);
//extern char* XResourceManagerString(
//    Display*		/* display */
//);
//extern char* XScreenResourceString(
//    Screen*		/* screen */
//);
//extern unsigned long XDisplayMotionBufferSize(
//    Display*		/* display */
//);
//extern VisualID XVisualIDFromVisual(
//    Visual*		/* visual */
//);
//
///* multithread routines */
//
//extern Status XInitThreads(
//    void
//);
//
//extern void XLockDisplay(
//    Display*		/* display */
//);
//
//extern void XUnlockDisplay(
//    Display*		/* display */
//);
//
///* routines for dealing with extensions */
//
//extern XExtCodes* XInitExtension(
//    Display*		/* display */,
//    _Xconst char*	/* name */
//);
//
//extern XExtCodes* XAddExtension(
//    Display*		/* display */
//);
//extern XExtData* XFindOnExtensionList(
//    XExtData**		/* structure */,
//    int			/* number */
//);
//extern XExtData** XEHeadOfExtensionList(
//    XEDataObject	/* object */
//);
//
///* these are routines for which there are also macros */
//extern Window XRootWindow(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//extern Window XDefaultRootWindow(
//    Display*		/* display */
//);
//extern Window XRootWindowOfScreen(
//    Screen*		/* screen */
//);
//extern Visual* XDefaultVisual(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//extern Visual* XDefaultVisualOfScreen(
//    Screen*		/* screen */
//);
//extern GC XDefaultGC(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//extern GC XDefaultGCOfScreen(
//    Screen*		/* screen */
//);
//extern unsigned long XBlackPixel(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//extern unsigned long XWhitePixel(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//extern unsigned long XAllPlanes(
//    void
//);
//extern unsigned long XBlackPixelOfScreen(
//    Screen*		/* screen */
//);
//extern unsigned long XWhitePixelOfScreen(
//    Screen*		/* screen */
//);
//extern unsigned long XNextRequest(
//    Display*		/* display */
//);
//extern unsigned long XLastKnownRequestProcessed(
//    Display*		/* display */
//);
//extern char* XServerVendor(
//    Display*		/* display */
//);
//extern char* XDisplayString(
//    Display*		/* display */
//);
//extern Colormap XDefaultColormap(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//extern Colormap XDefaultColormapOfScreen(
//    Screen*		/* screen */
//);
//extern Display* XDisplayOfScreen(
//    Screen*		/* screen */
//);
//extern Screen* XScreenOfDisplay(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//extern Screen* XDefaultScreenOfDisplay(
//    Display*		/* display */
//);
//extern long XEventMaskOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XScreenNumberOfScreen(
//    Screen*		/* screen */
//);
//
//typedef int (*XErrorHandler) (	    /* WARNING, this type not in Xlib spec */
//    Display*		/* display */,
//    XErrorEvent*	/* error_event */
//    );
//
//extern XErrorHandler XSetErrorHandler(
//    XErrorHandler	/* handler */
//);
//
//
//typedef int (*XIOErrorHandler) (    /* WARNING, this type not in Xlib spec */
//    Display*		/* display */
//    );
//
//extern XIOErrorHandler XSetIOErrorHandler(
//    XIOErrorHandler	/* handler */
//);
//
//typedef void (*XIOErrorExitHandler) ( /* WARNING, this type not in Xlib spec */
//    Display*,		/* display */
//    void*		/* user_data */
//    );
//
//extern void XSetIOErrorExitHandler(
//    Display*,			/* display */
//    XIOErrorExitHandler,	/* handler */
//    void*			/* user_data */
//);
//
//extern XPixmapFormatValues* XListPixmapFormats(
//    Display*		/* display */,
//    int*		/* count_return */
//);
//extern int* XListDepths(
//    Display*		/* display */,
//    int			/* screen_number */,
//    int*		/* count_return */
//);
//
///* ICCCM routines for things that don't require special include files; */
///* other declarations are given in Xutil.h                             */
//extern Status XReconfigureWMWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* screen_number */,
//    unsigned int	/* mask */,
//    XWindowChanges*	/* changes */
//);
//
//extern Status XGetWMProtocols(
//    Display*		/* display */,
//    Window		/* w */,
//    Atom**		/* protocols_return */,
//    int*		/* count_return */
//);
//extern Status XSetWMProtocols(
//    Display*		/* display */,
//    Window		/* w */,
//    Atom*		/* protocols */,
//    int			/* count */
//);
//extern Status XIconifyWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* screen_number */
//);
//extern Status XWithdrawWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* screen_number */
//);
//extern Status XGetCommand(
//    Display*		/* display */,
//    Window		/* w */,
//    char***		/* argv_return */,
//    int*		/* argc_return */
//);
//extern Status XGetWMColormapWindows(
//    Display*		/* display */,
//    Window		/* w */,
//    Window**		/* windows_return */,
//    int*		/* count_return */
//);
//extern Status XSetWMColormapWindows(
//    Display*		/* display */,
//    Window		/* w */,
//    Window*		/* colormap_windows */,
//    int			/* count */
//);
//extern void XFreeStringList(
//    char**		/* list */
//);
//extern int XSetTransientForHint(
//    Display*		/* display */,
//    Window		/* w */,
//    Window		/* prop_window */
//);
//
///* The following are given in alphabetical order */
//
//extern int XActivateScreenSaver(
//    Display*		/* display */
//);
//
//extern int XAddHost(
//    Display*		/* display */,
//    XHostAddress*	/* host */
//);
//
//extern int XAddHosts(
//    Display*		/* display */,
//    XHostAddress*	/* hosts */,
//    int			/* num_hosts */
//);
//
//extern int XAddToExtensionList(
//    struct _XExtData**	/* structure */,
//    XExtData*		/* ext_data */
//);
//
//extern int XAddToSaveSet(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern Status XAllocColor(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    XColor*		/* screen_in_out */
//);
//
//extern Status XAllocColorCells(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    Bool	        /* contig */,
//    unsigned long*	/* plane_masks_return */,
//    unsigned int	/* nplanes */,
//    unsigned long*	/* pixels_return */,
//    unsigned int 	/* npixels */
//);
//
//extern Status XAllocColorPlanes(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    Bool		/* contig */,
//    unsigned long*	/* pixels_return */,
//    int			/* ncolors */,
//    int			/* nreds */,
//    int			/* ngreens */,
//    int			/* nblues */,
//    unsigned long*	/* rmask_return */,
//    unsigned long*	/* gmask_return */,
//    unsigned long*	/* bmask_return */
//);
//
//extern Status XAllocNamedColor(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    _Xconst char*	/* color_name */,
//    XColor*		/* screen_def_return */,
//    XColor*		/* exact_def_return */
//);
//
//extern int XAllowEvents(
//    Display*		/* display */,
//    int			/* event_mode */,
//    Time		/* time */
//);
//
//extern int XAutoRepeatOff(
//    Display*		/* display */
//);
//
//extern int XAutoRepeatOn(
//    Display*		/* display */
//);
//
//extern int XBell(
//    Display*		/* display */,
//    int			/* percent */
//);
//
//extern int XBitmapBitOrder(
//    Display*		/* display */
//);
//
//extern int XBitmapPad(
//    Display*		/* display */
//);
//
//extern int XBitmapUnit(
//    Display*		/* display */
//);
//
//extern int XCellsOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XChangeActivePointerGrab(
//    Display*		/* display */,
//    unsigned int	/* event_mask */,
//    Cursor		/* cursor */,
//    Time		/* time */
//);
//
//extern int XChangeGC(
//    Display*		/* display */,
//    GC			/* gc */,
//    unsigned long	/* valuemask */,
//    XGCValues*		/* values */
//);
//
//extern int XChangeKeyboardControl(
//    Display*		/* display */,
//    unsigned long	/* value_mask */,
//    XKeyboardControl*	/* values */
//);
//
//extern int XChangeKeyboardMapping(
//    Display*		/* display */,
//    int			/* first_keycode */,
//    int			/* keysyms_per_keycode */,
//    KeySym*		/* keysyms */,
//    int			/* num_codes */
//);
//
//extern int XChangePointerControl(
//    Display*		/* display */,
//    Bool		/* do_accel */,
//    Bool		/* do_threshold */,
//    int			/* accel_numerator */,
//    int			/* accel_denominator */,
//    int			/* threshold */
//);
//
//extern int XChangeProperty(
//    Display*		/* display */,
//    Window		/* w */,
//    Atom		/* property */,
//    Atom		/* type */,
//    int			/* format */,
//    int			/* mode */,
//    _Xconst unsigned char*	/* data */,
//    int			/* nelements */
//);
//
//extern int XChangeSaveSet(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* change_mode */
//);
//
//extern int XChangeWindowAttributes(
//    Display*		/* display */,
//    Window		/* w */,
//    unsigned long	/* valuemask */,
//    XSetWindowAttributes* /* attributes */
//);
//
//extern Bool XCheckIfEvent(
//    Display*		/* display */,
//    XEvent*		/* event_return */,
//    Bool(*) (
//        Display*			/* display */,
//        XEvent*			/* event */,
//        XPointer			/* arg */
//        )		/* predicate */,
//    XPointer		/* arg */
//);
//
//extern Bool XCheckMaskEvent(
//    Display*		/* display */,
//    long		/* event_mask */,
//    XEvent*		/* event_return */
//);
//
//extern Bool XCheckTypedEvent(
//    Display*		/* display */,
//    int			/* event_type */,
//    XEvent*		/* event_return */
//);
//
//extern Bool XCheckTypedWindowEvent(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* event_type */,
//    XEvent*		/* event_return */
//);
//
//extern Bool XCheckWindowEvent(
//    Display*		/* display */,
//    Window		/* w */,
//    long		/* event_mask */,
//    XEvent*		/* event_return */
//);
//
//extern int XCirculateSubwindows(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* direction */
//);
//
//extern int XCirculateSubwindowsDown(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XCirculateSubwindowsUp(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XClearArea(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    Bool		/* exposures */
//);
//
//extern int XClearWindow(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XCloseDisplay(
//    Display*		/* display */
//);
//
//extern int XConfigureWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    unsigned int	/* value_mask */,
//    XWindowChanges*	/* values */
//);
//
//extern int XConnectionNumber(
//    Display*		/* display */
//);
//
//extern int XConvertSelection(
//    Display*		/* display */,
//    Atom		/* selection */,
//    Atom 		/* target */,
//    Atom		/* property */,
//    Window		/* requestor */,
//    Time		/* time */
//);
//
//extern int XCopyArea(
//    Display*		/* display */,
//    Drawable		/* src */,
//    Drawable		/* dest */,
//    GC			/* gc */,
//    int			/* src_x */,
//    int			/* src_y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    int			/* dest_x */,
//    int			/* dest_y */
//);
//
//extern int XCopyGC(
//    Display*		/* display */,
//    GC			/* src */,
//    unsigned long	/* valuemask */,
//    GC			/* dest */
//);
//
//extern int XCopyPlane(
//    Display*		/* display */,
//    Drawable		/* src */,
//    Drawable		/* dest */,
//    GC			/* gc */,
//    int			/* src_x */,
//    int			/* src_y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    int			/* dest_x */,
//    int			/* dest_y */,
//    unsigned long	/* plane */
//);
//
//extern int XDefaultDepth(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//
//extern int XDefaultDepthOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XDefaultScreen(
//    Display*		/* display */
//);
//
//extern int XDefineCursor(
//    Display*		/* display */,
//    Window		/* w */,
//    Cursor		/* cursor */
//);
//
//extern int XDeleteProperty(
//    Display*		/* display */,
//    Window		/* w */,
//    Atom		/* property */
//);
//
//extern int XDestroyWindow(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XDestroySubwindows(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XDoesBackingStore(
//    Screen*		/* screen */
//);
//
//extern Bool XDoesSaveUnders(
//    Screen*		/* screen */
//);
//
//extern int XDisableAccessControl(
//    Display*		/* display */
//);
//
//
//extern int XDisplayCells(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//
//extern int XDisplayHeight(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//
//extern int XDisplayHeightMM(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//
//extern int XDisplayKeycodes(
//    Display*		/* display */,
//    int*		/* min_keycodes_return */,
//    int*		/* max_keycodes_return */
//);
//
//extern int XDisplayPlanes(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//
//extern int XDisplayWidth(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//
//extern int XDisplayWidthMM(
//    Display*		/* display */,
//    int			/* screen_number */
//);
//
//extern int XDrawArc(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    int			/* angle1 */,
//    int			/* angle2 */
//);
//
//extern int XDrawArcs(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XArc*		/* arcs */,
//    int			/* narcs */
//);
//
//extern int XDrawImageString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst char*	/* string */,
//    int			/* length */
//);
//
//extern int XDrawImageString16(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst XChar2b*	/* string */,
//    int			/* length */
//);
//
//extern int XDrawLine(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x1 */,
//    int			/* y1 */,
//    int			/* x2 */,
//    int			/* y2 */
//);
//
//extern int XDrawLines(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XPoint*		/* points */,
//    int			/* npoints */,
//    int			/* mode */
//);
//
//extern int XDrawPoint(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */
//);
//
//extern int XDrawPoints(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XPoint*		/* points */,
//    int			/* npoints */,
//    int			/* mode */
//);
//
//extern int XDrawRectangle(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */
//);
//
//extern int XDrawRectangles(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XRectangle*		/* rectangles */,
//    int			/* nrectangles */
//);
//
//extern int XDrawSegments(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XSegment*		/* segments */,
//    int			/* nsegments */
//);
//
//extern int XDrawString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst char*	/* string */,
//    int			/* length */
//);
//
//extern int XDrawString16(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst XChar2b*	/* string */,
//    int			/* length */
//);
//
//extern int XDrawText(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    XTextItem*		/* items */,
//    int			/* nitems */
//);
//
//extern int XDrawText16(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    XTextItem16*	/* items */,
//    int			/* nitems */
//);
//
//extern int XEnableAccessControl(
//    Display*		/* display */
//);
//
//extern int XEventsQueued(
//    Display*		/* display */,
//    int			/* mode */
//);
//
//extern Status XFetchName(
//    Display*		/* display */,
//    Window		/* w */,
//    char**		/* window_name_return */
//);
//
//extern int XFillArc(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    int			/* angle1 */,
//    int			/* angle2 */
//);
//
//extern int XFillArcs(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XArc*		/* arcs */,
//    int			/* narcs */
//);
//
//extern int XFillPolygon(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XPoint*		/* points */,
//    int			/* npoints */,
//    int			/* shape */,
//    int			/* mode */
//);
//
//extern int XFillRectangle(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */
//);
//
//extern int XFillRectangles(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XRectangle*		/* rectangles */,
//    int			/* nrectangles */
//);
//
//extern int XFlush(
//    Display*		/* display */
//);
//
//extern int XForceScreenSaver(
//    Display*		/* display */,
//    int			/* mode */
//);
//
//extern int XFree(
//    void*		/* data */
//);
//
//extern int XFreeColormap(
//    Display*		/* display */,
//    Colormap		/* colormap */
//);
//
//extern int XFreeColors(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    unsigned long*	/* pixels */,
//    int			/* npixels */,
//    unsigned long	/* planes */
//);
//
//extern int XFreeCursor(
//    Display*		/* display */,
//    Cursor		/* cursor */
//);
//
//extern int XFreeExtensionList(
//    char**		/* list */
//);
//
//extern int XFreeFont(
//    Display*		/* display */,
//    XFontStruct*	/* font_struct */
//);
//
//extern int XFreeFontInfo(
//    char**		/* names */,
//    XFontStruct*	/* free_info */,
//    int			/* actual_count */
//);
//
//extern int XFreeFontNames(
//    char**		/* list */
//);
//
//extern int XFreeFontPath(
//    char**		/* list */
//);
//
//extern int XFreeGC(
//    Display*		/* display */,
//    GC			/* gc */
//);
//
//extern int XFreeModifiermap(
//    XModifierKeymap*	/* modmap */
//);
//
//extern int XFreePixmap(
//    Display*		/* display */,
//    Pixmap		/* pixmap */
//);
//
//extern int XGeometry(
//    Display*		/* display */,
//    int			/* screen */,
//    _Xconst char*	/* position */,
//    _Xconst char*	/* default_position */,
//    unsigned int	/* bwidth */,
//    unsigned int	/* fwidth */,
//    unsigned int	/* fheight */,
//    int			/* xadder */,
//    int			/* yadder */,
//    int*		/* x_return */,
//    int*		/* y_return */,
//    int*		/* width_return */,
//    int*		/* height_return */
//);
//
//extern int XGetErrorDatabaseText(
//    Display*		/* display */,
//    _Xconst char*	/* name */,
//    _Xconst char*	/* message */,
//    _Xconst char*	/* default_string */,
//    char*		/* buffer_return */,
//    int			/* length */
//);
//
//extern int XGetErrorText(
//    Display*		/* display */,
//    int			/* code */,
//    char*		/* buffer_return */,
//    int			/* length */
//);
//
//extern Bool XGetFontProperty(
//    XFontStruct*	/* font_struct */,
//    Atom		/* atom */,
//    unsigned long*	/* value_return */
//);
//
//extern Status XGetGCValues(
//    Display*		/* display */,
//    GC			/* gc */,
//    unsigned long	/* valuemask */,
//    XGCValues*		/* values_return */
//);
//
//extern Status XGetGeometry(
//    Display*		/* display */,
//    Drawable		/* d */,
//    Window*		/* root_return */,
//    int*		/* x_return */,
//    int*		/* y_return */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */,
//    unsigned int*	/* border_width_return */,
//    unsigned int*	/* depth_return */
//);
//
//extern Status XGetIconName(
//    Display*		/* display */,
//    Window		/* w */,
//    char**		/* icon_name_return */
//);
//
//extern int XGetInputFocus(
//    Display*		/* display */,
//    Window*		/* focus_return */,
//    int*		/* revert_to_return */
//);
//
//extern int XGetKeyboardControl(
//    Display*		/* display */,
//    XKeyboardState*	/* values_return */
//);
//
//extern int XGetPointerControl(
//    Display*		/* display */,
//    int*		/* accel_numerator_return */,
//    int*		/* accel_denominator_return */,
//    int*		/* threshold_return */
//);
//
//extern int XGetPointerMapping(
//    Display*		/* display */,
//    unsigned char*	/* map_return */,
//    int			/* nmap */
//);
//
//extern int XGetScreenSaver(
//    Display*		/* display */,
//    int*		/* timeout_return */,
//    int*		/* interval_return */,
//    int*		/* prefer_blanking_return */,
//    int*		/* allow_exposures_return */
//);
//
//extern Status XGetTransientForHint(
//    Display*		/* display */,
//    Window		/* w */,
//    Window*		/* prop_window_return */
//);
//
//extern int XGetWindowProperty(
//    Display*		/* display */,
//    Window		/* w */,
//    Atom		/* property */,
//    long		/* long_offset */,
//    long		/* long_length */,
//    Bool		/* delete */,
//    Atom		/* req_type */,
//    Atom*		/* actual_type_return */,
//    int*		/* actual_format_return */,
//    unsigned long*	/* nitems_return */,
//    unsigned long*	/* bytes_after_return */,
//    unsigned char**	/* prop_return */
//);
//
//extern Status XGetWindowAttributes(
//    Display*		/* display */,
//    Window		/* w */,
//    XWindowAttributes*	/* window_attributes_return */
//);
//
//extern int XGrabButton(
//    Display*		/* display */,
//    unsigned int	/* button */,
//    unsigned int	/* modifiers */,
//    Window		/* grab_window */,
//    Bool		/* owner_events */,
//    unsigned int	/* event_mask */,
//    int			/* pointer_mode */,
//    int			/* keyboard_mode */,
//    Window		/* confine_to */,
//    Cursor		/* cursor */
//);
//
//extern int XGrabKey(
//    Display*		/* display */,
//    int			/* keycode */,
//    unsigned int	/* modifiers */,
//    Window		/* grab_window */,
//    Bool		/* owner_events */,
//    int			/* pointer_mode */,
//    int			/* keyboard_mode */
//);
//
//extern int XGrabKeyboard(
//    Display*		/* display */,
//    Window		/* grab_window */,
//    Bool		/* owner_events */,
//    int			/* pointer_mode */,
//    int			/* keyboard_mode */,
//    Time		/* time */
//);
//
//extern int XGrabPointer(
//    Display*		/* display */,
//    Window		/* grab_window */,
//    Bool		/* owner_events */,
//    unsigned int	/* event_mask */,
//    int			/* pointer_mode */,
//    int			/* keyboard_mode */,
//    Window		/* confine_to */,
//    Cursor		/* cursor */,
//    Time		/* time */
//);
//
//extern int XGrabServer(
//    Display*		/* display */
//);
//
//extern int XHeightMMOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XHeightOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XIfEvent(
//    Display*		/* display */,
//    XEvent*		/* event_return */,
//    Bool(*) (
//        Display*			/* display */,
//        XEvent*			/* event */,
//        XPointer			/* arg */
//        )		/* predicate */,
//    XPointer		/* arg */
//);
//
//extern int XImageByteOrder(
//    Display*		/* display */
//);
//
//extern int XInstallColormap(
//    Display*		/* display */,
//    Colormap		/* colormap */
//);
//
//extern KeyCode XKeysymToKeycode(
//    Display*		/* display */,
//    KeySym		/* keysym */
//);
//
//extern int XKillClient(
//    Display*		/* display */,
//    XID			/* resource */
//);
//
//extern Status XLookupColor(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    _Xconst char*	/* color_name */,
//    XColor*		/* exact_def_return */,
//    XColor*		/* screen_def_return */
//);
//
//extern int XLowerWindow(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XMapRaised(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XMapSubwindows(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XMapWindow(
//    Display*		/* display */,
//    Window		/* w */
//);
//
typedef int (*Hook_XMaskEvent)( Display*  display , long event_mask ,  XEvent* event_return  );
static Hook_XMaskEvent real_XMaskEvent = NULL;

int XMaskEvent(Display* display, long event_mask, XEvent* event_return)
{
    if (!real_XMaskEvent)
    {
        real_XMaskEvent = dlsym(RTLD_NEXT, "XMaskEvent");
    }
    //    XGetEventData
    int size = real_XMaskEvent(display, event_mask, event_return);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XMaskEvent][display = %p][event_mask = %u][g_count = %lu] \n", display, event_mask, g_count++);
    (void)write(1, buffer, strlen(buffer));

    //    XPeekEvent
    return size;
}
//
//extern int XMaxCmapsOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XMinCmapsOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XMoveResizeWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* x */,
//    int			/* y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */
//);
//
//extern int XMoveWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    int			/* x */,
//    int			/* y */
//);
//
//extern int XNextEvent(
//    Display*		/* display */,
//    XEvent*		/* event_return */
//);
//
//extern int XNoOp(
//    Display*		/* display */
//);
//
//extern Status XParseColor(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    _Xconst char*	/* spec */,
//    XColor*		/* exact_def_return */
//);
//
//extern int XParseGeometry(
//    _Xconst char*	/* parsestring */,
//    int*		/* x_return */,
//    int*		/* y_return */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */
//);
//
//extern int XPeekEvent(
//    Display*		/* display */,
//    XEvent*		/* event_return */
//);
//
//extern int XPeekIfEvent(
//    Display*		/* display */,
//    XEvent*		/* event_return */,
//    Bool(*) (
//        Display*		/* display */,
//        XEvent*		/* event */,
//        XPointer		/* arg */
//        )		/* predicate */,
//    XPointer		/* arg */
//);
//
//extern int XPending(
//    Display*		/* display */
//);
//
//extern int XPlanesOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XProtocolRevision(
//    Display*		/* display */
//);
//
//extern int XProtocolVersion(
//    Display*		/* display */
//);
//
//
//extern int XPutBackEvent(
//    Display*		/* display */,
//    XEvent*		/* event */
//);
//
//extern int XPutImage(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    XImage*		/* image */,
//    int			/* src_x */,
//    int			/* src_y */,
//    int			/* dest_x */,
//    int			/* dest_y */,
//    unsigned int	/* width */,
//    unsigned int	/* height */
//);
//
//extern int XQLength(
//    Display*		/* display */
//);
//
//extern Status XQueryBestCursor(
//    Display*		/* display */,
//    Drawable		/* d */,
//    unsigned int        /* width */,
//    unsigned int	/* height */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */
//);
//
//extern Status XQueryBestSize(
//    Display*		/* display */,
//    int			/* class */,
//    Drawable		/* which_screen */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */
//);
//
//extern Status XQueryBestStipple(
//    Display*		/* display */,
//    Drawable		/* which_screen */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */
//);
//
//extern Status XQueryBestTile(
//    Display*		/* display */,
//    Drawable		/* which_screen */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */
//);
//
//extern int XQueryColor(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    XColor*		/* def_in_out */
//);
//
//extern int XQueryColors(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    XColor*		/* defs_in_out */,
//    int			/* ncolors */
//);
//
//extern Bool XQueryExtension(
//    Display*		/* display */,
//    _Xconst char*	/* name */,
//    int*		/* major_opcode_return */,
//    int*		/* first_event_return */,
//    int*		/* first_error_return */
//);
//
//extern int XQueryKeymap(
//    Display*		/* display */,
//    char[32]		/* keys_return */
//);
//
//extern Bool XQueryPointer(
//    Display*		/* display */,
//    Window		/* w */,
//    Window*		/* root_return */,
//    Window*		/* child_return */,
//    int*		/* root_x_return */,
//    int*		/* root_y_return */,
//    int*		/* win_x_return */,
//    int*		/* win_y_return */,
//    unsigned int*       /* mask_return */
//);
//
//extern int XQueryTextExtents(
//    Display*		/* display */,
//    XID			/* font_ID */,
//    _Xconst char*	/* string */,
//    int			/* nchars */,
//    int*		/* direction_return */,
//    int*		/* font_ascent_return */,
//    int*		/* font_descent_return */,
//    XCharStruct*	/* overall_return */
//);
//
//extern int XQueryTextExtents16(
//    Display*		/* display */,
//    XID			/* font_ID */,
//    _Xconst XChar2b*	/* string */,
//    int			/* nchars */,
//    int*		/* direction_return */,
//    int*		/* font_ascent_return */,
//    int*		/* font_descent_return */,
//    XCharStruct*	/* overall_return */
//);
//
//extern Status XQueryTree(
//    Display*		/* display */,
//    Window		/* w */,
//    Window*		/* root_return */,
//    Window*		/* parent_return */,
//    Window**		/* children_return */,
//    unsigned int*	/* nchildren_return */
//);
//
//extern int XRaiseWindow(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XReadBitmapFile(
//    Display*		/* display */,
//    Drawable 		/* d */,
//    _Xconst char*	/* filename */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */,
//    Pixmap*		/* bitmap_return */,
//    int*		/* x_hot_return */,
//    int*		/* y_hot_return */
//);
//
//extern int XReadBitmapFileData(
//    _Xconst char*	/* filename */,
//    unsigned int*	/* width_return */,
//    unsigned int*	/* height_return */,
//    unsigned char**	/* data_return */,
//    int*		/* x_hot_return */,
//    int*		/* y_hot_return */
//);
//
//extern int XRebindKeysym(
//    Display*		/* display */,
//    KeySym		/* keysym */,
//    KeySym*		/* list */,
//    int			/* mod_count */,
//    _Xconst unsigned char*	/* string */,
//    int			/* bytes_string */
//);
//
//extern int XRecolorCursor(
//    Display*		/* display */,
//    Cursor		/* cursor */,
//    XColor*		/* foreground_color */,
//    XColor*		/* background_color */
//);
//
//extern int XRefreshKeyboardMapping(
//    XMappingEvent*	/* event_map */
//);
//
//extern int XRemoveFromSaveSet(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XRemoveHost(
//    Display*		/* display */,
//    XHostAddress*	/* host */
//);
//
//extern int XRemoveHosts(
//    Display*		/* display */,
//    XHostAddress*	/* hosts */,
//    int			/* num_hosts */
//);
//
//extern int XReparentWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    Window		/* parent */,
//    int			/* x */,
//    int			/* y */
//);
//
//extern int XResetScreenSaver(
//    Display*		/* display */
//);
//
//extern int XResizeWindow(
//    Display*		/* display */,
//    Window		/* w */,
//    unsigned int	/* width */,
//    unsigned int	/* height */
//);
//
//extern int XRestackWindows(
//    Display*		/* display */,
//    Window*		/* windows */,
//    int			/* nwindows */
//);
//
//extern int XRotateBuffers(
//    Display*		/* display */,
//    int			/* rotate */
//);
//
//extern int XRotateWindowProperties(
//    Display*		/* display */,
//    Window		/* w */,
//    Atom*		/* properties */,
//    int			/* num_prop */,
//    int			/* npositions */
//);
//
//extern int XScreenCount(
//    Display*		/* display */
//);
//
//extern int XSelectInput(
//    Display*		/* display */,
//    Window		/* w */,
//    long		/* event_mask */
//);
//
typedef  Status (*Hook_XSendEvent)( Display* display ,  Window w, Bool propagate, long event_mask, XEvent* event_send);
static Hook_XSendEvent real_XSendEvent = NULL;


Status dXSendEvent( Display* display ,  Window w, Bool propagate, long event_mask, XEvent* event_send)
{
    if (!real_XSendEvent)
    {
        real_XSendEvent = dlsym(RTLD_NEXT, "XSendEvent");
    }
    //    XGetEventData
    Status size = real_XSendEvent(display, w, propagate, event_mask, event_send);
    char buffer[102400] = { 0 };

    (void)sprintf(buffer, "[real_XSendEvent][display = %p][event_mask = %u][g_count = %lu] \n", display, event_mask, g_count++);
    (void)write(1, buffer, strlen(buffer));

    //    XPeekEvent
    return size;
}

//
//extern int XSetAccessControl(
//    Display*		/* display */,
//    int			/* mode */
//);
//
//extern int XSetArcMode(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* arc_mode */
//);
//
//extern int XSetBackground(
//    Display*		/* display */,
//    GC			/* gc */,
//    unsigned long	/* background */
//);
//
//extern int XSetClipMask(
//    Display*		/* display */,
//    GC			/* gc */,
//    Pixmap		/* pixmap */
//);
//
//extern int XSetClipOrigin(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* clip_x_origin */,
//    int			/* clip_y_origin */
//);
//
//extern int XSetClipRectangles(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* clip_x_origin */,
//    int			/* clip_y_origin */,
//    XRectangle*		/* rectangles */,
//    int			/* n */,
//    int			/* ordering */
//);
//
//extern int XSetCloseDownMode(
//    Display*		/* display */,
//    int			/* close_mode */
//);
//
//extern int XSetCommand(
//    Display*		/* display */,
//    Window		/* w */,
//    char**		/* argv */,
//    int			/* argc */
//);
//
//extern int XSetDashes(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* dash_offset */,
//    _Xconst char*	/* dash_list */,
//    int			/* n */
//);
//
//extern int XSetFillRule(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* fill_rule */
//);
//
//extern int XSetFillStyle(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* fill_style */
//);
//
//extern int XSetFont(
//    Display*		/* display */,
//    GC			/* gc */,
//    Font		/* font */
//);
//
//extern int XSetFontPath(
//    Display*		/* display */,
//    char**		/* directories */,
//    int			/* ndirs */
//);
//
//extern int XSetForeground(
//    Display*		/* display */,
//    GC			/* gc */,
//    unsigned long	/* foreground */
//);
//
//extern int XSetFunction(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* function */
//);
//
//extern int XSetGraphicsExposures(
//    Display*		/* display */,
//    GC			/* gc */,
//    Bool		/* graphics_exposures */
//);
//
//extern int XSetIconName(
//    Display*		/* display */,
//    Window		/* w */,
//    _Xconst char*	/* icon_name */
//);
//
//extern int XSetInputFocus(
//    Display*		/* display */,
//    Window		/* focus */,
//    int			/* revert_to */,
//    Time		/* time */
//);
//
//extern int XSetLineAttributes(
//    Display*		/* display */,
//    GC			/* gc */,
//    unsigned int	/* line_width */,
//    int			/* line_style */,
//    int			/* cap_style */,
//    int			/* join_style */
//);
//
//extern int XSetModifierMapping(
//    Display*		/* display */,
//    XModifierKeymap*	/* modmap */
//);
//
//extern int XSetPlaneMask(
//    Display*		/* display */,
//    GC			/* gc */,
//    unsigned long	/* plane_mask */
//);
//
//extern int XSetPointerMapping(
//    Display*		/* display */,
//    _Xconst unsigned char*	/* map */,
//    int			/* nmap */
//);
//
//extern int XSetScreenSaver(
//    Display*		/* display */,
//    int			/* timeout */,
//    int			/* interval */,
//    int			/* prefer_blanking */,
//    int			/* allow_exposures */
//);
//
//extern int XSetSelectionOwner(
//    Display*		/* display */,
//    Atom	        /* selection */,
//    Window		/* owner */,
//    Time		/* time */
//);
//
//extern int XSetState(
//    Display*		/* display */,
//    GC			/* gc */,
//    unsigned long 	/* foreground */,
//    unsigned long	/* background */,
//    int			/* function */,
//    unsigned long	/* plane_mask */
//);
//
//extern int XSetStipple(
//    Display*		/* display */,
//    GC			/* gc */,
//    Pixmap		/* stipple */
//);
//
//extern int XSetSubwindowMode(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* subwindow_mode */
//);
//
//extern int XSetTSOrigin(
//    Display*		/* display */,
//    GC			/* gc */,
//    int			/* ts_x_origin */,
//    int			/* ts_y_origin */
//);
//
//extern int XSetTile(
//    Display*		/* display */,
//    GC			/* gc */,
//    Pixmap		/* tile */
//);
//
//extern int XSetWindowBackground(
//    Display*		/* display */,
//    Window		/* w */,
//    unsigned long	/* background_pixel */
//);
//
//extern int XSetWindowBackgroundPixmap(
//    Display*		/* display */,
//    Window		/* w */,
//    Pixmap		/* background_pixmap */
//);
//
//extern int XSetWindowBorder(
//    Display*		/* display */,
//    Window		/* w */,
//    unsigned long	/* border_pixel */
//);
//
//extern int XSetWindowBorderPixmap(
//    Display*		/* display */,
//    Window		/* w */,
//    Pixmap		/* border_pixmap */
//);
//
//extern int XSetWindowBorderWidth(
//    Display*		/* display */,
//    Window		/* w */,
//    unsigned int	/* width */
//);
//
//extern int XSetWindowColormap(
//    Display*		/* display */,
//    Window		/* w */,
//    Colormap		/* colormap */
//);
//
//extern int XStoreBuffer(
//    Display*		/* display */,
//    _Xconst char*	/* bytes */,
//    int			/* nbytes */,
//    int			/* buffer */
//);
//
//extern int XStoreBytes(
//    Display*		/* display */,
//    _Xconst char*	/* bytes */,
//    int			/* nbytes */
//);
//
//extern int XStoreColor(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    XColor*		/* color */
//);
//
//extern int XStoreColors(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    XColor*		/* color */,
//    int			/* ncolors */
//);
//
//extern int XStoreName(
//    Display*		/* display */,
//    Window		/* w */,
//    _Xconst char*	/* window_name */
//);
//
//extern int XStoreNamedColor(
//    Display*		/* display */,
//    Colormap		/* colormap */,
//    _Xconst char*	/* color */,
//    unsigned long	/* pixel */,
//    int			/* flags */
//);
//
//extern int XSync(
//    Display*		/* display */,
//    Bool		/* discard */
//);
//
//extern int XTextExtents(
//    XFontStruct*	/* font_struct */,
//    _Xconst char*	/* string */,
//    int			/* nchars */,
//    int*		/* direction_return */,
//    int*		/* font_ascent_return */,
//    int*		/* font_descent_return */,
//    XCharStruct*	/* overall_return */
//);
//
//extern int XTextExtents16(
//    XFontStruct*	/* font_struct */,
//    _Xconst XChar2b*	/* string */,
//    int			/* nchars */,
//    int*		/* direction_return */,
//    int*		/* font_ascent_return */,
//    int*		/* font_descent_return */,
//    XCharStruct*	/* overall_return */
//);
//
//extern int XTextWidth(
//    XFontStruct*	/* font_struct */,
//    _Xconst char*	/* string */,
//    int			/* count */
//);
//
//extern int XTextWidth16(
//    XFontStruct*	/* font_struct */,
//    _Xconst XChar2b*	/* string */,
//    int			/* count */
//);
//
//extern Bool XTranslateCoordinates(
//    Display*		/* display */,
//    Window		/* src_w */,
//    Window		/* dest_w */,
//    int			/* src_x */,
//    int			/* src_y */,
//    int*		/* dest_x_return */,
//    int*		/* dest_y_return */,
//    Window*		/* child_return */
//);
//
//extern int XUndefineCursor(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XUngrabButton(
//    Display*		/* display */,
//    unsigned int	/* button */,
//    unsigned int	/* modifiers */,
//    Window		/* grab_window */
//);
//
//extern int XUngrabKey(
//    Display*		/* display */,
//    int			/* keycode */,
//    unsigned int	/* modifiers */,
//    Window		/* grab_window */
//);
//
//extern int XUngrabKeyboard(
//    Display*		/* display */,
//    Time		/* time */
//);
//
//extern int XUngrabPointer(
//    Display*		/* display */,
//    Time		/* time */
//);
//
//extern int XUngrabServer(
//    Display*		/* display */
//);
//
//extern int XUninstallColormap(
//    Display*		/* display */,
//    Colormap		/* colormap */
//);
//
//extern int XUnloadFont(
//    Display*		/* display */,
//    Font		/* font */
//);
//
//extern int XUnmapSubwindows(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XUnmapWindow(
//    Display*		/* display */,
//    Window		/* w */
//);
//
//extern int XVendorRelease(
//    Display*		/* display */
//);
//
//extern int XWarpPointer(
//    Display*		/* display */,
//    Window		/* src_w */,
//    Window		/* dest_w */,
//    int			/* src_x */,
//    int			/* src_y */,
//    unsigned int	/* src_width */,
//    unsigned int	/* src_height */,
//    int			/* dest_x */,
//    int			/* dest_y */
//);
//
//extern int XWidthMMOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XWidthOfScreen(
//    Screen*		/* screen */
//);
//
//extern int XWindowEvent(
//    Display*		/* display */,
//    Window		/* w */,
//    long		/* event_mask */,
//    XEvent*		/* event_return */
//);
//
//extern int XWriteBitmapFile(
//    Display*		/* display */,
//    _Xconst char*	/* filename */,
//    Pixmap		/* bitmap */,
//    unsigned int	/* width */,
//    unsigned int	/* height */,
//    int			/* x_hot */,
//    int			/* y_hot */
//);
//
//extern Bool XSupportsLocale(void);
//
//extern char* XSetLocaleModifiers(
//    const char*		/* modifier_list */
//);
//
//extern XOM XOpenOM(
//    Display*			/* display */,
//    struct _XrmHashBucketRec*	/* rdb */,
//    _Xconst char*		/* res_name */,
//    _Xconst char*		/* res_class */
//);
//
//extern Status XCloseOM(
//    XOM			/* om */
//);
//
//extern char* XSetOMValues(
//    XOM			/* om */,
//    ...
//) _X_SENTINEL(0);
//
//extern char* XGetOMValues(
//    XOM			/* om */,
//    ...
//) _X_SENTINEL(0);
//
//extern Display* XDisplayOfOM(
//    XOM			/* om */
//);
//
//extern char* XLocaleOfOM(
//    XOM			/* om */
//);
//
//extern XOC XCreateOC(
//    XOM			/* om */,
//    ...
//) _X_SENTINEL(0);
//
//extern void XDestroyOC(
//    XOC			/* oc */
//);
//
//extern XOM XOMOfOC(
//    XOC			/* oc */
//);
//
//extern char* XSetOCValues(
//    XOC			/* oc */,
//    ...
//) _X_SENTINEL(0);
//
//extern char* XGetOCValues(
//    XOC			/* oc */,
//    ...
//) _X_SENTINEL(0);
//
//extern XFontSet XCreateFontSet(
//    Display*		/* display */,
//    _Xconst char*	/* base_font_name_list */,
//    char***		/* missing_charset_list */,
//    int*		/* missing_charset_count */,
//    char**		/* def_string */
//);
//
//extern void XFreeFontSet(
//    Display*		/* display */,
//    XFontSet		/* font_set */
//);
//
//extern int XFontsOfFontSet(
//    XFontSet		/* font_set */,
//    XFontStruct***	/* font_struct_list */,
//    char***		/* font_name_list */
//);
//
//extern char* XBaseFontNameListOfFontSet(
//    XFontSet		/* font_set */
//);
//
//extern char* XLocaleOfFontSet(
//    XFontSet		/* font_set */
//);
//
//extern Bool XContextDependentDrawing(
//    XFontSet		/* font_set */
//);
//
//extern Bool XDirectionalDependentDrawing(
//    XFontSet		/* font_set */
//);
//
//extern Bool XContextualDrawing(
//    XFontSet		/* font_set */
//);
//
//extern XFontSetExtents* XExtentsOfFontSet(
//    XFontSet		/* font_set */
//);
//
//extern int XmbTextEscapement(
//    XFontSet		/* font_set */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */
//);
//
//extern int XwcTextEscapement(
//    XFontSet		/* font_set */,
//    _Xconst wchar_t*	/* text */,
//    int			/* num_wchars */
//);
//
//extern int Xutf8TextEscapement(
//    XFontSet		/* font_set */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */
//);
//
//extern int XmbTextExtents(
//    XFontSet		/* font_set */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */,
//    XRectangle*		/* overall_ink_return */,
//    XRectangle*		/* overall_logical_return */
//);
//
//extern int XwcTextExtents(
//    XFontSet		/* font_set */,
//    _Xconst wchar_t*	/* text */,
//    int			/* num_wchars */,
//    XRectangle*		/* overall_ink_return */,
//    XRectangle*		/* overall_logical_return */
//);
//
//extern int Xutf8TextExtents(
//    XFontSet		/* font_set */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */,
//    XRectangle*		/* overall_ink_return */,
//    XRectangle*		/* overall_logical_return */
//);
//
//extern Status XmbTextPerCharExtents(
//    XFontSet		/* font_set */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */,
//    XRectangle*		/* ink_extents_buffer */,
//    XRectangle*		/* logical_extents_buffer */,
//    int			/* buffer_size */,
//    int*		/* num_chars */,
//    XRectangle*		/* overall_ink_return */,
//    XRectangle*		/* overall_logical_return */
//);
//
//extern Status XwcTextPerCharExtents(
//    XFontSet		/* font_set */,
//    _Xconst wchar_t*	/* text */,
//    int			/* num_wchars */,
//    XRectangle*		/* ink_extents_buffer */,
//    XRectangle*		/* logical_extents_buffer */,
//    int			/* buffer_size */,
//    int*		/* num_chars */,
//    XRectangle*		/* overall_ink_return */,
//    XRectangle*		/* overall_logical_return */
//);
//
//extern Status Xutf8TextPerCharExtents(
//    XFontSet		/* font_set */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */,
//    XRectangle*		/* ink_extents_buffer */,
//    XRectangle*		/* logical_extents_buffer */,
//    int			/* buffer_size */,
//    int*		/* num_chars */,
//    XRectangle*		/* overall_ink_return */,
//    XRectangle*		/* overall_logical_return */
//);
//
//extern void XmbDrawText(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    XmbTextItem*	/* text_items */,
//    int			/* nitems */
//);
//
//extern void XwcDrawText(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    XwcTextItem*	/* text_items */,
//    int			/* nitems */
//);
//
//extern void Xutf8DrawText(
//    Display*		/* display */,
//    Drawable		/* d */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    XmbTextItem*	/* text_items */,
//    int			/* nitems */
//);
//
//extern void XmbDrawString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    XFontSet		/* font_set */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */
//);
//
//extern void XwcDrawString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    XFontSet		/* font_set */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst wchar_t*	/* text */,
//    int			/* num_wchars */
//);
//
//extern void Xutf8DrawString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    XFontSet		/* font_set */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */
//);
//
//extern void XmbDrawImageString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    XFontSet		/* font_set */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */
//);
//
//extern void XwcDrawImageString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    XFontSet		/* font_set */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst wchar_t*	/* text */,
//    int			/* num_wchars */
//);
//
//extern void Xutf8DrawImageString(
//    Display*		/* display */,
//    Drawable		/* d */,
//    XFontSet		/* font_set */,
//    GC			/* gc */,
//    int			/* x */,
//    int			/* y */,
//    _Xconst char*	/* text */,
//    int			/* bytes_text */
//);
//
//extern XIM XOpenIM(
//    Display*			/* dpy */,
//    struct _XrmHashBucketRec*	/* rdb */,
//    char*			/* res_name */,
//    char*			/* res_class */
//);
//
//extern Status XCloseIM(
//    XIM /* im */
//);
//
//extern char* XGetIMValues(
//    XIM /* im */, ...
//) _X_SENTINEL(0);
//
//extern char* XSetIMValues(
//    XIM /* im */, ...
//) _X_SENTINEL(0);
//
//extern Display* XDisplayOfIM(
//    XIM /* im */
//);
//
//extern char* XLocaleOfIM(
//    XIM /* im*/
//);
//
//extern XIC XCreateIC(
//    XIM /* im */, ...
//) _X_SENTINEL(0);
//
//extern void XDestroyIC(
//    XIC /* ic */
//);
//
//extern void XSetICFocus(
//    XIC /* ic */
//);
//
//extern void XUnsetICFocus(
//    XIC /* ic */
//);
//
//extern wchar_t* XwcResetIC(
//    XIC /* ic */
//);
//
//extern char* XmbResetIC(
//    XIC /* ic */
//);
//
//extern char* Xutf8ResetIC(
//    XIC /* ic */
//);
//
//extern char* XSetICValues(
//    XIC /* ic */, ...
//) _X_SENTINEL(0);
//
//extern char* XGetICValues(
//    XIC /* ic */, ...
//) _X_SENTINEL(0);
//
//extern XIM XIMOfIC(
//    XIC /* ic */
//);
//
//extern Bool XFilterEvent(
//    XEvent*	/* event */,
//    Window	/* window */
//);
//
//extern int XmbLookupString(
//    XIC			/* ic */,
//    XKeyPressedEvent*	/* event */,
//    char*		/* buffer_return */,
//    int			/* bytes_buffer */,
//    KeySym*		/* keysym_return */,
//    Status*		/* status_return */
//);
//
//extern int XwcLookupString(
//    XIC			/* ic */,
//    XKeyPressedEvent*	/* event */,
//    wchar_t*		/* buffer_return */,
//    int			/* wchars_buffer */,
//    KeySym*		/* keysym_return */,
//    Status*		/* status_return */
//);
//
//extern int Xutf8LookupString(
//    XIC			/* ic */,
//    XKeyPressedEvent*	/* event */,
//    char*		/* buffer_return */,
//    int			/* bytes_buffer */,
//    KeySym*		/* keysym_return */,
//    Status*		/* status_return */
//);
//
//extern XVaNestedList XVaCreateNestedList(
//    int /*unused*/, ...
//) _X_SENTINEL(0);
//
///* internal connections for IMs */
//
//extern Bool XRegisterIMInstantiateCallback(
//    Display*			/* dpy */,
//    struct _XrmHashBucketRec*	/* rdb */,
//    char*			/* res_name */,
//    char*			/* res_class */,
//    XIDProc			/* callback */,
//    XPointer			/* client_data */
//);
//
//extern Bool XUnregisterIMInstantiateCallback(
//    Display*			/* dpy */,
//    struct _XrmHashBucketRec*	/* rdb */,
//    char*			/* res_name */,
//    char*			/* res_class */,
//    XIDProc			/* callback */,
//    XPointer			/* client_data */
//);
//
//typedef void (*XConnectionWatchProc)(
//    Display*			/* dpy */,
//    XPointer			/* client_data */,
//    int				/* fd */,
//    Bool			/* opening */,	 /* open or close flag */
//    XPointer*			/* watch_data */ /* open sets, close uses */
//    );
//
//
//extern Status XInternalConnectionNumbers(
//    Display*			/* dpy */,
//    int**			/* fd_return */,
//    int*			/* count_return */
//);
//
//extern void XProcessInternalConnection(
//    Display*			/* dpy */,
//    int				/* fd */
//);
//
//extern Status XAddConnectionWatch(
//    Display*			/* dpy */,
//    XConnectionWatchProc	/* callback */,
//    XPointer			/* client_data */
//);
//
//extern void XRemoveConnectionWatch(
//    Display*			/* dpy */,
//    XConnectionWatchProc	/* callback */,
//    XPointer			/* client_data */
//);
//
//extern void XSetAuthorization(
//    char*			/* name */,
//    int				/* namelen */,
//    char*			/* data */,
//    int				/* datalen */
//);
//
//extern int _Xmbtowc(
//    wchar_t*			/* wstr */,
//    char*			/* str */,
//    int				/* len */
//);
//
//extern int _Xwctomb(
//    char*			/* str */,
//    wchar_t			/* wc */
//);
//
//extern Bool XGetEventData(
//    Display*			/* dpy */,
//    XGenericEventCookie*	/* cookie*/
//);
//
//extern void XFreeEventData(
//    Display*			/* dpy */,
//    XGenericEventCookie*	/* cookie*/
//);
//
//
// __attribute__((constructor))
// void  test_hook()
// {


//     FILE * out_file_ptr = fopen("chensong.log", "wb+");
// }
