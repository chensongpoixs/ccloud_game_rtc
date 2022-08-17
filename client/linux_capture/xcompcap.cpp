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
//#define xdisp (Xcompcap::disp())
#define WIN_STRING_DIV "\r\n"
struct WindowInfo {
    std::string lex_comparable;
    std::string name;
    std::string desc;
};

Display  * xdisp = NULL;



bool init()
{
    xdisp = XOpenDisplay(NULL);



    XInitThreads();
    int eventBase, errorBase;
    if (!XCompositeQueryExtension(xdisp, &eventBase, &errorBase))
    {
        printf( "Xcomposite extension not supported\n");
        return false;
    }

    int major = 0, minor = 2;
    XCompositeQueryVersion(xdisp, &major, &minor);

    if (major == 0 && minor < 2)
    {
        printf( "Xcomposite extension is too old: %d.%d < 0.2\n", major, minor);
        return false;
    }

}


// Specification for checking for ewmh support at
// http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#idm140200472693600

bool ewmhIsSupported()
{
    Display *display = xdisp;
    Atom netSupportingWmCheck =
            XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", true);
    Atom actualType;
    int format = 0;
    unsigned long num = 0, bytes = 0;
    unsigned char *data = NULL;
    Window ewmh_window = 0;

    int status = XGetWindowProperty(display, DefaultRootWindow(display),
                                    netSupportingWmCheck, 0L, 1L, false,
                                    XA_WINDOW, &actualType, &format, &num,
                                    &bytes, &data);

    if (status == Success) {
        if (num > 0) {
            ewmh_window = ((Window *)data)[0];
        }
        if (data) {
            XFree(data);
            data = NULL;
        }
    }

    if (ewmh_window) {
        status = XGetWindowProperty(display, ewmh_window,
                                    netSupportingWmCheck, 0L, 1L, false,
                                    XA_WINDOW, &actualType, &format,
                                    &num, &bytes, &data);
        if (status != Success || num == 0 ||
            ewmh_window != ((Window *)data)[0]) {
            ewmh_window = 0;
        }
        if (status == Success && data) {
            XFree(data);
        }
    }

    return ewmh_window != 0;
}


std::list<Window> getTopLevelWindows()
{
    std::list<Window> res;

    if (!ewmhIsSupported()) {
        printf( "Unable to query window list "
                          "because window manager "
                          "does not support extended "
                          "window manager Hints\n");
        return res;
    }

    Atom netClList = XInternAtom(xdisp, "_NET_CLIENT_LIST", true);
    Atom actualType;
    int format;
    unsigned long num, bytes;
    Window *data = 0;

    for (int i = 0; i < ScreenCount(xdisp); ++i) {
        Window rootWin = RootWindow(xdisp, i);

        int status = XGetWindowProperty(xdisp, rootWin, netClList, 0L,
                                        ~0L, false, AnyPropertyType,
                                        &actualType, &format, &num,
                                        &bytes, (uint8_t **)&data);

        if (status != Success) {
            printf("Failed getting root "
                              "window properties\n");
            continue;
        }

        for (unsigned long i = 0; i < num; ++i)
        {
            res.push_back(data[i]);
        }

        XFree(data);
    }

    return res;
}
//size_t os_mbs_to_wcs_ptr(const char *str, size_t len, wchar_t **pstr)
//{
//    if (str) {
//        size_t out_len = os_mbs_to_wcs(str, len, NULL, 0);
//
//        *pstr = ::malloc((out_len + 1) * sizeof(wchar_t));
//        return os_mbs_to_wcs(str, len, *pstr, out_len + 1);
//    } else {
//        *pstr = NULL;
//        return 0;
//    }
//}
//size_t os_mbs_to_utf8_ptr(const char *str, size_t len, char **pstr)
//{
//    char *dst = NULL;
//    size_t out_len = 0;
//
//    if (str) {
//        wchar_t *wstr = NULL;
//        size_t wlen = os_mbs_to_wcs_ptr(str, len, &wstr);
//        out_len = os_wcs_to_utf8_ptr(wstr, wlen, &dst);
//        bfree(wstr);
//    }
//
//    *pstr = dst;
//    return out_len;
//}
std::string getWindowAtom(Window win, const char *atom)
{
    Atom netWmName = XInternAtom(xdisp, atom, false);
    int n;
    char **list = 0;
    XTextProperty tp;
    std::string res = "unknown";

    XGetTextProperty(xdisp, win, &tp, netWmName);

    if (!tp.nitems)
        XGetWMName(xdisp, win, &tp);

    if (!tp.nitems)
        return "error";

    if (tp.encoding == XA_STRING) {
        res = (char *)tp.value;
    } else {
        int ret = XmbTextPropertyToTextList(xdisp, &tp, &list, &n);

        if (ret >= Success && n > 0 && *list) {
            res = *list;
            XFreeStringList(list);
        }
    }

//    char *conv = nullptr;
//    if (os_mbs_to_utf8_ptr(res.c_str(), 0, &conv))
//        res = conv;
//    bfree(conv);

    XFree(tp.value);

    return res;
}
inline std::string getWindowName(Window win)
{
    return getWindowAtom(win, "_NET_WM_NAME");
}

inline std::string getWindowClass(Window win)
{
    return getWindowAtom(win, "WM_CLASS");
}

void show_all_window()
{
    std::vector<WindowInfo> window_strings;
    for (Window win : getTopLevelWindows()) {
        std::string wname =  getWindowName(win);
        std::string cls =  getWindowClass(win);
        std::string winid = std::to_string((long long)win);
        std::string desc = (winid + WIN_STRING_DIV + wname + WIN_STRING_DIV + cls);

        std::string wname_lowercase = wname;
        std::transform(wname_lowercase.begin(), wname_lowercase.end(),
                       wname_lowercase.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        window_strings.push_back({.lex_comparable = wname_lowercase,
                                         .name = wname,
                                         .desc = desc});
    }

    std::sort(window_strings.begin(), window_strings.end(),
              [](const WindowInfo &a, const WindowInfo &b) -> bool {
                  return std::lexicographical_compare(
                          a.lex_comparable.begin(),
                          a.lex_comparable.end(),
                          b.lex_comparable.begin(),
                          b.lex_comparable.end());
              });

    for (auto s : window_strings)
    {
        printf("[lex_comparable = %s][name = %s][desc = %s]\n", s.lex_comparable.c_str(), s.name.c_str(), s.desc.c_str());
    }

}

int main(int argc, char *argv[])
{
    init();
    show_all_window();
    return 0;
}