
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/composite.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xdefs.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <pthread.h>


#include <X11/Xlib.h>


int show_all_window_name()
{
    // 获取根窗口
    Display* display = XOpenDisplay(NULL);
    auto root_window = DefaultRootWindow(display);

    Window root_return, parent_return;
    Window * child_list = NULL;
    unsigned int child_num = 0;
    // 函数详细说明见xlib文档：https://tronche.com/gui/x/xlib/window-information/XQueryTree.html
    // 该函数会返回父窗口的子窗口列表child_list，因为这里用的是当前桌面的根窗口作为父窗口，所以会返回所有子窗口
    // 注意：窗口顺序(z-order)为自底向上
    XQueryTree(display, root_window, &root_return, &parent_return, &child_list, &child_num);

    for(unsigned int i = 0; i < child_num; ++i) {
        XWindowAttributes attrs;
        // 获取窗口的一些参数
        XGetWindowAttributes(display, child_list[i], &attrs);

        // map_state 可以判断窗口是否可见
        // 三个状态：IsUnmapped, IsUnviewable, IsViewable
        // IsViewable 即为可见窗口
        // 注意：窗口被其他窗口完全遮挡看不到也是 IsViewable ，可以用窗口z-order结合位置尺寸判断这种情况
        if (attrs.map_state == 2)
        { // IsViewable
            char* buffer = NULL;
            // 获取窗口名字，获取成功后buffer才会有值
            XFetchName(display, child_list[i], &buffer);

            // 打印窗口的位置、尺寸和名字
            printf("#[ i = %u ][attrs.x = %u, attrs.y = %u][attr.width = %u, attr.height = %u][window_name = %s]\n", i, attrs.x, attrs.y, attrs.width, attrs.height, buffer);
//            std::cout << "#" << i <<": "<< "(" << attrs.x << ", " << attrs.y << ", " << attrs.width << "x" << attrs.height << ") \"" << (buffer ? buffer : "") << "\"" << std::endl;
            // 注意释放
            XFree(buffer);
        }
    }

    // 注意释放资源
    XFree(child_list);
    XCloseDisplay(display);

    return 0;
}


//////////////////////////////////////////////////////////////////////
Display * disp = NULL;
xcb_connection_t * conn = NULL;


/////////////////////////////////////////////////////////////////////


// Atoms used throughout our plugin
xcb_atom_t ATOM_UTF8_STRING;
xcb_atom_t ATOM_STRING;
xcb_atom_t ATOM_TEXT;
xcb_atom_t ATOM_COMPOUND_TEXT;
xcb_atom_t ATOM_WM_NAME;
xcb_atom_t ATOM_WM_CLASS;
xcb_atom_t ATOM__NET_WM_NAME;
xcb_atom_t ATOM__NET_SUPPORTING_WM_CHECK;
xcb_atom_t ATOM__NET_CLIENT_LIST;




xcb_screen_t *xcb_get_screen(xcb_connection_t *xcb, int screen)
{
    xcb_screen_iterator_t iter;

    iter = xcb_setup_roots_iterator(xcb_get_setup(xcb));
    for (; iter.rem; --screen, xcb_screen_next(&iter))
    {
        if (screen == 0)
        {
            return iter.data;
        }
    }

    return NULL;
}



xcb_get_property_reply_t *xcomp_property_sync(xcb_connection_t *conn,
                                              xcb_window_t win, xcb_atom_t atom)
{
    if (atom == XCB_ATOM_NONE)
        return NULL;

    xcb_generic_error_t *err = NULL;
    // Read properties up to 4096*4 bytes
    xcb_get_property_cookie_t prop_cookie =
            xcb_get_property(conn, 0, win, atom, 0, 0, 4096);
    xcb_get_property_reply_t *prop =
            xcb_get_property_reply(conn, prop_cookie, &err);
    if (err != NULL || xcb_get_property_value_length(prop) == 0) {
        free(prop);
        return NULL;
    }

    return prop;
}

// Specification for checking for ewmh support at
// http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#idm140200472693600
bool xcomp_check_ewmh(xcb_connection_t *conn, xcb_window_t root)
{
    xcb_get_property_reply_t *check = xcomp_property_sync(conn, root, ATOM__NET_SUPPORTING_WM_CHECK);
    if (!check)
    {

        printf("[%s][%s][%d][ATOM__NET_SUPPORTING_WM_CHECK = %d]\n", __FILE__, __FUNCTION__ , __LINE__, ATOM__NET_SUPPORTING_WM_CHECK);
        return false;
    }

    xcb_window_t ewmh_window = ((xcb_window_t *)xcb_get_property_value(check))[0];
    free(check);

    xcb_get_property_reply_t *check2 = xcomp_property_sync( conn, ewmh_window, ATOM__NET_SUPPORTING_WM_CHECK);
    if (!check2)
    {
        printf("[%s][%s][%d] xcomp_property_sync\n", __FILE__, __FUNCTION__ , __LINE__);
        return false;
    }
    free(check2);

    return true;
}



xcb_atom_t get_atom(xcb_connection_t *conn, const char *name)
{
    xcb_intern_atom_cookie_t atom_c =
            xcb_intern_atom(conn, 1, strlen(name), name);
    xcb_intern_atom_reply_t *atom_r =
            xcb_intern_atom_reply(conn, atom_c, NULL);
    xcb_atom_t a = atom_r->atom;
    free(atom_r);
    return a;
}
/**
 *
 */
