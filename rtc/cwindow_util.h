#ifndef _C_WINDOW_UTIL_H_
#define _C_WINDOW_UTIL_H_
#if defined(_MSC_VER)
#include <Windows.h>
namespace chen {
	HWND FindMainWindow();
	HWND FindMainWindow(long long id);
	HWND MainChildPoint(HWND mwnd, POINT pt);
}
#endif //#if defined(_MSC_VER)
#endif// _C_WINDOW_UTIL_H_