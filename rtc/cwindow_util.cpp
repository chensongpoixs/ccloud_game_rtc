
#include "cwindow_util.h"
#if defined(_MSC_VER)
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>

#include <thread>

namespace chen {






	int Pnum = 0, Cnum;//父窗口数量，每一级父窗口的子窗口数量

					   //---------------------------------------------------------
					   //EnumChildWindows回调函数，hwnd为指定的父窗口
					   //---------------------------------------------------------
	BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam)
	{
		wchar_t WindowTitle[100] = { 0 };
		Cnum++;
		::GetWindowText(hWnd, WindowTitle, 100);
		
		return true;
	}
	//---------------------------------------------------------
	//EnumWindows回调函数，hwnd为发现的顶层窗口
	//---------------------------------------------------------
	BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
	{
		if (GetParent(hWnd) == NULL && IsWindowVisible(hWnd))  //判断是否顶层窗口并且可见
		{
			Pnum++;
			Cnum = 0;
			wchar_t WindowTitle[100] = { 0 };
			::GetWindowText(hWnd, WindowTitle, 100);
			
			EnumChildWindows(hWnd, EnumChildWindowsProc, NULL); //获取父窗口的所有子窗口
		}
		return true;
	}
	//---------------------------------------------------------
	//main函数
	//---------------------------------------------------------

	//获取屏幕上所有的顶层窗口,每发现一个窗口就调用回调函数一次


	struct handle_data {
		unsigned long process_id;
		HWND best_handle;
	};

	BOOL IsMainWindow(HWND handle)
	{
		return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
	}
	BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
	{
		handle_data& data = *(handle_data*)lParam;
		unsigned long process_id = 0;
		GetWindowThreadProcessId(handle, &process_id);
		//printf("process_id = %lu\n", process_id);
		if (data.process_id != process_id || !IsMainWindow(handle)) {
			return TRUE;
		}
		data.best_handle = handle;
		return FALSE;
	}

	HWND FindMainWindow()
	{ 
		handle_data data;
		data.process_id = ::GetCurrentProcessId();
		data.best_handle = 0;
		EnumWindows(EnumWindowsCallback, (LPARAM)&data);
		return data.best_handle;
	}

	HWND FindMainWindow(long long id)
	{
		handle_data data;
		data.process_id = id;
		data.best_handle = 0;
		EnumWindows(EnumWindowsCallback, (LPARAM)&data);
		return data.best_handle;
	}
	
	/** 
	* TODO@chensong 2022-01-19 
	* 获取子窗口的api  -> 搞怎么一个东西用 三天 哭了
	*/
	HWND MainChildPoint(HWND mwnd, POINT pt)
	{
		if (!mwnd)
		{
			return NULL;
		}
		return ::ChildWindowFromPoint(mwnd, pt);
	}
}

#endif //#if defined(_MSC_VER)