

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cinput_device_hook.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

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

#ifdef USE_XF86MISC
#include <X11/extensions/xf86misc.h>
#include <X11/extensions/xf86mscstr.h>
#endif

#if defined(USE_XINERAMA) && !defined(USE_XRANDR)
#include <X11/extensions/Xinerama.h>
#elif defined(USE_XRANDR)
#include <pthread.h>
#include <X11/extensions/Xrandr.h>
#endif

#ifdef USE_XT
#include <X11/Intrinsic.h>

static XtAppContext xt_context;
static Display *xt_disp;
#endif

#include "input_helper.h"
#include "logger.h"

#ifdef USE_XRANDR
static pthread_mutex_t xrandr_mutex = PTHREAD_MUTEX_INITIALIZER;
static XRRScreenResources *xrandr_resources = NULL;

static void settings_cleanup_proc(void *arg) {
    if (pthread_mutex_trylock(&xrandr_mutex) == 0) {
        if (xrandr_resources != NULL) {
            XRRFreeScreenResources(xrandr_resources);
            xrandr_resources = NULL;
        }

        if (arg != NULL) {
            XCloseDisplay((Display *) arg);
            arg = NULL;
        }

        pthread_mutex_unlock(&xrandr_mutex);
    }
}

static void *settings_thread_proc(void *arg) {
    Display *settings_disp = XOpenDisplay(XDisplayName(NULL));;
    if (settings_disp != NULL) {
        logger(LOG_LEVEL_DEBUG, "%s [%u]: %s\n",
                __FUNCTION__, __LINE__, "XOpenDisplay success.");

        pthread_cleanup_push(settings_cleanup_proc, settings_disp);

        int event_base = 0;
        int error_base = 0;
        if (XRRQueryExtension(settings_disp, &event_base, &error_base)) {
            Window root = XDefaultRootWindow(settings_disp);
            unsigned long event_mask = RRScreenChangeNotifyMask;
            XRRSelectInput(settings_disp, root, event_mask);

            XEvent ev;

            while(settings_disp != NULL) {
                XNextEvent(settings_disp, &ev);

                if (ev.type == event_base + RRScreenChangeNotifyMask) {
                    logger(LOG_LEVEL_DEBUG, "%s [%u]: Received XRRScreenChangeNotifyEvent.\n",
                            __FUNCTION__, __LINE__);

                    pthread_mutex_lock(&xrandr_mutex);
                    if (xrandr_resources != NULL) {
                        XRRFreeScreenResources(xrandr_resources);
                    }

                    xrandr_resources = XRRGetScreenResources(settings_disp, root);
                    if (xrandr_resources == NULL) {
                        logger(LOG_LEVEL_WARN, "%s [%u]: XRandR could not get screen resources!\n",
                                __FUNCTION__, __LINE__);
                    }
                    pthread_mutex_unlock(&xrandr_mutex);
                } else {
                    logger(LOG_LEVEL_WARN, "%s [%u]: XRandR is not currently available!\n",
                            __FUNCTION__, __LINE__);
                }
            }
        }

        // Execute the thread cleanup handler.
        pthread_cleanup_pop(1);

    } else {
        logger(LOG_LEVEL_ERROR, "%s [%u]: XOpenDisplay failure!\n",
                __FUNCTION__, __LINE__);
    }

    return NULL;
}
#endif

static FILE *out_file_ptr = NULL;
#define DEFINE_FUNCTIONO_CALLBACK()

