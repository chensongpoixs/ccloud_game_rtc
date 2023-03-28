/***********************************************************************************************
created: 		2022-08-17

author:			chensong

purpose:		linux_app_capture
************************************************************************************************/


#ifndef CLOUD_GAME_RTC_CLINUX_CAPTURE_H
#define CLOUD_GAME_RTC_CLINUX_CAPTURE_H

#ifdef __unix__
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
#include "cnet_types.h"
#include <X11/Xutil.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/composite.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/composite.h>
namespace  chen {
    class clinux_capture {
    private:
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

    public:
        clinux_capture()
        : m_stoped(false)
        , m_display_ptr(NULL)

        , m_all_window_info()
        , m_connection_ptr(NULL)
        , m_win(0)
        , m_win_width(0)
        , m_win_height(0)
        , m_win_depth(0)
        , m_win_pixmap(0)
        , m_win_name("")
        , m_video_width(0)
        , m_video_height(0)
        {}
        ~clinux_capture();
    public:

        bool init();

        bool startup(const char * window_name);

        void stop();

        void destroy();

    private:
        void _work_thread();
        bool _ewmhIsSupported();
        /**
         *  get all window
         * @return
         */
        bool _get_all_window_info();


        void _show_all_window_info();

        bool _find_window_name(const char * window_name);

        bool _init_window();


        bool _check_xcomp_window_exists();

        bool _watcher_process(xcb_generic_event_t *ev);
    private:
        bool                m_stoped;
        Display          *  m_display_ptr;

        std::thread         m_thread;
        all_window_info     m_all_window_info;

        /////////////////////////app capture //////////////////////////////
        xcb_connection_t *  m_connection_ptr;
        Window               m_win;
        uint32               m_win_width;
        uint32               m_win_height;
        uint32               m_win_depth;
        /*xcb_pixmap_t*/ Pixmap         m_win_pixmap;
        std::string          m_win_name;
//        extern  Window  g_window;
//        extern int g_win_h;
//        extern int g_win_w;
//        extern int g_win_d;
//        extern xcb_pixmap_t g_win_pixmap;


        uint32              m_video_width;
        uint32              m_video_height;
    };
}


#endif

#endif //CLOUD_GAME_RTC_CLINUX_CAPTURE_H
