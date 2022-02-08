 
#include "mediasoupclient.hpp"
//#include <cpr/cpr.h>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
 
#include "cwebsocket_mgr.h"
#include "ccfg.h"
 
#include "cwebsocket_mgr.h"
#include "ccfg.h"
 
 
 
#include "cwindow_util.h"
#include <api/task_queue/global_task_queue_factory.h>
#include "cclient.h"
using json = nlohmann::json;


chen::cclient g_client;
bool stoped = false;
//chen::ccloud_rendering ccloud_rendering_mgr;

void stop()
{
	//ccloud_rendering_mgr.Destroy();
	//webrtc::g_websocket_mgr.destroy();
	//// Remove broadcaster from the server.
	//broadcaster->Stop();
	//RTC_LOG(LS_INFO) << "[INFO] leaving!";
	//delete broadcaster;
	//broadcaster = nullptr;
	//stoped = true;
	////std::exit(signum);
	
}
void signalHandler(int signum)
{
	//RTC_LOG(LS_INFO) << "[INFO] interrupt signal (" << signum << ") received";

	//stop();
	
}


int  main(int argc, char* argv[])
{ 
 

	
	const char* config_filename = "client.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	bool init = g_client.init(config_filename);
	if (!init)
	{
		//
		g_client.Destory();
		printf("g_client init failed config_name = %s", config_filename);
		
		return -1;
	}
	g_client.Loop();
	g_client.Destory();


	
	
	return 0;
}
















#if defined(_MSC_VER)



// Test_Console.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
//#include <afx.h>  
//#include <afxwin.h>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <WtsApi32.h>
#include <locale.h>
#include <stdio.h>
#include<Windows.h>
#include<iostream>
#pragma comment(lib,"WtsApi32.lib")

int test_windown_main(int argc, char* argv[])
{
	HWND notepadWnd =   FindWindowA("Notepad", NULL); // 记事本窗口句柄

	//if (notepadWnd == NULL)
	//{

	//	printf("没有找到记事本窗口\n");

	//	return 0;

	//}

	//
	HWND wnd = chen::FindMainWindow(23580);
	HWND editWnd = FindWindowExA(notepadWnd, NULL,  "Edit" , NULL);
	//HWND editWnd = FindWindowExA(notepadWnd, NULL,  "SysTabControl32" , NULL); // 编辑框窗口句柄
	const BYTE vk = 'O'; // 虚拟键码
	//bool ret = SetForegroundWindow(editWnd);
	//HWND new_wnd = GetFocus();
	//DWORD SelfThreadId = GetCurrentThreadId(); // GetCurrentThreadId();//获取本身的线程ID
	//DWORD ForeThreadId = GetWindowThreadProcessId(wnd, NULL);//根据窗口句柄获取线程ID
	//AttachThreadInput(ForeThreadId, SelfThreadId, true);//附加线程
	//HWND new_wnd = GetFocus();//获取具有输入焦点的窗口句柄
	//::PostMessage(editWnd, WM_NCMOUSEMOVE, MAKEWPARAM(0, 0), MAKEWPARAM(100, 100));
	//::PostMessage(editWnd, WM_KEYDOWN, vk, MAKEWPARAM(100, 100) /*| scanCode << 16*/);
	::PostMessage(editWnd, WM_KEYDOWN, 0x11, /*0x1D0001*/0);
	::PostMessage(editWnd, WM_KEYDOWN, 0x9, /*0xF0001*/ 0);
	
	::PostMessage(editWnd, WM_KEYUP, 0x9, /*0xC00F0001*/ 0);
	::PostMessage(editWnd, WM_KEYUP, 0x11, /*0xC01D0001*/ 0);
	
	//::PostMessage (editWnd, WM_CHAR, 'h', 0);   
	::PostMessage(editWnd, WM_KEYDOWN, 0x0D, /*0x1D0001*/0);
	//::PostMessage(editWnd, WM_KEYUP, 0x0D, /*0xF0001*/ 0);
	::PostMessage(editWnd, WM_KEYDOWN, 0X0F, /*0x1D0001*/0);
	//::PostMessage(editWnd, WM_KEYUP, 0x0D, /*0xF0001*/ 0);
	::PostMessage(editWnd, WM_KEYDOWN, vk, MAKEWPARAM(100, 100) /*| scanCode << 16*/);
	::PostMessage(editWnd, WM_KEYUP, vk, MAKEWPARAM(100, 100) /*| scanCode << 16*/ | 1 << 30 | 1 << 31);
	const BYTE vkv = 'L'; // 虚拟键码
	::PostMessage(editWnd, WM_KEYDOWN, VK_SHIFT, 0);
	::PostMessage (editWnd, WM_KEYDOWN, vkv, 0); 
	::PostMessage (editWnd, WM_KEYUP, vkv, 0); 
	//AttachThreadInput(ForeThreadId, SelfThreadId, false);//取消附加的线程
						//UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC); // 扫描码


	Sleep(100);

	
	return 0;
}
#endif