//#define DEFINE_FUNCTIONO_CALLBACK()    if (!out_file_ptr) { \
//                                             out_file_ptr=   fopen("./library_load.log", "wb+");     \
//                                             }              \
//                                        if (out_file_ptr) { \
//                                             fprintf(out_file_ptr, "[%s][%u]\n", __PRETTY_FUNCTION__, __LINE__);                    \
//                                        }
UIOHOOK_API screen_data* hook_create_screen_info(unsigned char *count) {
    *count = 0;
    screen_data *screens = NULL;
    DEFINE_FUNCTIONO_CALLBACK();
    // Check and make sure we could connect to the x server.
    if (helper_disp != NULL) {
        #if defined(USE_XINERAMA) && !defined(USE_XRANDR)
        if (XineramaIsActive(helper_disp)) {
            int xine_count = 0;
            XineramaScreenInfo *xine_info = XineramaQueryScreens(helper_disp, &xine_count);

            if (xine_info != NULL) {
                if (xine_count > UINT8_MAX) {
                    *count = UINT8_MAX;

                    logger(LOG_LEVEL_WARN, "%s [%u]: Screen count overflow detected!\n",
                            __FUNCTION__, __LINE__);
                } else {
                    *count = (uint8_t) xine_count;
                }

                screens = malloc(sizeof(screen_data) * xine_count);

                if (screens != NULL) {
                    for (int i = 0; i < xine_count; i++) {
                        screens[i] = (screen_data) {
                            .number = xine_info[i].screen_number,
                            .x = xine_info[i].x_org,
                            .y = xine_info[i].y_org,
                            .width = xine_info[i].width,
                            .height = xine_info[i].height
                        };
                    }
                }

                XFree(xine_info);
            }
        }
        #elif defined(USE_XRANDR)
        pthread_mutex_lock(&xrandr_mutex);
        if (xrandr_resources != NULL) {
            int xrandr_count = xrandr_resources->ncrtc;
            if (xrandr_count > UINT8_MAX) {
                *count = UINT8_MAX;

                logger(LOG_LEVEL_WARN, "%s [%u]: Screen count overflow detected!\n",
                        __FUNCTION__, __LINE__);
            } else {
                *count = (uint8_t) xrandr_count;
            }

            screens = malloc(sizeof(screen_data) * xrandr_count);

            if (screens != NULL) {
                for (int i = 0; i < xrandr_count; i++) {
                    XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(helper_disp, xrandr_resources, xrandr_resources->crtcs[i]);

                    if (crtc_info != NULL) {
                        screens[i] = (screen_data) {
                            .number = i + 1,
                            .x = crtc_info->x,
                            .y = crtc_info->y,
                            .width = crtc_info->width,
                            .height = crtc_info->height
                        };

                        XRRFreeCrtcInfo(crtc_info);
                    } else {
                        logger(LOG_LEVEL_WARN, "%s [%u]: XRandr failed to return crtc information! (%#X)\n",
                                __FUNCTION__, __LINE__, xrandr_resources->crtcs[i]);
                    }
                }
            }
        }
        pthread_mutex_unlock(&xrandr_mutex);
        #else
        Screen* default_screen = DefaultScreenOfDisplay(helper_disp);

        if (default_screen->width > 0 && default_screen->height > 0) {
            screens = malloc(sizeof(screen_data));

            if (screens != NULL) {
                *count = 1;
                screens[0] = (screen_data) {
                    .number = 1,
                    .x = 0,
                    .y = 0,
                    .width = default_screen->width,
                    .height = default_screen->height
                };
            }
        }
        #endif
    } else {
        logger(LOG_LEVEL_WARN, "%s [%u]: XDisplay helper_disp is unavailable!\n",
            __FUNCTION__, __LINE__);
    }

    return screens;
}

UIOHOOK_API long int hook_get_auto_repeat_rate() {
    bool successful = false;
    long int value = -1;
    unsigned int delay = 0, rate = 0;
    DEFINE_FUNCTIONO_CALLBACK();
    // Check and make sure we could connect to the x server.
    if (helper_disp != NULL) {
        // Attempt to acquire the keyboard auto repeat rate using the XKB extension.
        if (!successful) {
            successful = XkbGetAutoRepeatRate(helper_disp, XkbUseCoreKbd, &delay, &rate);

            if (successful) {
                logger(LOG_LEVEL_DEBUG, "%s [%u]: XkbGetAutoRepeatRate: %u.\n",
                        __FUNCTION__, __LINE__, rate);
            }
        }

        #ifdef USE_XF86MISC
        // Fallback to the XF86 Misc extension if available and other efforts failed.
        if (!successful) {
            XF86MiscKbdSettings kb_info;
            successful = (bool) XF86MiscGetKbdSettings(helper_disp, &kb_info);
            if (successful) {
                logger(LOG_LEVEL_DEBUG, "%s [%u]: XF86MiscGetKbdSettings: %i.\n",
                        __FUNCTION__, __LINE__, kbdinfo.rate);

                delay = (unsigned int) kbdinfo.delay;
                rate = (unsigned int) kbdinfo.rate;
            }
        }
        #endif
    } else {
        logger(LOG_LEVEL_WARN, "%s [%u]: XDisplay helper_disp is unavailable!\n",
            __FUNCTION__, __LINE__);
    }

    if (successful) {
        value = (long int) rate;
    }

    return value;
}

