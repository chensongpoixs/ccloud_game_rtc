#include "Broadcaster.hpp"
#include "mediasoupclient.hpp"
//#include <cpr/cpr.h>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include "httplib.h"
#include "cwebsocket_mgr.h"
#include "ccfg.h"
#include "Broadcaster.hpp"
#include "cwebsocket_mgr.h"
#include "ccfg.h"
#include "httplib.h"
#include "peerConnectionUtils.hpp"
#include "ccloud_rendering.h"
#include "cwindow_util.h"
#include <api/task_queue/global_task_queue_factory.h>
using json = nlohmann::json;

bool stoped = false;
chen::ccloud_rendering ccloud_rendering_mgr;

void stop()
{
	ccloud_rendering_mgr.Destroy();
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
	RTC_LOG(LS_INFO) << "[INFO] interrupt signal (" << signum << ") received";

	stop();
	
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
	std::thread m_thread;
	int32_t  count = 0;
	if (!ccloud_rendering_mgr.init(config_filename))
	{
		// error
		return -1;
	}
	ccloud_rendering_mgr.Loop();;

	ccloud_rendering_mgr.Destroy();
	return 0;
	printf("init ok  2... \n");
	//while (false)
	{
		printf("main loop ........ \n");
		m_thread = std::thread(
			[=]() {
			printf("child ->  loop ........ \n");
			ccloud_rendering_mgr.Loop();
			printf("child ->  loop exit ........ \n");
		}
		);
		printf("main [g_thread_count = %d] loop min 2... \n", g_thread_count);
		std::this_thread::sleep_for(std::chrono::seconds(30));
		printf("main [g_thread_count = %d][] loop min 2... \n", g_thread_count);
		ccloud_rendering_mgr.Destroy();
		
		++count;
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		/*if (count < 10)
		{
			break;
		}*/
	}
	//mediasoupclient::Handler::all_close();
	printf("all stop\n");
	//std::this_thread::sleep_for(std::chrono::seconds(10));

	
	printf("+++++++++++++++++ all stop ok !!! \n");
	//std::this_thread::sleep_for(std::chrono::seconds(5));
	
	
	//std::this_thread::sleep_for(std::chrono::minutes(1));
	//webrtc::GlobalTaskQueueFactory()
	//while (true)
	{
		printf("main [g_thread_count = %d] ====================loop min 2... \n", g_thread_count);
		std::this_thread::sleep_for(std::chrono::seconds(10));
		printf("main exit [g_thread_count = %d] ====================loop min 2... \n", g_thread_count);
		//exit(0);
	}
	

	return 0;
	
	
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
