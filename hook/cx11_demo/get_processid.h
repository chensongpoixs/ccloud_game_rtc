// #ifndef __WINDOWIDUTIL_H__
// #define __WINDOWIDUTIL_H__


// // win
// #ifdef WIN32

// #include <windows.h>

// typedef struct
// {
//     HWND hWnd;
//     DWORD dwPid;
// }WNDINFO;


// BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
// {
//     WNDINFO* pInfo = (WNDINFO*)lParam;
//     DWORD dwProcessId = 0;
//     GetWindowThreadProcessId(hWnd, &dwProcessId);

//     if(dwProcessId == pInfo->dwPid)
//     {
//      pInfo->hWnd = hWnd;
//      return FALSE;
//     }
//     return TRUE;
// }


// WId wwwget_win_id_from_pid(DWORD dwProcessId)
// {
//     WNDINFO info = {0};
//     info.hWnd = NULL;
//     info.dwPid = dwProcessId;
//     EnumWindows(EnumWindowsProc, (LPARAM)&info);
//     return (WId)info.hWnd;
// }

// #else // linux


// #include <X11/Xlib.h>
// #include <X11/Xatom.h>
// #include <iostream>
// #include <list>
// #include <stdlib.h>
// #include <unistd.h>



// class WindowsMatchingPid
// {
// public:
//     WindowsMatchingPid(Display *display, Window wRoot, unsigned long pid)
//         : _display(display),
//           _pid(pid)
//     {
//         // Get the PID property atom.
//         _atomPID = XInternAtom(display, "_NET_WM_PID", True);
//         if(_atomPID == None)
//         {
//             std::cout << "No such atom" << std::endl;
//             return;
//         }

//         search(wRoot);
//     }

//     const std::list<Window> &result() const { return _result; }

// private:
//     unsigned long  _pid;
//     Atom           _atomPID;
//     Display       *_display;
//     std::list<Window>   _result;

//     void
//     search(Window w)
//     {
//         // Get the PID for the current Window.
//         Atom           type;
//         int            format;
//         unsigned long  nItems;
//         unsigned long  bytesAfter;
//         unsigned char *propPID = 0;
//         if(Success == XGetWindowProperty(
//                     _display,
//                     w,
//                     _atomPID,
//                     0,
//                     1,
//                     False,
//                     XA_CARDINAL,
//                     &type,
//                     &format,
//                     &nItems,
//                     &bytesAfter,
//                     &propPID))
//         {
//             if(propPID != 0)
//             {
//                 // If the PID matches, add this window to the result set.
//                 if(_pid == *((unsigned long *)propPID))
//                 {
//                     _result.push_back(w);
//                 }

//                 XFree(propPID);
//             }
//         }

//         // Recurse into child windows.
//         Window    wRoot;
//         Window    wParent;
//         Window   *wChild;
//         unsigned  nChildren;
//         if(0 != XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren))
//         {
//             for(unsigned i = 0; i < nChildren; i++)
//                 search(wChild[i]);
//         }

//         //XFree(propPID);
//     }
// };


// //getpid()
// unsigned long get_win_id_from_pid(int pid)
// {
//     //std::cout << "Searching for windows associated with PID " << pid << std::endl;

//     // Start with the root window.
//     Display *display = XOpenDisplay(0);

//     WindowsMatchingPid match(display, XDefaultRootWindow(display), pid);

//     // Print the result.
//     const std::list<Window> &result = match.result();

//     long win_id = 0;
// #if 0
//     for(std::list<Window>::const_iterator pos = result.begin(); pos != result.end(); pos++)
//     {
//         std::cout << "Window #" << (unsigned long)(*pos) << std::endl;
//         win_id = (unsigned long)(*pos);

//         break;
//     }
// #else
//     win_id = (long)(*result.begin());
// #endif

//     std::cout << "Window id: "<< win_id << std::endl;
//     return win_id;
// }


// #endif

// #endif // __WINDOWIDUTIL_H__
//  