void init_xcomposite()
{
//    ATOM__NET_SUPPORTING_WM_CHECK = XInternAtom(disp, "_NET_SUPPORTING_WM_CHECK", True);

    ATOM_UTF8_STRING = get_atom(conn, "UTF8_STRING");
    ATOM_STRING = get_atom(conn, "STRING");
    ATOM_TEXT = get_atom(conn, "TEXT");
    ATOM_COMPOUND_TEXT = get_atom(conn, "COMPOUND_TEXT");
    ATOM_WM_NAME = get_atom(conn, "WM_NAME");
    ATOM_WM_CLASS = get_atom(conn, "WM_CLASS");
    ATOM__NET_WM_NAME = get_atom(conn, "_NET_WM_NAME");
    ATOM__NET_SUPPORTING_WM_CHECK = get_atom(conn, "_NET_SUPPORTING_WM_CHECK");
    ATOM__NET_CLIENT_LIST = get_atom(conn, "_NET_CLIENT_LIST");

    xcb_atom_t _atomPID = XInternAtom(disp, "_NET_WM_PID", True);
    printf("_atomPID = %d\n", _atomPID);


    Atom type;

    int format;

    unsigned long nItems;

    unsigned long bytesAfter;

    unsigned char *propPID = 0;
    if (Success == XGetWindowProperty(disp, XDefaultRootWindow(disp), _atomPID, 0, 1, False, XA_CARDINAL, &type, &format, &nItems, &bytesAfter, &propPID))
    {
        if (0 != propPID)
        {
            printf("propPID = %s\n", propPID);
        }
    }
}


void xcomposite_load(void)
{
    disp = XOpenDisplay(NULL);
    conn = XGetXCBConnection(disp);
    if (xcb_connection_has_error(conn)) {
       printf("failed opening display\n");
        return;
    }

    const xcb_query_extension_reply_t *xcomp_ext =
            xcb_get_extension_data(conn, &xcb_composite_id);
    if (!xcomp_ext->present) {
       printf("Xcomposite extension not supported\n");
        return;
    }

    xcb_composite_query_version_cookie_t version_cookie =
            xcb_composite_query_version(conn, 0, 2);
    xcb_composite_query_version_reply_t *version =
            xcb_composite_query_version_reply(conn, version_cookie, NULL);
    if (version->major_version == 0 && version->minor_version < 2) {
        printf("Xcomposite extension is too old: %d.%d < 0.2\n",
             version->major_version, version->minor_version);
        free(version);
        return;
    }
    free(version);

    // Must be done before other helpers called.
//    xcomp_gather_atoms(conn);
    init_xcomposite();
    xcb_screen_t *screen_default = xcb_get_screen(conn, DefaultScreen(disp));
    if (!screen_default)
    {
        printf("[%s][%s][%d] not get default screen \n", __FILE__, __FUNCTION__ , __LINE__);
        return ;
    }
    if (  !xcomp_check_ewmh(conn, screen_default->root))
    {
        printf("window manager does not support Extended Window Manager Hints (EWMH).\nXComposite capture disabled.\n");
        return;
    }

//    struct obs_source_info sinfo = {
//            .id = "xcomposite_input",
//            .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW |
//                            OBS_SOURCE_DO_NOT_DUPLICATE,
//            .get_name = xcompcap_getname,
//            .create = xcompcap_create,
//            .destroy = xcompcap_destroy,
//            .get_properties = xcompcap_props,
//            .get_defaults = xcompcap_defaults,
//            .update = xcompcap_update,
//            .video_tick = xcompcap_video_tick,
//            .video_render = xcompcap_video_render,
//            .get_width = xcompcap_get_width,
//            .get_height = xcompcap_get_height,
//            .icon_type = OBS_ICON_TYPE_WINDOW_CAPTURE,
//    };

//    obs_register_source(&sinfo);
}



void  xcomp_top_level_windows(xcb_connection_t *conn)
{
//    DARRAY(xcb_window_t) res = {0};

    // EWMH top level window listing is not supported.
    if (ATOM__NET_CLIENT_LIST == XCB_ATOM_NONE)
    {
        return  ;
    }

    xcb_screen_iterator_t screen_iter =  xcb_setup_roots_iterator(xcb_get_setup(conn));
    for (; screen_iter.rem > 0; xcb_screen_next(&screen_iter))
    {
        xcb_generic_error_t *err = NULL;
        // Read properties up to 4096*4 bytes
        xcb_get_property_cookie_t cl_list_cookie = xcb_get_property(conn, 0, screen_iter.data->root,
                                 ATOM__NET_CLIENT_LIST, 0, 0, 4096);
        xcb_get_property_reply_t *cl_list = xcb_get_property_reply(conn, cl_list_cookie, &err);
        if (err != NULL) {
            goto done;
        }

        uint32_t len = xcb_get_property_value_length(cl_list) / sizeof(xcb_window_t);
        for (uint32_t i = 0; i < len; i++)
        {
            printf("[i = %u] void = %s\n", i, xcb_get_property_value( cl_list));
           // da_push_back(res, &(((xcb_window_t *)xcb_get_property_value( cl_list))[i]));
        }

        done:
        free(cl_list);
    }
//return NULL;
   // return res.da;
}



int main(int argc, char argv[])
{

//    show_all_window_name();
//
    xcomposite_load();
    xcomp_top_level_windows(conn);
    while (1)
    {
        usleep(10);
    }
    return EXIT_SUCCESS;
}