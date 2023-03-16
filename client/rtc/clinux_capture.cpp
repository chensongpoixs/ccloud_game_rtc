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
#include <iostream>
#include <memory>
#include <stdint.h>
#include "Logger.h"
#include "NvEncoderGL.h"
#include "NvEncoderCLIOptions.h"
//#include "NvCodecUtils.h"
// #include <GL/glew.h>
// #include <GL/glut.h>
// #include <GL/freeglut_ext.h>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <cstring>
#include <functional>
#include "cgl_egl_common.h"

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






/////////////////////////////////////////////pid -> window////////////////////////////////////////////////////////////////////////


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
    ERROR_EX_LOG("window id # 0x%lx does not exists!", window);
  //  fprintf(stderr, "window id # 0x%lx does not exists!", window);
    return NULL;
  }

if (status != Success) 
{
    ERROR_EX_LOG("XGetWindowProperty failed!");
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


static int silence_x11_errors(Display *display, XErrorEvent *error)
{
	//UNUSED_PARAMETER(display);
	//UNUSED_PARAMETER(err);
    char str1[512];
	char str2[512];
	char str3[512];
	XGetErrorText(display, error->error_code, str1, sizeof(str1));
	XGetErrorText(display, error->request_code, str2, sizeof(str2));
	XGetErrorText(display, error->minor_code, str3, sizeof(str3));

	ERROR_EX_LOG( "X Error: %s, Major opcode: %s, " "Minor opcode: %s, Serial: %lu", str1, str2, str3, error->serial);
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
        NORMAL_EX_LOG("XInitThreads status = %u", status);
        int32 eventBase, errorBase;
        if (!XCompositeQueryExtension(m_display_ptr, &eventBase, &errorBase))
        {
            ERROR_EX_LOG( "Xcomposite extension not supported\n");
            return false;
        }

        int major = 0, minor = 2;
        status = XCompositeQueryVersion(m_display_ptr, &major, &minor);
        NORMAL_EX_LOG("XCompositeQueryVersion status = %u", status);
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

        m_win_name = window_name;
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
        ////////////////////////////////////////////////////////////////////////////////////////////////////
       
        int count = 0;
        while (count++ < 2)
        {
            NORMAL_EX_LOG("linux capture sleep !!!");
            sleep(1);
        }
        _get_all_window_info();
        if (!_find_window_name(m_win_name.c_str()))
        {
            _show_all_window_info();
            ERROR_EX_LOG("not find window_name = %s", m_win_name.c_str());
            return ;
        }
        set_global_display(m_display_ptr);
        s_input_device.set_main_window(m_win);
//        m_win = win;
        xcb_generic_error_t *err = NULL, *err2 = NULL;
        m_connection_ptr = xcb_connect(NULL,NULL); //XGetXCBConnection(helper_disp);
        if (!m_connection_ptr)
        {
            ERROR_EX_LOG("xcb_connect failed !!!");
            return ;
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
            return ;
        }

        const xcb_setup_t *setup = xcb_get_setup(m_connection_ptr);
        xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(setup);
        xcb_screen_t *screen = screen_iter.data;
        // request redirection of window
        //这个代码将窗口内容重定向的离屏缓存并且跟踪 damage 信号，
        xcb_composite_redirect_window(m_connection_ptr, m_win, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
 
        _init_window();
        
        NORMAL_EX_LOG("app capture [width = %u][height = %u][depth = %u]", m_win_width, m_win_height, m_win_depth);
         
        xcb_map_window(m_connection_ptr, m_win);

        xcb_flush(m_connection_ptr);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        
        std::chrono::steady_clock::time_point cur_time_ms;
        std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration dur;
        std::chrono::milliseconds ms;
        int32_t elapse = 0;
        int32_t CAPTUER_TICK_TIME = 1000 /30;
        if (g_cfg.get_int32(ECI_RtcFrames)> 0)
        {
            CAPTUER_TICK_TIME = 1000 /g_cfg.get_int32(ECI_RtcFrames);
        }
        NORMAL_EX_LOG("cpature tick time frames = %u", CAPTUER_TICK_TIME);

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     char   szOutFilePath[256] = "test_yuv.h264";
    //     int32 nWidth = m_win_width, nHeight = m_win_height;
    //     NV_ENC_BUFFER_FORMAT eFormat = NV_ENC_BUFFER_FORMAT_IYUV;
        
    //     //  glutInit();
    //     glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    //      std::ostringstream oss;
    //         oss << "-codec h264 -fps 25 ";
    //       NvEncoderInitParam encodeCLIOptions(oss.str().c_str());
    //     // glutInitWindowSize(16, 16);
    //     //     int window = glutCreateWindow("AppEncGL");
    //     //     if (!window)
    //     //     {
    //     //         std::cout << "Unable to create GLUT window." << std::endl;
    //     //         return 1;
    //     //     }
    //     //     glutHideWindow();

    //      NvEncoderGL enc(nWidth, nHeight, eFormat);

    //     NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
    //     NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
    //     initializeParams.encodeConfig = &encodeConfig;
    //     enc.CreateDefaultEncoderParams(&initializeParams, encodeCLIOptions.GetEncodeGUID(),
    //         encodeCLIOptions.GetPresetGUID());

    //     encodeCLIOptions.SetInitParams(&initializeParams, eFormat);

    //     enc.CreateEncoder(&initializeParams);

    //     int nFrameSize = enc.GetFrameSize();
    //     std::unique_ptr<uint8_t[]> pHostFrame(new uint8_t[nFrameSize]);
        int nFrame = 0;
    // std::ofstream fpOut(szOutFilePath, std::ios::out | std::ios::binary);
        // if (!fpOut)
        // {
        //     std::ostringstream err;
        //     err << "Unable to open output file: " << szOutFilePath << std::endl;
        //     throw std::invalid_argument(err.str());
        // }

        while (!m_stoped)
        { 
            pre_time = std::chrono::steady_clock::now();

           if (0 == m_win_pixmap || NULL == m_connection_ptr ||  NULL == m_display_ptr)
            {

                WARNING_EX_LOG("!!g_win_pixmap = %u|| !!g_connection = %p|| !!helper_disp = %p\n", m_win_pixmap, m_connection_ptr, m_display_ptr);
               
            }
            else 
            {
                //  const NvEncInputFrame* encoderInputFrame = enc.GetNextInputFrame();
                // NV_ENC_INPUT_RESOURCE_OPENGL_TEX *pResource = (NV_ENC_INPUT_RESOURCE_OPENGL_TEX *)encoderInputFrame->inputPtr;

                gl_egl_create_texture_from_pixmap(NULL/*pResource*/ , m_win_width, m_win_height, GL_BGRA, EGL_TEXTURE_2D,  m_win_pixmap );
                std::vector<std::vector<uint8_t>> vPacket;
                // enc.EndEncode(vPacket);
                  nFrame += (int)vPacket.size();
                for (std::vector<uint8_t> &packet : vPacket)
                {
                    // fpOut.write(reinterpret_cast<char*>(packet.data()), packet.size());
                    //fpOut.out.flush();
                }
                // xcb_generic_error_t *err = NULL, *err2 = NULL;
                // xcb_get_image_cookie_t gi_cookie = xcb_get_image(m_connection_ptr, XCB_IMAGE_FORMAT_Z_PIXMAP, m_win_pixmap, 0, 0, m_win_width, m_win_height, (uint32_t)(~0UL));
                // xcb_get_image_reply_t *gi_reply = xcb_get_image_reply(m_connection_ptr, gi_cookie, &err);
                // if (gi_reply)
                // {
                //     uint8_t *data = xcb_get_image_data(gi_reply);
                //     s_client.webrtc_video(data, 48,  m_win_width, m_win_height);
                //     static FILE * out_file_yuv_ptr = fopen("./capture.yuv", "wb+");
                //     NORMAL_EX_LOG("get frame OK !!!");
                //     fwrite(data, m_win_width * m_win_height * 4, 1,  out_file_yuv_ptr);
                //     fflush(out_file_yuv_ptr);
                //     free(gi_reply);
                // }
                // else
                // {
                //     WARNING_EX_LOG("gi reply failed !!!");
                // }
            }
            //if (!m_stoped)
            {
                cur_time_ms = std::chrono::steady_clock::now();
                dur = cur_time_ms - pre_time;
                pre_time = cur_time_ms;
                ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
                elapse = static_cast<int32_t>(ms.count());
                NORMAL_EX_LOG("capture frame [ms = %u][m_win_width = %u][m_win_height = %u]", elapse, m_win_width, m_win_height);
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
                windowinfo.pid = xdo_get_pid_window(m_display_ptr, data[i]);
                m_all_window_info.emplace_back(windowinfo);
            }

            XFree(data);
        }
        _show_all_window_info();
        return true;
    }
    void clinux_capture::_show_all_window_info()
    {
        NORMAL_EX_LOG("all window info ...");
        for (auto & win : m_all_window_info)
        {
            NORMAL_EX_LOG("[name = %s][cls = %s][win = %u][pid = %u]", win.name.c_str(), win.cls.c_str(), win.win, win.pid);
        }

    }


    bool clinux_capture::_find_window_name(const char * window_name)
    {
        pid_t pid = 3154;// //getpid();
        for (const WindowInfo& win: m_all_window_info)
        {
            if (/*window_name == win.cls && win.win != m_win*/ pid == win.pid)
            {
                m_win = win.win;
                NORMAL_EX_LOG("[system_pid = %u][win_pid_id = %u][win_id = %u][win_cls = %s][win_name = %s]", pid, win.pid, win.win, win.cls.c_str(), win.name.c_str());
                return true;
            }
            else
            {
                NORMAL_EX_LOG("[cls = %s][window_name = %s]", win.cls.c_str(), win.name.c_str());
            }
        }
        return false;
    }



    bool clinux_capture::_init_window()
    {
        if (!m_connection_ptr)
        {
            WARNING_EX_LOG(" m_connection_ptr == null !!!");
        }
        if (m_win_pixmap)
        {
            XFreePixmap(m_display_ptr, m_win_pixmap);
            m_win_pixmap = 0;
        }
        xcb_generic_error_t *err = NULL;
        xcb_get_geometry_cookie_t gg_cookie = xcb_get_geometry(m_connection_ptr, m_win);
        xcb_get_geometry_reply_t *gg_reply = xcb_get_geometry_reply(m_connection_ptr, gg_cookie, &err);
        if (gg_reply)
        {
            
            if (m_win_width != gg_reply-> width || m_win_height != gg_reply->height || m_win_depth != gg_reply->depth)
            {
                WARNING_EX_LOG("old [width = %u][height = %u][depth = %u] --> new [width = %u][height = %u][depth = %u]", m_win_width, m_win_height, m_win_depth, gg_reply-> width, gg_reply->height, gg_reply->depth);
                m_win_width = gg_reply-> width;
                m_win_height = gg_reply->height;
                m_win_depth = gg_reply->depth;
            }
            
            free(gg_reply);
        }
        else
        {
            if (err) 
            {
                ERROR_EX_LOG( "get geometry: XCB error %d\n", err->error_code);
                free(err);
            }
            ERROR_EX_LOG("get geometry: XCB error");
            return  false;
        }

       // NORMAL_EX_LOG("app capture [width = %u][height = %u][depth = %u]", m_win_width, m_win_height, m_win_depth);
        m_win_pixmap = xcb_generate_id(m_connection_ptr);
        xcb_void_cookie_t name_cookie = xcb_composite_name_window_pixmap(m_connection_ptr, m_win, m_win_pixmap);

        err = NULL;
        if ((err = xcb_request_check(m_connection_ptr, name_cookie)) != NULL)
        {
            ERROR_EX_LOG("xcb_composite_name_window_pixmap failed\n");

            return  false;
        }
        XErrorHandler prev = XSetErrorHandler(silence_x11_errors);

        /////////////////////////////
        //   gl_egl_create_texture_from_pixmap(m_win_width, m_win_height, GL_BGRA, EGL_TEXTURE_2D,  m_win_pixmap );
         /////////////////////


        XSetErrorHandler(prev);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
       // xcb_map_window(m_connection_ptr, m_win);

       // xcb_flush(m_connection_ptr);
      

        return true;
    }
}
#endif