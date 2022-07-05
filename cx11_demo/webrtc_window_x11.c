///***********************************************************************************************
//created: 		2022-07-05
//
//author:			chensong
//
//purpose:		webrtc window xll api
//************************************************************************************************/
//
//
//
//#include <X11/Xutil.h>
//#include <stdint.h>
//#include <string.h>
//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <X11/Xlib.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <X11/X.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <X11/extensions/Xcomposite.h>
//#include <X11/extensions/composite.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>
//#include <assert.h>
//#include <stdint.h>
//#define bool int
//#define true (1)
//#define false (0)
//
//// TODO(sergeyu): This code is not thread safe. Fix it. Bug 2202.
//static bool g_xserver_error_trap_enabled = false;
//static int g_last_xserver_error_code = 0;
//XErrorHandler original_error_handler_ = NULL;
//
//
//int XServerErrorHandler(Display* display, XErrorEvent* error_event)
//{
//    assert(g_xserver_error_trap_enabled);
//    g_last_xserver_error_code = error_event->error_code;
//    return 0;
//}
//
//void XServerGetErrorHandler(Display* display)
//{
//    original_error_handler_ = XSetErrorHandler(&XServerErrorHandler);
//    g_xserver_error_trap_enabled = true;
//    g_last_xserver_error_code = 0;
//}
//int getLastErrorAndDisable()
//{
//
//
//    assert(g_xserver_error_trap_enabled);
//    XSetErrorHandler(original_error_handler_);
//    g_xserver_error_trap_enabled = false;
//    return g_last_xserver_error_code;
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////int32_t GetWindowState(XAtomCache* cache, Window window)
////{
////    // Get WM_STATE property of the window.
////    XWindowProperty<uint32_t> window_state(cache->display(), window,
////                                           cache->WmState());
////
////    // WM_STATE is considered to be set to WithdrawnState when it missing.
////    return window_state.is_valid() ? *window_state.data() : WithdrawnState;
////}
//
//
//// Iterates through |window| hierarchy to find first visible window, i.e. one
//// that has WM_STATE property set to NormalState.
//// See http://tronche.com/gui/x/icccm/sec-4.html#s-4.1.3.1 .
//// Window GetApplicationWindow(XAtomCache* cache, Window window) {
////    int32_t state = GetWindowState(cache, window);
////    if (state == NormalState) {
////        // Window has WM_STATE==NormalState. Return it.
////        return window;
////    } else if (state == IconicState) {
////        // Window is in minimized. Skip it.
////        return 0;
////    }
////
////    RTC_DCHECK_EQ(state, WithdrawnState);
////    // If the window is in WithdrawnState then look at all of its children.
////    ::Window root, parent;
////    ::Window* children;
////    unsigned int num_children;
////    if (!XQueryTree(cache->display(), window, &root, &parent, &children,
////                    &num_children)) {
////        RTC_LOG(LS_ERROR) << "Failed to query for child windows although window"
////                          << "does not have a valid WM_STATE.";
////        return 0;
////    }
////    ::Window app_window = 0;
////    for (unsigned int i = 0; i < num_children; ++i) {
////        app_window = GetApplicationWindow(cache, children[i]);
////        if (app_window)
////            break;
////    }
////
////    if (children)
////        XFree(children);
////    return app_window;
////}
//
//
///////////////////////////////////////////////////////////////////////////
//bool GetWindowRect(Display* display, Window window,   XWindowAttributes* attributes /* = nullptr */)
//{
//    XWindowAttributes local_attributes;
//    int offset_x;
//    int offset_y;
//    if (attributes == NULL)
//    {
//        attributes = &local_attributes;
//    }
//
//    {
//        XServerGetErrorHandler(display);
//        if (!XGetWindowAttributes(display, window, attributes) ||
//                getLastErrorAndDisable() != 0)
//        {
//            return false;
//        }
//    }
//
//
//    {
//       // XErrorTrap error_trap(display);
//        Window child;
//        if (!XTranslateCoordinates(display, window, attributes->root, -attributes->width,
//                                   -attributes->height, &offset_x, &offset_y, &child) /*||
//            error_trap.GetLastErrorAndDisable() != 0*/) {
//            return false;
//        }
//    }
//
//    return true;
//}
//
//
//int main(int argc, char *argv[])
//{
//    Display* display = XOpenDisplay(  NULL  );
//    if (!display)
//    {
//        printf( "Unable to open display\n");
//        return NULL;
//    }
//
//
//    int event_base, error_base, major_version, minor_version;
//    if (XCompositeQueryExtension(display, &event_base, &error_base) &&
//        XCompositeQueryVersion(display, &major_version, &minor_version) &&
//        // XCompositeNameWindowPixmap() requires version 0.2
//        (major_version > 0 || minor_version >= 2))
//    {
//
//    }
//    else
//    {
//        printf( "Xcomposite extension not available or too old.\n");
//    }
//
//
//    // all window info
////     Display* const display_ptr = display;
////
////    int failed_screens = 0;
////    const int num_screens = XScreenCount(display);
////    for (int screen = 0; screen < num_screens; ++screen)
////    {
////         Window root_window = XRootWindow(display, screen);
////         Window parent;
////         Window* children;
////        unsigned int num_children;
////        {
//////            XErrorTrap error_trap(display);
////
////            original_error_handler_ = XSetErrorHandler(&XServerErrorHandler);
////            g_xserver_error_trap_enabled = true;
////            g_last_xserver_error_code = 0;
////            if (XQueryTree(display, root_window, &root_window, &parent, &children,
////                           &num_children) == 0 ||
////                    getLastErrorAndDisable() != 0)
////            {
////                failed_screens++;
////               printf("Failed to query for child windows for screen = %u\n" , screen);
////                continue;
////            }
////        }
////
//////        DeferXFree free_children(children);
////
////        for (unsigned int i = 0; i < num_children; ++i)
////        {
////            // Iterates in reverse order to return windows from front to back.
////             Window app_window = GetApplicationWindow(cache, children[num_children - 1 - i]);
////            if (app_window && !IsDesktopElement(cache, app_window))
////            {
////                if (!on_window(app_window))
////                {
////                    XFree(children);
////                    return true;
////                }
////            }
////        }
////        XFree(children);
////    }
////
////    return failed_screens < num_screens;
//
//
//    //////////////////////////////// capture init /////////////////////////////////
//
//    XWindowAttributes attributes;
//    int window = 0;
//    if (!GetWindowRect(display, window,   &attributes))
//    {
//        return false;
//    }
//
//
//    Visual* default_visual = attributes.visual;
//    int default_depth = attributes.depth;
//
//    int major, minor;
//    Bool have_pixmaps;
//    if (!XShmQueryVersion(display, &major, &minor, &have_pixmaps))
//    {
//        // Shared memory not supported. CaptureRect will use the XImage API instead.
//        return;
//    }
//
//    bool using_shm = false;
//    shm_segment_info_ = new XShmSegmentInfo;
//    shm_segment_info_->shmid = -1;
//    shm_segment_info_->shmaddr = nullptr;
//    shm_segment_info_->readOnly = False;
//    x_shm_image_ = XShmCreateImage(display_, default_visual, default_depth,
//                                   ZPixmap, 0, shm_segment_info_,
//                                   window_rect_.width(), window_rect_.height());
//    if (x_shm_image_) {
//        shm_segment_info_->shmid =
//                shmget(IPC_PRIVATE, x_shm_image_->bytes_per_line * x_shm_image_->height,
//                       IPC_CREAT | 0600);
//        if (shm_segment_info_->shmid != -1) {
//            void* shmat_result = shmat(shm_segment_info_->shmid, 0, 0);
//            if (shmat_result != reinterpret_cast<void*>(-1)) {
//                shm_segment_info_->shmaddr = reinterpret_cast<char*>(shmat_result);
//                x_shm_image_->data = shm_segment_info_->shmaddr;
//
//                XErrorTrap error_trap(display_);
//                using_shm = XShmAttach(display_, shm_segment_info_);
//                XSync(display_, False);
//                if (error_trap.GetLastErrorAndDisable() != 0)
//                    using_shm = false;
//                if (using_shm) {
//                    RTC_LOG(LS_VERBOSE)
//                            << "Using X shared memory segment " << shm_segment_info_->shmid;
//                }
//            }
//        } else {
//            RTC_LOG(LS_WARNING) << "Failed to get shared memory segment. "
//                                   "Performance may be degraded.";
//        }
//    }
//
//    if (!using_shm) {
//        RTC_LOG(LS_WARNING)
//                << "Not using shared memory. Performance may be degraded.";
//        ReleaseSharedMemorySegment();
//        return;
//    }
//
//    if (have_pixmaps)
//        have_pixmaps = InitPixmaps(default_depth);
//
//    shmctl(shm_segment_info_->shmid, IPC_RMID, 0);
//    shm_segment_info_->shmid = -1;
//
//    RTC_LOG(LS_VERBOSE) << "Using X shared memory extension v" << major << "."
//                        << minor << " with" << (have_pixmaps ? "" : "out")
//                        << " pixmaps.";
//
//
//    return 0;
//}