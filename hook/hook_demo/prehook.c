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
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
//#include <Xlib.h>
#include <X11/Xlib.h>
//#include <gtk/gtk.h>

//#include <GL/wglew.h>
//#include <GL/glew.h>
//typedef void	(*t_glFlush)(void);
//FILE  * out_log_file_ptr = fopen("./log/hook.log", "wb+");
//#include <>


#include <xcb/xproto.h>
#define gettid() syscall(__NR_gettid)


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

void show(const char * str, size_t len)
{
//    char buffer[10240] = {0};
//    (void) sprintf(buffer, "real_XNextEvent %s [time_now :%d-%d-%d %d:%d:%d.%ld]\n", str, 1900 + t->tm_year,
//                   1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec);
    (void) write(1, str, len);
    (void) write(1, "\n", 1);
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
            int len = sprintf(buffer, "[KeyPress  type = %u][display = %p][serial = %u][send_event = %d][display = %p][window = %p][root = %u][subwindow = %p][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][keycode = %u][same_screen = = %d]\n",
                          e->xkey.type, display, e->xkey.serial, e->xkey.send_event, e->xkey.display, e->xkey.window, e->xkey.root, e->xkey.subwindow, e->xkey.x, e->xkey.y, e->xkey.x_root, e->xkey.y_root, e->xkey.state, e->xkey.keycode, e->xkey.same_screen);
            show(buffer, len);
            break;
        }
        case KeyRelease:
        {
            int len =  sprintf(buffer, "[KeyRelease  type = %u][display = %p][serial = %u][send_event = %d][display = %p][window = %p][root = %u][subwindow = %p][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][keycode = %u][same_screen = = %d]\n",
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
            int len =  sprintf(buffer, "[ButtonPress type = %u][display = %p][serial = %llu][send_event = %d][display = %p][window = %p][root = %u][time][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][button = %u][same_screen = %d]",
                          e->xbutton.type, display, e->xbutton.serial, e->xbutton.send_event, e->xbutton.display, e->xbutton.window, e->xbutton.root, e->xbutton.x, e->xbutton.y, e->xbutton.x_root, e->xbutton.y_root, e->xbutton.state, e->xbutton.button, e->xbutton.same_screen);
            show(buffer, len);
            break;
        }
        case ButtonRelease:
        {
            int len =  sprintf(buffer, "[ButtonRelease type = %u][display = %p][serial = %llu][send_event = %d][display = %p][window = %p][root = %u][time][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][button = %u][same_screen = %d]",
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

            int len =  sprintf(buffer, "[MotionNotify type = %u][ display = %p][serial = %u][send_event = %d][display = %p][window = %p][root = %u][subwindow = %p][x = %u, y = %u][x_root = %u, y_root = %u][state = %u][is_hint = %c][same_screen = %d]\n",
                          e->xmotion.type, display, e->xmotion.serial, e->xmotion.send_event, e->xmotion.display, e->xmotion.window, e->xmotion.root, e->xmotion.subwindow, e->xmotion.x, e->xmotion.y, e->xmotion.x_root, e->xmotion.y_root, e->xmotion.state, e->xmotion.is_hint, e->xmotion.same_screen);

           show(buffer, len);
            break;
        }
        case EnterNotify:
        {
            int len =  sprintf(buffer, "EnterNotify   = %d", EnterNotify);
            show(buffer, len);
            break;
        }
        case LeaveNotify:
        {
            int len =  sprintf(buffer, "LeaveNotify   = %d", LeaveNotify);
            show(buffer, len);
            break;
        }
        case FocusIn:
        {
            int len =  sprintf(buffer, "FocusIn   = %d", FocusIn);
            show(buffer, len);
            break;
        }
        case FocusOut:
        {
            int len =  sprintf(buffer, "FocusOut   = %d", FocusOut);
            show(buffer, len);
            break;
        }
        case KeymapNotify:
        {
            int len =  sprintf(buffer, "KeymapNotify   = %d", KeymapNotify);
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
            int len =  sprintf(buffer, "PropertyNotify   = %d", PropertyNotify);
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



void call_time_out(const char * str)
{
    struct timeval tv;
    struct timezone tz;
    struct tm * t;
    gettimeofday(&tv, &tz);
    t = localtime(&tv.tv_sec);
    char buffer[10240] = {0};
    (void) sprintf(buffer, "real_XNextEvent %s [time_now :%d-%d-%d %d:%d:%d.%ld]\n", str, 1900 + t->tm_year,
                   1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec);
    (void) write(1, buffer, strlen(buffer));
}
static int *(*real_XNextEvent)(
        Display*	d 	/* display */,
        XEvent*		e/* event_return */
) = NULL;

  int XNextEvent(
        Display*	d	/* display */,
        XEvent*		e/* event_return */
)
  {
      if (!real_XNextEvent)
    {
        real_XNextEvent = dlsym(RTLD_NEXT, "XNextEvent");
    }

      int ret = real_XNextEvent(d, e);
      {
          e->xbutton.serial + g_new_seria + 1;

          show_hook_info(d, e);


      }

    return ret;
  }

  static int *(*real_XSelectInput)(
          Display*	display	/* display */,
          Window	w	/* w */,
          long		event_mask/* event_mask */
  ) = NULL;


  int  XSelectInput(
Display* display		/* display */,
Window	w 	/* w */,
long	event_mask	/* event_mask */
)
  {
      if (!real_XSelectInput)
      {
          real_XSelectInput = dlsym(RTLD_NEXT, "XSelectInput");
      }

      int size  = real_XSelectInput(display, w, event_mask);
      char buffer[102400] = {0};
      (void)sprintf(buffer, "[real_XSelectInput][display = %p][window = %u][event_mask = %lu][ret = %u]\n", display, w, event_mask, size);
      (void)write(1, buffer, strlen(buffer));
      return size;
  }
static  int* (*real_XPending)(
        Display*	display	/* display */
) = NULL;


int XPending(
        Display*	display	/* display */
)
{
        if (!real_XPending)
        {
            real_XPending = dlsym(RTLD_NEXT, "XPending");
        }
        char buffer[102400] = {0};

        (void)sprintf(buffer, "[real_XPending][display = %p] \n", display);
        (void)write(1, buffer, strlen(buffer));


        return real_XPending(display );
}

//  gtk_widget_add_event();
//  gtk_widget_add_event();
//
//static void (*real_glFlush)(void   ) = NULL;
//
////static GLenum  (*real_glewInit) (void) = NULL;
////
////GLenum  glewInit (void)
////{
////    if (!real_glewInit)
////    {
////        real_glewInit = dlsym(RTLD_NEXT, "glewInit");
////    }
////    char buffer[100];
////    (void) sprintf(buffer, "%lu glewInit   == %p\n", gettid(),   real_glewInit);
////    (void) write(1, buffer, strlen(buffer));
////    return real_glewInit();
////}
//void  glFlush(void)
//{
//
//    if (!real_glFlush)
//    {
//        real_glFlush = dlsym(RTLD_NEXT, "glFlush");
//    }
//
//
//    char buffer[100];
//    (void) sprintf(buffer, "%lu glFinish   == %p\n", gettid(),   real_glFlush);
//    (void) write(1, buffer, strlen(buffer));
//    real_glFlush();
//}
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
