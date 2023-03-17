#include <iostream>
#include <string>
#include <sstream>
// #include "NvEncoderGL.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <cstring>
#include <functional>
#include <X11/Xlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xdefs.h>
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <thread>
#include <X11/Xlib.h>
#include <X11/XKBlib.h> 
#include <X11/Xutil.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/composite.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/composite.h>
// #include "cgl_egl_common.h"
// #include "NvEncoderGL.h"/
// #include "NvEncoderCLIOptions.h"
 struct WindowInfo {
            std::string cls;
            std::string name;
                Window win;
                int pid;
            WindowInfo()
            : cls("")
            , name("")
            , win(0 )
            , pid(0){}
        };
        typedef std::vector<WindowInfo>                                  all_window_info;

Display          *  g_display_ptr = NULL;
all_window_info     g_all_window_info;
/* Arbitrary window property retrieval
 * slightly modified version from xprop.c from Xorg */
unsigned char *xdo_get_window_property_by_atom(Display *display, Window window, Atom atom,
                                            long *nitems, Atom *type, int *size) {
  Atom actual_type;
  int actual_format;
  unsigned long _nitems;
  /*unsigned long nbytes;*/
  unsigned long bytes_after; /* unused */
  unsigned char *prop;
  int status;

  status = XGetWindowProperty(display, window, atom, 0, (~0L),
                              False, AnyPropertyType, &actual_type,
                              &actual_format, &_nitems, &bytes_after,
                              &prop);
  if (status == BadWindow) 
  {
    printf("[%s][%d][error]X window id # 0x%lx does not exists!", window, __FUNCTION__, __LINE__);
  //  fprintf(stderr, "window id # 0x%lx does not exists!", window);
    return NULL;
  }

if (status != Success) 
{
    printf("[%s][%d][error]XGetWindowProperty failed!\n", __FUNCTION__, __LINE__);
    //fprintf(stderr, "XGetWindowProperty failed!");
    return NULL;
  }

  /*
   *if (actual_format == 32)
   *  nbytes = sizeof(long);
   *else if (actual_format == 16)
   *  nbytes = sizeof(short);
   *else if (actual_format == 8)
   *  nbytes = 1;
   *else if (actual_format == 0)
   *  nbytes = 0;
   */

  if (nitems != NULL) {
    *nitems = _nitems;
  }

  if (type != NULL) {
    *type = actual_type;
  }

  if (size != NULL) {
    *size = actual_format;
  }
  return prop;
}
int xdo_get_pid_window(Display *display, Window window) 
{
  Atom type;
  int size;
  long nitems;
  unsigned char *data;
  int window_pid = 0;

  static Atom atom_NET_WM_PID = -1;
  //if (atom_NET_WM_PID == (Atom)-1) 
  {
    atom_NET_WM_PID = XInternAtom( display, "_NET_WM_PID", False);
  }

  data = xdo_get_window_property_by_atom(display, window, atom_NET_WM_PID, &nitems, &type, &size);

  if (nitems > 0) 
  {
    /* The data itself is unsigned long, but everyone uses int as pid values */
    window_pid = (int) *((unsigned long *)data);
  }
  free(data);

  return window_pid;
}

 static std::string getWindowAtom(Display * xdisp, Window win, const char *atom)
    {
        if (!xdisp)
        {
            printf("[%s][%d][error]xdisp = NULL !!!", __FUNCTION__, __LINE__);
            return std::string();
        }
        Atom netWmName = XInternAtom(xdisp, atom, false);
        int n;
        char **list = 0;
        XTextProperty tp;
        std::string res = "unknown";

        XGetTextProperty(xdisp, win, &tp, netWmName);

        if (!tp.nitems)
        {
            printf("[%s][%d][error]xdisp = NULL !!!", __FUNCTION__, __LINE__);
            XGetWMName(xdisp, win, &tp);
        }

        if (!tp.nitems)
        {
            printf("[%s][%d][error]xdisp = NULL !!!", __FUNCTION__, __LINE__);
            return "error";
        }

        if (tp.encoding == XA_STRING)
        {
            res = (char *)tp.value;
        } else {
            int ret = XmbTextPropertyToTextList(xdisp, &tp, &list, &n);

            if (ret >= Success && n > 0 && *list)
            {
                res = *list;
                XFreeStringList(list);
            }
        }

//    char *conv = nullptr;
//    if (os_mbs_to_utf8_ptr(res.c_str(), 0, &conv))
//        res = conv;100
//    bfree(conv);

        XFree(tp.value);

        return res;
    }
    inline std::string getWindowName(Display *display,  Window win)
    {
        return getWindowAtom(display, win, "_NET_WM_NAME");
    }

    inline std::string getWindowClass(Display *display,Window win)
    {
        return getWindowAtom(display, win, "WM_CLASS");
    }

  bool  _ewmhIsSupported()
    {

//        Display *display = xdisp;
        Atom netSupportingWmCheck = XInternAtom(g_display_ptr, "_NET_SUPPORTING_WM_CHECK", true);
        Atom actualType;
        int format = 0;
        unsigned long num = 0, bytes = 0;
        unsigned char *data = NULL;
        Window ewmh_window = 0;

        int status = XGetWindowProperty(g_display_ptr, DefaultRootWindow(g_display_ptr),
                                        netSupportingWmCheck, 0L, 1L, false,
                                        XA_WINDOW, &actualType, &format, &num,
                                        &bytes, &data);

        if (status == Success)
        {
            if (num > 0)
            {
                ewmh_window = ((Window *)data)[0];
            }
            if (data)
            {
                XFree(data);
                data = NULL;
            }
        }

        if (ewmh_window)
        {
            status = XGetWindowProperty(g_display_ptr, ewmh_window,
                                        netSupportingWmCheck, 0L, 1L, false,
                                        XA_WINDOW, &actualType, &format,
                                        &num, &bytes, &data);
            if (status != Success || num == 0 ||
                ewmh_window != ((Window *)data)[0])
            {
                ewmh_window = 0;
            }
            if (status == Success && data)
            {
                XFree(data);
            }
        }

        return ewmh_window != 0;
    }