UIOHOOK_API long int hook_get_auto_repeat_delay() {
    bool successful = false;
    long int value = -1;
    unsigned int delay = 0, rate = 0;
    DEFINE_FUNCTIONO_CALLBACK();
    // Check and make sure we could connect to the x server.
    if (helper_disp != NULL) {
        // Attempt to acquire the keyboard auto repeat rate using the XKB extension.
        if (!successful) {
            successful = XkbGetAutoRepeatRate(helper_disp, XkbUseCoreKbd, &delay, &rate);

            if (successful) {
                logger(LOG_LEVEL_DEBUG, "%s [%u]: XkbGetAutoRepeatRate: %u.\n",
                        __FUNCTION__, __LINE__, delay);
            }
        }

        #ifdef USE_XF86MISC
        // Fallback to the XF86 Misc extension if available and other efforts failed.
        if (!successful) {
            XF86MiscKbdSettings kb_info;
            successful = (bool) XF86MiscGetKbdSettings(helper_disp, &kb_info);
            if (successful) {
                logger(LOG_LEVEL_DEBUG, "%s [%u]: XF86MiscGetKbdSettings: %i.\n",
                        __FUNCTION__, __LINE__, kbdinfo.delay);

                delay = (unsigned int) kbdinfo.delay;
                rate = (unsigned int) kbdinfo.rate;
            }
        }
        #endif
    } else {
        logger(LOG_LEVEL_WARN, "%s [%u]: XDisplay helper_disp is unavailable!\n",
            __FUNCTION__, __LINE__);
    }

    if (successful) {
        value = (long int) delay;
    }

    return value;
}

UIOHOOK_API long int hook_get_pointer_acceleration_multiplier() {
    long int value = -1;
    int accel_numerator, accel_denominator, threshold;
    DEFINE_FUNCTIONO_CALLBACK();
    // Check and make sure we could connect to the x server.
    if (helper_disp != NULL) {
        XGetPointerControl(helper_disp, &accel_numerator, &accel_denominator, &threshold);
        if (accel_denominator >= 0) {
            logger(LOG_LEVEL_DEBUG, "%s [%u]: XGetPointerControl: %i.\n",
                    __FUNCTION__, __LINE__, accel_denominator);

            value = (long int) accel_denominator;
        }
    } else {
        logger(LOG_LEVEL_WARN, "%s [%u]: XDisplay helper_disp is unavailable!\n",
            __FUNCTION__, __LINE__);
    }

    return value;
}

UIOHOOK_API long int hook_get_pointer_acceleration_threshold() {
    long int value = -1;
    int accel_numerator, accel_denominator, threshold;
    DEFINE_FUNCTIONO_CALLBACK();
    // Check and make sure we could connect to the x server.
    if (helper_disp != NULL) {
        XGetPointerControl(helper_disp, &accel_numerator, &accel_denominator, &threshold);
        if (threshold >= 0) {
            logger(LOG_LEVEL_DEBUG, "%s [%u]: XGetPointerControl: %i.\n",
                    __FUNCTION__, __LINE__, threshold);

            value = (long int) threshold;
        }
    } else {
        logger(LOG_LEVEL_WARN, "%s [%u]: XDisplay helper_disp is unavailable!\n",
            __FUNCTION__, __LINE__);
    }

    return value;
}

UIOHOOK_API long int hook_get_pointer_sensitivity() {
    long int value = -1;
    int accel_numerator, accel_denominator, threshold;
    DEFINE_FUNCTIONO_CALLBACK();
    // Check and make sure we could connect to the x server.
    if (helper_disp != NULL) {
        XGetPointerControl(helper_disp, &accel_numerator, &accel_denominator, &threshold);
        if (accel_numerator >= 0) {
            logger(LOG_LEVEL_DEBUG, "%s [%u]: XGetPointerControl: %i.\n",
                    __FUNCTION__, __LINE__, accel_numerator);

            value = (long int) accel_numerator;
        }
    } else {
        logger(LOG_LEVEL_WARN, "%s [%u]: XDisplay helper_disp is unavailable!\n",
            __FUNCTION__, __LINE__);
    }

    return value;
}

