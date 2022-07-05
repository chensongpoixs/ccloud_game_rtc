#include "desktop_capture.h"
#include "video_renderer.h"
#include "rtc_base/logging.h"

#include <thread>
#include "system_wrappers/include/field_trial.h"
//##include "test/field_trial.h"
//#include  <Windows.h>
//#include <iostream>
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//int WINAPI open_windows(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
//{
//	// Register the window class.
//	const wchar_t CLASS_NAME[]  = L"Sample Window Class";
//
//	WNDCLASS wc = { };
//
//	wc.lpfnWndProc   = WindowProc;
//	wc.hInstance     = hInstance;
//	wc.lpszClassName = CLASS_NAME;
//
//	RegisterClass(&wc);
//
//	// Create the window.
//
//	HWND hwnd = CreateWindowEx(
//		0,                              // Optional window styles.
//		CLASS_NAME,                     // Window class
//		L"Learn to Program Windows",    // Window text
//		WS_OVERLAPPEDWINDOW,            // Window style
//
//										// Size and position
//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//
//		NULL,       // Parent window    
//		NULL,       // Menu
//		hInstance,  // Instance handle
//		NULL        // Additional application data
//	);
//
//	if (hwnd == NULL)
//	{
//		return 0;
//	}
//
//	ShowWindow(hwnd, nCmdShow);
//	// 设置窗口隐藏功能
//	//ShowWindow(hwnd,SW_HIDE );
//	RTC_LOG(INFO) << "desktop capture !!!";
//	std::unique_ptr<webrtc_demo::DesktopCapture> capturer(webrtc_demo::DesktopCapture::Create(15,0));
//	capturer->StartCapture();
//
//	std::unique_ptr<webrtc::test::VideoRenderer> renderer(webrtc::test::VideoRenderer::Create(capturer->GetWindowTitle().c_str(), 720, 480));
//
//	{
//	
//		
//		
//
//		capturer->AddOrUpdateSink(renderer.get(), rtc::VideoSinkWants());
//
//		/*while (true)
//		{
//			std::this_thread::sleep_for(std::chrono::seconds(30));
//		}*/
//		
//
//		
//	
//	
//	}
//	// Run the message loop.
//
//	MSG msg = { };
//	while (GetMessage(&msg, NULL, 0, 0))
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	capturer->RemoveSink(renderer.get());
//	RTC_LOG(WARNING) << "Demo exit";
//	return 0;
//}
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	switch (uMsg)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		return 0;
//
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = BeginPaint(hwnd, &ps);
//		FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
//
//		EndPaint(hwnd, &ps);
//	}
//	return 0;
//
//	}
//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
//}
//
//
//int PASCAL wWinMain(HINSTANCE instance,
//	HINSTANCE prev_instance,
//	wchar_t* cmd_line,
//	int cmd_show)

int desktop_test_main(int argc, char *argv[])
{
	//HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow
//	open_windows(instance, prev_instance, cmd_line, cmd_show);
	//return 0;
	RTC_LOG(INFO) << "desktop capture !!!";

  std::unique_ptr<webrtc_demo::DesktopCapture> capturer(webrtc_demo::DesktopCapture::Create(15,0));

  capturer->StartCapture();

  std::unique_ptr<webrtc::test::VideoRenderer> renderer(webrtc::test::VideoRenderer::Create(capturer->GetWindowTitle().c_str(), 720, 480));

  capturer->AddOrUpdateSink(renderer.get(), rtc::VideoSinkWants());

  while (true)
  {
	  std::this_thread::sleep_for(std::chrono::seconds(30));
  }
  capturer->RemoveSink(renderer.get());
  
  RTC_LOG(WARNING) << "Demo exit";
  return 0;
}



#include <iostream>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include "cmediasoup_mgr.h"




cmediasoup::cmediasoup_mgr g_mediasoup_mgr;


bool stoped = false;

void signalHandler(int signum)
{
    stoped = true;

}



int  main(int argc, char *argv[])
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);


    g_mediasoup_mgr.init();

    //g_mediasoup_mgr.set_mediasoup_status_callback(&mediasoup_callback);
    /*
    const char* mediasoupIp, uint16_t port
        , const char* roomName, const char* clientName

    */
    g_mediasoup_mgr.startup("192.168.1.58", 8888, "unix", "unix");
    while (!stoped)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    g_mediasoup_mgr.destroy();
    return 0;
}


