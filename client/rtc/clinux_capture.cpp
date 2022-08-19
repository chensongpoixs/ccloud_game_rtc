/***********************************************************************************************
created: 		2022-08-17

author:			chensong

purpose:		linux_app_capture
************************************************************************************************/


#include "clinux_capture.h"
#ifdef __unix__
#include "clog.h"
#include "cclient.h"
#include "ccfg.h"
#include "cinput_device.h"
namespace  chen {

    static std::string getWindowAtom(Display * xdisp, Window win, const char *atom)
    {
        if (!xdisp)
        {
            ERROR_EX_LOG("xdisp = NULL !!!");
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
            ERROR_EX_LOG("xdisp = NULL !!!");
            XGetWMName(xdisp, win, &tp);
        }

        if (!tp.nitems)
        {
            ERROR_EX_LOG("xdisp = NULL !!!");
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

    clinux_capture::~clinux_capture()
    {}

    bool clinux_capture::init()
    {
        m_display_ptr = XOpenDisplay(NULL);
        if (!m_display_ptr)
        {
            ERROR_EX_LOG("XOpenDisplay not open filed !!!");
            return false;
        }



        Status status =  XInitThreads();
        SYSTEM_LOG("XInitThreads status = %u", status);
        int32 eventBase, errorBase;
        if (!XCompositeQueryExtension(m_display_ptr, &eventBase, &errorBase))
        {
            ERROR_EX_LOG( "Xcomposite extension not supported\n");
            return false;
        }

        int major = 0, minor = 2;
        status = XCompositeQueryVersion(m_display_ptr, &major, &minor);
        SYSTEM_LOG("XCompositeQueryVersion status = %u", status);
        if (major == 0 && minor < 2)
        {
            ERROR_EX_LOG( "Xcomposite extension is too old: %d.%d < 0.2\n", major, minor);
            return false;
        }
        if (!_ewmhIsSupported())
        {
            ERROR_EX_LOG("Unable to query window list because window manager  does not support extended  window manager Hints");
            return  false;
        }
        _get_all_window_info();

        return true;
    }

    bool clinux_capture::startup(const char * window_name)
    {
        if (!_find_window_name(window_name))
        {
            _show_all_window_info();
            ERROR_EX_LOG("not find window_name = %s", window_name);
            return false;
        }
        s_input_device.set_main_window(m_win);
//        m_win = win;
        xcb_generic_error_t *err = NULL, *err2 = NULL;
        m_connection_ptr = xcb_connect(NULL,NULL); //XGetXCBConnection(helper_disp);
        if (!m_connection_ptr)
        {
            ERROR_EX_LOG("xcb_connect failed !!!");
            return false;
        }
        xcb_composite_query_version_cookie_t comp_ver_cookie = xcb_composite_query_version(m_connection_ptr, 0, 2);
        xcb_composite_query_version_reply_t *comp_ver_reply = xcb_composite_query_version_reply(m_connection_ptr, comp_ver_cookie, &err);
        if (comp_ver_reply)
        {
            if (comp_ver_reply->minor_version < 2)
            {
                ERROR_EX_LOG("query composite failure: server returned v%d.%d", comp_ver_reply->major_version, comp_ver_reply->minor_version);
                free(comp_ver_reply);
                false;
            }
            free(comp_ver_reply);
        }
        else if (err)
        {
            ERROR_EX_LOG( "xcb error: %d\n", err->error_code);
            free(err);
            return false;
        }

        const xcb_setup_t *setup = xcb_get_setup(m_connection_ptr);
        xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(setup);
        xcb_screen_t *screen = screen_iter.data;
        // request redirection of window
        xcb_composite_redirect_window(m_connection_ptr, m_win, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
//    int win_h, win_w, win_d;

        xcb_get_geometry_cookie_t gg_cookie = xcb_get_geometry(m_connection_ptr, m_win);
        xcb_get_geometry_reply_t *gg_reply = xcb_get_geometry_reply(m_connection_ptr, gg_cookie, &err);
        if (gg_reply)
        {
            m_win_width = gg_reply-> width;
            m_win_height = gg_reply->height;
            m_win_depth = gg_reply->depth;
            free(gg_reply);
        }
        else
        {
            if (err) {
                ERROR_EX_LOG( "get geometry: XCB error %d\n", err->error_code);
                free(err);
            }
            return  false;
        }

        SYSTEM_LOG("app capture [width = %u][height = %u][depth = %u]", m_win_width, m_win_height, m_win_depth);
        m_win_pixmap = xcb_generate_id(m_connection_ptr);
        xcb_void_cookie_t name_cookie = xcb_composite_name_window_pixmap(m_connection_ptr, m_win, m_win_pixmap);

        err = NULL;
        if ((err = xcb_request_check(m_connection_ptr, name_cookie)) != NULL)
        {
            ERROR_EX_LOG("xcb_composite_name_window_pixmap failed\n");

            return  false;
        }
        SYSTEM_LOG("app catpure pixmap ---> ok !!!");
        m_stoped = false;
        m_thread = std::thread(&clinux_capture::_work_thread, this);



        return true;
    }

    void clinux_capture::stop()
    {

        m_stoped = true;
    }

    void clinux_capture::destroy()
    {
        stop();
        if (m_thread.joinable())
        {
            m_thread.join();
        }
        if (m_display_ptr)
        {
            XCloseDisplay(m_display_ptr);
            m_display_ptr = NULL;
        }
        if (m_connection_ptr)
        {
            xcb_disconnect(m_connection_ptr);
            m_connection_ptr = NULL;
        }
    }

    void clinux_capture::_work_thread()
    {
        if (0 == m_win_pixmap || NULL == m_connection_ptr ||  NULL == m_display_ptr)
        {

            ERROR_EX_LOG("!!g_win_pixmap = %u|| !!g_connection = %p|| !!helper_disp = %p\n", m_win_pixmap, m_connection_ptr, m_display_ptr);
            return;
        }
        std::chrono::steady_clock::time_point cur_time_ms;
        std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration dur;
        std::chrono::milliseconds ms;
        uint32_t elapse = 0;
        uint32_t CAPTUER_TICK_TIME = 100 /30;
        if (g_cfg.get_uint32(ECI_RtcFrames)> 0)
        {
            CAPTUER_TICK_TIME = 100 /g_cfg.get_uint32(ECI_RtcFrames);
        }
        while (!m_stoped)
        {
            pre_time = std::chrono::steady_clock::now();
            xcb_generic_error_t *err = NULL, *err2 = NULL;
            xcb_get_image_cookie_t gi_cookie = xcb_get_image(m_connection_ptr, XCB_IMAGE_FORMAT_Z_PIXMAP, m_win_pixmap, 0, 0, m_win_width, m_win_height, (uint32_t)(~0UL));
            xcb_get_image_reply_t *gi_reply = xcb_get_image_reply(m_connection_ptr, gi_cookie, &err);
            if (gi_reply)
            {
                uint8_t *data = xcb_get_image_data(gi_reply);
                s_client.webrtc_video(data, m_win_width, m_win_height);
//                    if (callback)
//                    {
//                        callback(data, g_win_w, g_win_h);
//                    }
                free(gi_reply);
            }
            else
            {
                WARNING_EX_LOG("gi reply failed !!!");
            }
            if (!m_stoped)
            {
                cur_time_ms = std::chrono::steady_clock::now();
                dur = cur_time_ms - pre_time;
                ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
                elapse = static_cast<uint32_t>(ms.count());
                if (elapse < CAPTUER_TICK_TIME)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(CAPTUER_TICK_TIME- elapse));
                }

            }
        }
    }

    bool clinux_capture::_ewmhIsSupported()
    {

//        Display *display = xdisp;
        Atom netSupportingWmCheck = XInternAtom(m_display_ptr, "_NET_SUPPORTING_WM_CHECK", true);
        Atom actualType;
        int format = 0;
        unsigned long num = 0, bytes = 0;
        unsigned char *data = NULL;
        Window ewmh_window = 0;

        int status = XGetWindowProperty(m_display_ptr, DefaultRootWindow(m_display_ptr),
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
            status = XGetWindowProperty(m_display_ptr, ewmh_window,
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

    bool clinux_capture::_get_all_window_info()
    {
        m_all_window_info.clear();

        Atom netClList = XInternAtom(m_display_ptr, "_NET_CLIENT_LIST", true);
        Atom actualType;
        int format;
        unsigned long num, bytes;
        Window *data = 0;

        for (int i = 0; i < ScreenCount(m_display_ptr); ++i)
        {
            Window rootWin = RootWindow(m_display_ptr, i);

            int status = XGetWindowProperty(m_display_ptr, rootWin, netClList, 0L,
                                            ~0L, false, AnyPropertyType,
                                            &actualType, &format, &num,
                                            &bytes, (uint8_t **)&data);

            if (status != Success)
            {
                WARNING_EX_LOG("Failed getting root  window properties\n");
                continue;
            }

            for (unsigned long i = 0; i < num; ++i)
            {
//                res.push_back(data[i]);
                WindowInfo windowinfo;
                windowinfo.name =  getWindowName(m_display_ptr, data[i]);
                windowinfo.cls =  getWindowClass(m_display_ptr, data[i]);
                windowinfo.win = data[i];
                m_all_window_info.emplace_back(windowinfo);
            }

            XFree(data);
        }
        _show_all_window_info();
        return true;
    }
    void clinux_capture::_show_all_window_info()
    {
        SYSTEM_LOG("all window info ...");
        for (auto & win : m_all_window_info)
        {
            SYSTEM_LOG("[name = %s][cls = %s][win = %u]", win.name.c_str(), win.cls.c_str(), win.win);
        }

    }


    bool clinux_capture::_find_window_name(const char * window_name)
    {
        for (const WindowInfo& win: m_all_window_info)
        {
            if (window_name == win.cls)
            {
                m_win = win.win;
//                ERROR_EX_LOG("");
                return true;
            }
        }
        return false;
    }
}
#endif