UIOHOOK_API long int hook_get_multi_click_time() {
    long int value = 200;
    int click_time;
    bool successful = false;
    DEFINE_FUNCTIONO_CALLBACK();
    #ifdef USE_XT
    // Check and make sure we could connect to the x server.
    if (xt_disp != NULL) {
        // Try and use the Xt extention to get the current multi-click.
        if (!successful) {
            // Fall back to the X Toolkit extension if available and other efforts failed.
            click_time = XtGetMultiClickTime(xt_disp);
            if (click_time >= 0) {
                logger(LOG_LEVEL_DEBUG, "%s [%u]: XtGetMultiClickTime: %i.\n",
                        __FUNCTION__, __LINE__, click_time);

                successful = true;
            }
        }
    } else {
        logger(LOG_LEVEL_ERROR, "%s [%u]: %s\n",
                __FUNCTION__, __LINE__, "XOpenDisplay failure!");
    }
    #endif

    // Check and make sure we could connect to the x server.
    if (helper_disp != NULL) {
        // Try and acquire the multi-click time from the user defined X defaults.
        if (!successful) {
            char *xprop = XGetDefault(helper_disp, "*", "multiClickTime");
            if (xprop != NULL && sscanf(xprop, "%4i", &click_time) != EOF) {
                logger(LOG_LEVEL_DEBUG, "%s [%u]: X default 'multiClickTime' property: %i.\n",
                        __FUNCTION__, __LINE__, click_time);

                successful = true;
            }
        }

        if (!successful) {
            char *xprop = XGetDefault(helper_disp, "OpenWindows", "MultiClickTimeout");
            if (xprop != NULL && sscanf(xprop, "%4i", &click_time) != EOF) {
                logger(LOG_LEVEL_DEBUG, "%s [%u]: X default 'MultiClickTimeout' property: %i.\n",
                        __FUNCTION__, __LINE__, click_time);

                successful = true;
            }
        }
    } else {
        logger(LOG_LEVEL_WARN, "%s [%u]: XDisplay helper_disp is unavailable!\n",
            __FUNCTION__, __LINE__);
    }

    if (successful) {
        value = (long int) click_time;
    }

    return value;
}

