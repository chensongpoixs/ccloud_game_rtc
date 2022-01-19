#ifndef _C_WINDOW_UTIL_H_
#define _C_WINDOW_UTIL_H_
#include <Windows.h>
namespace webrtc {
	HWND FindMainWindow();
	HWND FindMainWindow(long long id);
	HWND MainChildPoint(HWND mwnd, POINT pt);
}
#endif// _C_WINDOW_UTIL_H_