void  _show_all_window_info()
    {
        printf("all window info ...\n");
        for (auto & win : g_all_window_info)
        {
            printf("[name = %s][cls = %s][win = %u][pid = %u]\n", win.name.c_str(), win.cls.c_str(), win.win, win.pid);
        }

    }
    void get_all_window_info()
    {
        g_all_window_info.clear();
        Atom netClList = XInternAtom(g_display_ptr, "_NET_CLIENT_LIST", true);
        Atom actualType;
        int format;
        unsigned long num, bytes;
        Window *data = 0;

        for (int i = 0; i < ScreenCount(g_display_ptr); ++i)
        {
            Window rootWin = RootWindow(g_display_ptr, i);

            int status = XGetWindowProperty(g_display_ptr, rootWin, netClList, 0L,
                                            ~0L, false, AnyPropertyType,
                                            &actualType, &format, &num,
                                            &bytes, (uint8_t **)&data);

            if (status != Success)
            {
                printf("[%s][%d][error]Failed getting root  window properties\n", __FUNCTION__, __LINE__);
                continue;
            }

            for (unsigned long i = 0; i < num; ++i)
            {
//                res.push_back(data[i]);
                WindowInfo windowinfo;
                windowinfo.name =  getWindowName(g_display_ptr, data[i]);
                windowinfo.cls =  getWindowClass(g_display_ptr, data[i]);
                windowinfo.win = data[i];
                windowinfo.pid = xdo_get_pid_window(g_display_ptr, data[i]);
                g_all_window_info.emplace_back(windowinfo);
            }

            XFree(data);
        }
        _show_all_window_info();
    }

bool init_window()
{
    g_display_ptr = XOpenDisplay(NULL);
    if (!g_display_ptr)
    {
        printf("[%s][%d]XOpenDisplay not open filed !!!\n", __FUNCTION__, __LINE__);
        return false;
    }

    Status status =  XInitThreads();
   // NORMAL_EX_LOG("XInitThreads status = %u", status);
    int32_t eventBase, errorBase;
    if (!XCompositeQueryExtension(g_display_ptr, &eventBase, &errorBase))
    {
        printf("[%s][%d]Xcomposite extension not supported\n", __FUNCTION__, __LINE__);
        return false;
    }

    int major = 0, minor = 2;
    status = XCompositeQueryVersion(g_display_ptr, &major, &minor);
   // NORMAL_EX_LOG("XCompositeQueryVersion status = %u", status);
    if (major == 0 && minor < 2)
    {
         printf("[%s][%d]Xcomposite extension is too old: %d.%d < 0.2\n", __FUNCTION__, __LINE__, major, minor);
        return false;
    }
    if (!_ewmhIsSupported())
    {
         printf("[%s][%d]Unable to query window list because window manager  does not support extended  window manager Hints", __FUNCTION__, __LINE__);
        return  false;
    }
get_all_window_info();
    return true;
}



int main(int argc, char *argv[])
{

    init_window();

    return 0;
}