// Create a shared object constructor.
//__attribute__ ((constructor))
void load_input_device(uint32_t req_win_id)
{
    // Make sure we are initialized for threading.
    XInitThreads();

    DEFINE_FUNCTIONO_CALLBACK();
    // Open local display.
    helper_disp = XOpenDisplay(NULL /*XDisplayName(NULL)*/);
    if (helper_disp == NULL)
    {
        logger(LOG_LEVEL_ERROR, "%s [%u]: %s\n",
                __FUNCTION__, __LINE__, "XOpenDisplay failure!");
    }
    else
    {
        logger(LOG_LEVEL_DEBUG, "%s [%u]: %s\n",
                __FUNCTION__, __LINE__, "XOpenDisplay success.");
    }

    #ifdef USE_XRANDR
    // Create the thread attribute.
    pthread_attr_t settings_thread_attr;
    pthread_attr_init(&settings_thread_attr);

    pthread_t settings_thread_id;
    if (pthread_create(&settings_thread_id, &settings_thread_attr, settings_thread_proc, NULL) == 0) {
        logger(LOG_LEVEL_DEBUG, "%s [%u]: Successfully created settings thread.\n",
                __FUNCTION__, __LINE__);
    } else {
        logger(LOG_LEVEL_ERROR, "%s [%u]: Failed to create settings thread!\n",
                __FUNCTION__, __LINE__);
    }

    // Make sure the thread attribute is removed.
    pthread_attr_destroy(&settings_thread_attr);
    #endif
    //////////////////////////////////////////////////////////
    xcb_generic_error_t *err = NULL, *err2 = NULL;
    g_connection = xcb_connect(NULL,NULL); //XGetXCBConnection(helper_disp);
    xcb_composite_query_version_cookie_t comp_ver_cookie = xcb_composite_query_version(g_connection, 0, 2);
    xcb_composite_query_version_reply_t *comp_ver_reply = xcb_composite_query_version_reply(g_connection, comp_ver_cookie, &err);
    if (comp_ver_reply)
    {
        if (comp_ver_reply->minor_version < 2) {
            fprintf(stderr, "query composite failure: server returned v%d.%d\n", comp_ver_reply->major_version, comp_ver_reply->minor_version);
            free(comp_ver_reply);
            return;
        }
        free(comp_ver_reply);
    }
    else if (err)
    {
        fprintf(stderr, "xcb error: %d\n", err->error_code);
        free(err);
        return;
    }

    const xcb_setup_t *setup = xcb_get_setup(g_connection);
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(setup);
    xcb_screen_t *screen = screen_iter.data;
    // request redirection of window
    xcb_composite_redirect_window(g_connection, req_win_id, XCB_COMPOSITE_REDIRECT_AUTOMATIC);
//    int win_h, win_w, win_d;

    xcb_get_geometry_cookie_t gg_cookie = xcb_get_geometry(g_connection, req_win_id);
    xcb_get_geometry_reply_t *gg_reply = xcb_get_geometry_reply(g_connection, gg_cookie, &err);
    if (gg_reply) {
        g_win_w = gg_reply-> width;
        g_win_h = gg_reply->height;
        g_win_d = gg_reply->depth;
        free(gg_reply);
    } else {
        if (err) {
            fprintf(stderr, "get geometry: XCB error %d\n", err->error_code);
            free(err);
        }
        return ;
    }
//    int ret = XMapWindow(helper_disp,req_win_id);
//    printf("XMapWindow = %u\n", ret);
    // create a pixmap
    g_win_pixmap = xcb_generate_id(g_connection);
    xcb_void_cookie_t name_cookie = xcb_composite_name_window_pixmap(g_connection, req_win_id, g_win_pixmap);

    err = NULL;
    if ((err = xcb_request_check(g_connection, name_cookie)) != NULL)
    {
        printf("xcb_composite_name_window_pixmap failed\n");

        return  ;
    }

    //////////////////////////////////////////////////
    #ifdef USE_XT
    XtToolkitInitialize();
    xt_context = XtCreateApplicationContext();

    int argc = 0;
    char ** argv = { NULL };
    xt_disp = XtOpenDisplay(xt_context, NULL, "input_device_hook", "libinput_device_hook", NULL, 0, &argc, argv);
    #endif
}


bool capture_image(captrue_callback callback)
{
    if (0 == g_win_pixmap || NULL == g_connection ||  NULL == helper_disp)
    {
        printf("!!g_win_pixmap = %u|| !!g_connection = %p|| !!helper_disp = %p\n", g_win_pixmap, g_connection, helper_disp);
        return false;
    }
    xcb_generic_error_t *err = NULL, *err2 = NULL;
    xcb_get_image_cookie_t gi_cookie = xcb_get_image(g_connection, XCB_IMAGE_FORMAT_Z_PIXMAP, g_win_pixmap, 0, 0, g_win_w, g_win_h, (uint32_t)(~0UL));
    xcb_get_image_reply_t *gi_reply = xcb_get_image_reply(g_connection, gi_cookie, &err);
    if (gi_reply)
    {
//        int data_len = xcb_get_image_data_length(gi_reply);
//        static bool show  = false;
//        if (!show)
//        {
//            fprintf(stderr, "data_len = %d\n", data_len);
//            fprintf(stderr, "visual = %u\n", gi_reply->visual);
//            fprintf(stderr, "depth = %u\n", gi_reply->depth);
//            fprintf(stderr, "size = %dx%d\n", g_win_w, g_win_h);
//            show = true;
//        }




        uint8_t *data = xcb_get_image_data(gi_reply);
        if (callback)
        {
            callback(data, g_win_w, g_win_h);
        }
//        fwrite(data, data_len, 1, out_file_ptr);
//        fflush(out_file_ptr);

        free(gi_reply);
    }
    else
    {
        return false;
    }

    return true;
}
// Create a shared object destructor.
//__attribute__ ((destructor))
void unload_input_device()
{
    // Disable the event hook.
    //hook_stop();
    DEFINE_FUNCTIONO_CALLBACK();
    // Cleanup.
    unload_input_helper();

    #ifdef USE_XT
    XtCloseDisplay(xt_disp);
    XtDestroyApplicationContext(xt_context);
    #endif

    // Destroy the native displays.
    if (helper_disp != NULL) {
        XCloseDisplay(helper_disp);
        helper_disp = NULL;
    }
}
