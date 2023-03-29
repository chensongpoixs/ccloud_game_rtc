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
//#include <glad/glad.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/composite.h>
// #include "NvCodecUtils.h"
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
#include "NvEncoderGL.h"
// #include "NvEncoderCLIOptions.h"
#include "cgl_global.h"
#include "cclient.h"
#include "third_party/libyuv/include/libyuv.h"

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
        // Subscribe to Events
		uint32_t vals[1] = {StructureNotifyMask | ExposureMask | VisibilityChangeMask};
		xcb_change_window_attributes(m_connection_ptr, m_win, XCB_CW_EVENT_MASK,  vals);
        // request redirection of window
        //这个代码将窗口内容重定向的离屏缓存并且跟踪 damage 信号，
        
        xcb_composite_redirect_window(m_connection_ptr, m_win, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
 
        _init_window();
        
        NORMAL_EX_LOG("app capture [width = %u][height = %u][depth = %u]", m_win_width, m_win_height, m_win_depth);
         
        xcb_map_window(m_connection_ptr, m_win);

        xcb_flush(m_connection_ptr);

        m_video_width = m_win_width;
        m_video_height = m_win_height;
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
    //     int32 nWidth = m_win_width;
    //     int32 nHeight = m_win_height;
    //     NV_ENC_BUFFER_FORMAT eFormat = NV_ENC_BUFFER_FORMAT_IYUV;
        
    //      NORMAL_EX_LOG("");
    //    init_gl();
    //      NORMAL_EX_LOG("");
    //      std::ostringstream oss;
    //         oss << "-codec h264 -fps 25 ";
    //       NvEncoderInitParam encodeCLIOptions(oss.str().c_str());
        
    //     NORMAL_EX_LOG("");
    //      NvEncoderGL enc(nWidth, nHeight, eFormat);
    //     NORMAL_EX_LOG("");
    //     NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
    //     NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
    //     initializeParams.encodeConfig = &encodeConfig;
    //     enc.CreateDefaultEncoderParams(&initializeParams, encodeCLIOptions.GetEncodeGUID(), encodeCLIOptions.GetPresetGUID());
    //     NORMAL_EX_LOG("");
    //     encodeCLIOptions.SetInitParams(&initializeParams, eFormat);
    //     NORMAL_EX_LOG("");
    //     enc.CreateEncoder(&initializeParams);
    //     NORMAL_EX_LOG("");
    //     int nFrameSize = enc.GetFrameSize();
    //     NORMAL_EX_LOG("");
    // //     std::unique_ptr<uint8_t[]> pHostFrame(new uint8_t[nFrameSize]);
    //     int nFrame = 0;
    //     std::ofstream fpOut(szOutFilePath, std::ios::out | std::ios::binary);
    //     if (!fpOut)
    //     {
    //         std::ostringstream err;
    //         err << "Unable to open output file: " << szOutFilePath << std::endl;
    //         throw std::invalid_argument(err.str());
    //     }
        //encodec_gl(m_win_width, m_win_height, m_win_pixmap);
        NORMAL_EX_LOG("");
        rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_  = webrtc::I420Buffer::Create(m_win_width, m_win_height);
        // return ;
        while (!m_stoped)
        { 
            pre_time = std::chrono::steady_clock::now();

           if (0 == m_win_pixmap || NULL == m_connection_ptr ||  NULL == m_display_ptr)
            {

                WARNING_EX_LOG("!!g_win_pixmap = %u|| !!g_connection = %p|| !!helper_disp = %p\n", m_win_pixmap, m_connection_ptr, m_display_ptr);
               
            }
            else 
            {
                xcb_generic_event_t *event;
                bool ret = false;
                while (!ret &&(event = xcb_poll_for_queued_event(m_connection_ptr)))
                {
                    ret = _watcher_process(event);
                }
                if (ret)
                {
                    WARNING_EX_LOG("window exits ---> ");


                    uint32_t valsff[1] = {0};
		            xcb_change_window_attributes(m_connection_ptr, m_win, XCB_CW_EVENT_MASK, valsff);

                     _get_all_window_info();
                    if (!_find_window_name(m_win_name.c_str()))
                    {
                        _show_all_window_info();
                        ERROR_EX_LOG("not find window_name = %s", m_win_name.c_str());
                        break;
                    } 
                    s_input_device.set_main_window(m_win);
            
//                    comp_ver_cookie = xcb_composite_query_version(m_connection_ptr, 0, 2);
//                    comp_ver_reply = xcb_composite_query_version_reply(m_connection_ptr, comp_ver_cookie, &err);
//                    if (comp_ver_reply)
//                    {
//                        if (comp_ver_reply->minor_version < 2)
//                        {
//                            ERROR_EX_LOG("query composite failure: server returned v%d.%d", comp_ver_reply->major_version, comp_ver_reply->minor_version);
//                            free(comp_ver_reply);
//                            false;
//                        }
//                        free(comp_ver_reply);
//                    }
//                    else if (err)
//                    {
//                        ERROR_EX_LOG( "xcb error: %d\n", err->error_code);
//                        free(err);
//                        break;
//                    }

//                     setup = xcb_get_setup(m_connection_ptr);
//                     screen_iter = xcb_setup_roots_iterator(setup);
//                     screen = screen_iter.data;
 
		            xcb_change_window_attributes(m_connection_ptr, m_win, XCB_CW_EVENT_MASK,  vals);
                    // request redirection of window
                    //这个代码将窗口内容重定向的离屏缓存并且跟踪 damage 信号，
                    xcb_composite_redirect_window(m_connection_ptr, m_win, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
            
                    _init_window();
                    
                    NORMAL_EX_LOG("app capture [width = %u][height = %u][depth = %u]", m_win_width, m_win_height, m_win_depth);
                    
                    xcb_map_window(m_connection_ptr, m_win);

                    xcb_flush(m_connection_ptr);


                }
               
                xcb_generic_error_t *err = NULL, *err2 = NULL;
                xcb_get_image_cookie_t gi_cookie = xcb_get_image(m_connection_ptr, XCB_IMAGE_FORMAT_Z_PIXMAP, m_win_pixmap, 0, 0, m_win_width, m_win_height, (uint32_t)(~0UL));
                xcb_get_image_reply_t *gi_reply = xcb_get_image_reply(m_connection_ptr, gi_cookie, &err);
                if (gi_reply)
                {
                    uint8_t *data = xcb_get_image_data(gi_reply);
//                     if (m_win_width != m_video_width || m_win_height != m_video_height)
//                     {
//                         rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_temp  = webrtc::I420Buffer::Create(m_win_width, m_win_height);
//                         libyuv::ConvertToI420(data, 0, i420_buffer_temp->MutableDataY(),
//                                             i420_buffer_temp->StrideY(), i420_buffer_temp->MutableDataU(),
//                                             i420_buffer_temp->StrideU(), i420_buffer_temp->MutableDataV(),
//                                             i420_buffer_temp->StrideV(), 0, 0, m_win_width, m_win_height, m_win_width,
//                                             m_win_height, libyuv::kRotate0, libyuv::FOURCC_ARGB);
//                         libyuv:: I420Scale(i420_buffer_temp->MutableDataY(),
//                                            i420_buffer_temp->StrideY(), i420_buffer_temp->MutableDataU(),
//                                            i420_buffer_temp->StrideU(), i420_buffer_temp->MutableDataV(),
//                                            i420_buffer_temp->StrideV(),
//                                            m_win_width,
//                                            m_win_height,
//                                            i420_buffer_->MutableDataY(),
//                                            i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
//                                            i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
//                                            i420_buffer_->StrideV(),
//                                         m_video_width,
//                                         m_video_height,
//                                            libyuv::kFilterNone);
                        libyuv::ARGBScale(data, m_win_width * 4,
                                          m_win_width, m_win_height, i420_buffer_->MutableDataY(), m_video_width * 4, m_video_width, m_win_height, libyuv::kFilterNone);
//                     }
//                     else
//                     {
//                         memcpy(i420_buffer_->MutableDataY(), data, m_win_width * m_win_height * 3);
////                         libyuv::ConvertToI420(data, 0, i420_buffer_->MutableDataY(),
////                                             i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
////                                             i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
////                                             i420_buffer_->StrideV(), 0, 0, m_win_width, m_win_height, m_win_width,
////                                             m_win_height, libyuv::kRotate0, libyuv::FOURCC_ARGB);
//                     }


            //         libyuv::int ARGBToI420(data, 0,);
            //             //const uint8_t* src_argb,
            // //    int src_stride_argb,
            // //    uint8_t* dst_y,
            // //    int dst_stride_y,
            // //    uint8_t* dst_u,
            // //    int dst_stride_u,
            // //    uint8_t* dst_v,
            // //    int dst_stride_v,
            // //    int width,
            // //    int height)

                       webrtc::VideoFrame captureFrame = webrtc::VideoFrame::Builder()
                        .set_video_frame_buffer(i420_buffer_)
                        .set_timestamp_rtp(0)
                        .set_timestamp_ms(rtc::TimeMillis())
                        .set_rotation(webrtc::kVideoRotation_0)
                        .build();
                // captureFrame.set_ntp_time_ms(0);
                    s_client.webrtc_video(captureFrame);
                    //TODO@chensong send video --> rtc --> 

                    // s_client.webrtc_video(data, 48,  m_win_width, m_win_height);
                    // static FILE * out_file_yuv_ptr = fopen("./capture.yuv", "wb+");
                    // NORMAL_EX_LOG("get frame OK !!!");
                    // fwrite( i420_buffer_->DataY(), m_win_width * m_win_height * 3/2, 1,  out_file_yuv_ptr);
                    // fflush(out_file_yuv_ptr);

                    //////////////////////////////////
//                     const NvEncInputFrame* encoderInputFrame = enc.GetNextInputFrame();
//                     NV_ENC_INPUT_RESOURCE_OPENGL_TEX *pResource = (NV_ENC_INPUT_RESOURCE_OPENGL_TEX *)encoderInputFrame->inputPtr;
// // uint32_t width, uint32_t height, uint32_t color_format, uint32_t target, void * pixmap
//                    gl_egl_create_texture_from_pixmap(pResource, m_win_width, m_win_height, GL_BGRA, GL_TEXTURE_2D, m_win_pixmap);
//                     //  glBindTexture(pResource->target, pResource->texture);
//                     //  glTexSubImage2D(pResource->target, 0, 0, 0, nWidth, nHeight * 3/2, GL_RED, GL_UNSIGNED_BYTE, i420_buffer_->DataY());
//                     //  glBindTexture(pResource->target, 0);
                    
                    
//                     std::vector<std::vector<uint8_t>> vPacket;
//                     enc.EncodeFrame(vPacket);
//                     NORMAL_EX_LOG("vPacket.size() = %u", vPacket.size());
//                         nFrame += (int)vPacket.size();
//                     for (std::vector<uint8_t> &packet : vPacket)
//                     {
//                         fpOut.write(reinterpret_cast<char*>(packet.data()), packet.size());
//                         // fpOut.out.fflush();
//                     }

                    /////////////////////////////////////////

                    free(gi_reply);

                     

                }
                else
                {
                    WARNING_EX_LOG("gi reply failed !!!");
                }
            }
            if (!m_stoped)
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
         pid_t pid =   getpid();
        if (g_cfg.get_uint32(ECI_Pid) != 0)
        {
                pid = g_cfg.get_uint32(ECI_Pid);
        }
        NORMAL_EX_LOG("--->   pid = %u", pid);
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
                s_input_device.set_point(m_win_width, m_win_height);
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

    bool clinux_capture::_check_xcomp_window_exists()
    {
        xcb_generic_error_t *err = NULL;
        xcb_get_window_attributes_cookie_t attr_cookie = xcb_get_window_attributes(m_connection_ptr, m_win);
        xcb_get_window_attributes_reply_t *attr = xcb_get_window_attributes_reply(m_connection_ptr, attr_cookie, &err);

        bool exists = err == NULL && attr->map_state == XCB_MAP_STATE_VIEWABLE;
        free(attr);
        return exists;

        //return true;
    }

    bool clinux_capture::_watcher_process(xcb_generic_event_t *ev)
    {
        if (!ev)
		{
            return false;
        }

	 
        xcb_window_t win = 0;

        switch (ev->response_type & ~0x80) {
        case XCB_CONFIGURE_NOTIFY:
            win = ((xcb_configure_notify_event_t *)ev)->event;
            break;
        case XCB_MAP_NOTIFY:
            win = ((xcb_map_notify_event_t *)ev)->event;
            break;
        case XCB_EXPOSE:
            win = ((xcb_expose_event_t *)ev)->window;
            break;
        case XCB_VISIBILITY_NOTIFY:
            win = ((xcb_visibility_notify_event_t *)ev)->window;
            break;
        case XCB_DESTROY_NOTIFY:
            win = ((xcb_destroy_notify_event_t *)ev)->event;
            break;
        }; 


        return win == m_win;
    }
}
#endif