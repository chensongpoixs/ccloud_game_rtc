/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
************************************************************************************************/

#include "cinput_device.h"
#include "cwindow_util.h"
#include "cprotocol.h"
#include "cinput_device_event.h"
#include "rtc_base/logging.h"
#include "clog.h"
#if defined(_MSC_VER)
// TODO@chensong 20220711  UE 鼠标控制权问题
#define _WIN32_WINNT 0x0400 
#include <Windows.h>
#include <WinUser.h>
#include <UserEnv.h>
#include <mutex>
//#include "detours.h"
//void CallMessage(HWND hwnd, int nMsgId, int wParam, int lParam)
//
//{
//
//	WNDPROC fWndProc = (WNDPROC)GetWindowLong(hwnd, GWL_WNDPROC); //获取窗口wndproc
//
//	if (fWndProc != NULL)
//
//	{
//
//		fWndProc(hwnd, nMsgId, wParam, lParam);
//
//	}
//
//}
#endif // WIN
namespace chen {
	int32_t  g_width = 0;
	int32_t  g_height = 0;
	using FKeyCodeType = uint8;
	using FCharacterType = TCHAR;
	using FRepeatType = uint8;
	using FButtonType = uint8;
	using FPosType = uint16;
	using FDeltaType = int16;
	//using FTouchesType = TArray<FTouch>;
	using FControllerIndex = uint8;
	using FControllerButtonIndex = uint8;
	using FControllerAnalog = double;
	using FControllerAxis = uint8;


	//static long long timeA = systemtime();
	//std::chrono::milliseconds
	//cout << timeA << endl;


	////PostMessage
//#define MESSAGE(g_wnd, message_id, param1, param2) MSG msg;  \
//														  msg.hwnd = g_wnd;			\
//												          msg.message = message_id;	\
//														  msg.wParam = param1;		\
//														  msg.lParam = param2;		\
//			std::chrono::steady_clock::time_point cur_time_ms = std::chrono::steady_clock::now();     \
//											long long ms = static_cast<long long >(cur_time_ms.time_since_epoch().count() / 1000000); \
//												 msg.time = static_cast<DWORD>(ms);	\
//														  msg.pt.x = g_width; msg.pt.y = g_height; \
//														::TranslateMessage(&msg);         \
//													long long ret_dispatch =	 ::DispatchMessage(&msg); 
//													//NORMAL_EX_LOG("move cur_ms = %u, [ret_dispatch = %s]", ms, std::to_string(ret_dispatch).c_str());
//

#define MESSAGE(g_wnd, message_id, param1, param2)  PostMessage(g_wnd, message_id, param1, param2);



 

	//if (GetCapture() != g_wnd)
#define MOUSE_INPUT(g_wnd)		 { \
								SetForegroundWindow(g_wnd);  \
								SetCapture(g_wnd);           \
								SetFocus(g_wnd); }

#define CliENTTOSCREENPOINT(hwnd, xx, yy) POINT CursorPoint; \
	CursorPoint.x = xx; \
	CursorPoint.y = yy; \
	ClientToScreen(hwnd, &CursorPoint);
	//BringWindowToTop(g_wnd);	\
	//	SetForegroundWindow(g_wnd); \
	// SetWindowLong(g_wnd, GWL_STYLE, GetWindowLong(g_wnd, GWL_STYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	//使用全局变量操作的哈 
#define SET_POINT() POINT pt; pt.x = g_width; pt.y = g_height;

#if defined(_MSC_VER)
#define WINDOW_MAIN()		HWND mwin = FindMainWindow()
#define WINDOW_CHILD()	HWND childwin = MainChildPoint(mwin, pt)
#define WINDOW_BNTTON_DOWN(v)  uint32 active_type = WM_LBUTTONDOWN;					 \
	switch (vec.button)																 \
	{                                                                             	 \
		case EMouseButtons::Left:													 \
		{																			 \
			active_type = WM_LBUTTONDOWN;											 \
			break;																	 \
		}																			 \
		case EMouseButtons::Middle:													 \
		{																			 \
			active_type = WM_MBUTTONDOWN;											 \
																					 \
			break;																	 \
		}																			 \
		case EMouseButtons::Right:													 \
		{																			 \
			active_type = WM_RBUTTONDOWN;											 \
			break;																	 \
		}																			 \
		default:																	 \
							 														 \
			break;																	 \
	}

#define WINDOW_BNTTON_UP(v)  uint32 active_type = WM_LBUTTONUP; switch (vec.button)   \
	{                                                                             	 \
		case EMouseButtons::Left:													 \
		{																			 \
			active_type = WM_LBUTTONUP;											 \
			break;																	 \
		}																			 \
		case EMouseButtons::Middle:													 \
		{																			 \
			active_type = WM_MBUTTONUP;											 \
																					 \
			break;																	 \
		}																			 \
		case EMouseButtons::Right:													 \
		{																			 \
			active_type = WM_RBUTTONUP;											 \
			break;																	 \
		}																			 \
		default:																	 \
							 														 \
			break;																	 \
	}
#endif //#if defined(_MSC_VER)
#define REGISTER_INPUT_DEVICE(type, handler_ptr) if (false == m_input_device.insert(std::make_pair(type, handler_ptr)).second){	 ERROR_EX_LOG("[type = %s][handler_ptr = %s]", #type, #handler_ptr);	return false;}
	
	static HWND g_main_mouse_down_up = NULL;
	static HWND g_child_mouse_down_up = NULL;
///	cinput_device   g_input_device_mgr;
	cinput_device::cinput_device() 
		:  m_input_device()
		,  m_int_point(){}
	cinput_device::~cinput_device() {}

#if defined(_MSC_VER)
	static void registerinputdevice(HWND hwnd)
	{
		static bool load = false;
		if (load)
		{
			return;
		}
		load = true;
		DWORD flags = RIDEV_REMOVE;


		// NOTE: Currently has to be created every time due to conflicts with Direct8 Input used by the wx unrealed
		RAWINPUTDEVICE RawInputDevice;

		//The HID standard for mouse
		const uint16 StandardMouse = 0x02;

		RawInputDevice.usUsagePage = 0x01;
		RawInputDevice.usUsage = StandardMouse;
		RawInputDevice.dwFlags = flags;

		// Process input for just the window that requested it.  NOTE: If we pass NULL here events are routed to the window with keyboard focus
		// which is not always known at the HWND level with Slate
		RawInputDevice.hwndTarget = hwnd;

		// Register the raw input device
		::RegisterRawInputDevices(&RawInputDevice, 1, sizeof(RAWINPUTDEVICE));
	}


#endif //#if defined(_MSC_VER)
	//static bool g_move_init = false;
	///////////////////////////////////////////////////////////////////////////////
	/*static HMODULE user32dll = NULL;
	typedef UINT(WINAPI* PFN_GetRawInputData)(HRAWINPUT, UINT, LPVOID, PUINT, UINT);
	PFN_GetRawInputData RealGetRawInputData = NULL;
	/// <summary>
	/// GetCursorPos
	/// </summary>

	typedef BOOL (WINAPI* PFN_GetCursorPos)( _Out_ LPPOINT lpPoint);
	PFN_GetCursorPos  RealGetCursorPos = NULL;

	typedef BOOL (WINAPI* PFN_SetCursorPos)( _In_ int X, _In_ int Y);
	PFN_SetCursorPos RealSetCursorPos = NULL;


	typedef  ATOM (WINAPI* PFN_RegisterClassA)( _In_ CONST WNDCLASSA* lpWndClass);
	PFN_RegisterClassA RealRegisterClassA;


	typedef ATOM (WINAPI* PFN_RegisterClassW)( _In_ CONST WNDCLASSW* lpWndClass);
	PFN_RegisterClassW  RealRegisterClassW;



	typedef BOOL (WINAPI* PFN_UnregisterClassA)(_In_ LPCSTR lpClassName,_In_opt_ HINSTANCE hInstance);
	PFN_UnregisterClassA RealUnregisterClassA;

	typedef BOOL (WINAPI* PFN_UnregisterClassW)(_In_ LPCWSTR lpClassName, _In_opt_ HINSTANCE hInstance);
	PFN_UnregisterClassW RealUnregisterClassW;*/

	///////////////////////////////////////////////////////////////////////////////
	 

	static  std::map<uint64, RAWINPUT> g_hrawinput;
	static const uint32 RAW_INPUT_SIZE = 300;
	static uint32     g_read_index = 0;
	static uint32	  g_write_index = 0;
	static  std::mutex   g_mutex;
	typedef std::lock_guard<std::mutex>		clock_guard;
	 
	static std::map<const WNDCLASSA*, const  WNDCLASSA*> g_wnd_classA;
	static std::map<const  WNDCLASSW*,const  WNDCLASSW*> g_wnd_classW;
	 
//#define MESSAGE(g_wnd, message_id, param1, param2)  for (std::map<const WNDCLASSA*, const WNDCLASSA*>::const_iterator iter = g_wnd_classA.begin(); iter != g_wnd_classA.end(); ++iter) \
//	{																																	\
//		if (iter->first && iter->first->lpfnWndProc)																					\
//		{																																\
//			iter->first->lpfnWndProc(g_wnd, message_id, param1, param2);																\
//		}																																\
//	}																																	\
//	for (std::map<const WNDCLASSW*, const WNDCLASSW*>::const_iterator iter = g_wnd_classW.begin(); iter != g_wnd_classW.end(); ++iter)	\
//	{																																	\
//		if (iter->first && iter->first->lpfnWndProc)																					\
//		{																																\
//			iter->first->lpfnWndProc(g_wnd, message_id, param1, param2);																\
//		}																																\
//	}

	//static UINT WINAPI hook_get_raw_input_data(_In_ HRAWINPUT hRawInput, _In_ UINT uiCommand, _Out_writes_bytes_to_opt_(*pcbSize, return) LPVOID pData, _Inout_ PUINT pcbSize, _In_ UINT cbSizeHeader)
	//{ 
	//	UINT ret = RealGetRawInputData(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);
	//	  
	//	{ 
	//		clock_guard lock(g_mutex);
	//		if (  uiCommand == RID_INPUT /*&& (ret != 48 && ret != 0) && g_hrawinput.size() > g_read_index*/ )
	//		{ 
	//			std::map<uint64, RAWINPUT>::iterator iter = g_hrawinput.find((uint64)hRawInput);
	//			if (iter != g_hrawinput.end())
	//			{
	//				DEBUG_LOG("[g_hrawinput.size() = %u][hRawInput = %u][uiCommand = %u][ret = %u][cbSizeHeader = %u]", g_hrawinput.size(), hRawInput, uiCommand, ret, cbSizeHeader);

	//				//DEBUG_LOG("find [hRawInput = %u]", hRawInput);
	//				ret = iter->second.header.dwSize;
	//				if (pcbSize)
	//				{
	//					*pcbSize = ret;
	//				}
	//				if (pData)
	//				{
	//					memcpy(pData, &iter->second, ret);
	//				}
	//				
	//			}
	//			else
	//			{
	//				DEBUG_LOG("not find hRawInput = %u", hRawInput);
	//			}  
	//		}
	//	}
	//	
	//	if (false)
	//	{
	//		RAWINPUT* temp_input = (RAWINPUT*)pData;
	//		DEBUG_LOG("[g_hrawinput.size() = %u][g_read_index = %u][uiCommand = %u][ret = %u][cbSizeHeader = %u]", g_hrawinput.size(), g_read_index, uiCommand, ret, cbSizeHeader);
	//		DEBUG_LOG("[temp_input->header.dwType = %u]", temp_input->header.dwType);
	//		DEBUG_LOG("[temp_input->data.mouse.lLastX = %u][temp_input->data.mouse.lLastY = %u]", temp_input->data.mouse.lLastX, temp_input->data.mouse.lLastY);
	//	}
	//	return ret;
	//}


	//static BOOL WINAPI hook_get_cursor_pos (_Out_ LPPOINT lpPoint)
	//{
	//	if (lpPoint)
	//	{
	//		lpPoint->x = g_width;
	//		lpPoint->y = g_height;
	//		return 1;
	//	}
	//	return RealGetCursorPos(lpPoint);;
	//}
	//static BOOL WINAPI hook_set_cursor_pos(_In_ int X, _In_ int Y)
	//{
	//	/*if (lpPoint)
	//	{
	//		lpPoint->x = g_width;
	//		lpPoint->y = g_height;
	//		return 1;
	//	}*/
	//	return RealSetCursorPos(X, Y);
	//}

	//static ATOM  hook_RegisterClassA(_In_ CONST WNDCLASSA* lpWndClass)
	//{
	//	if (lpWndClass && lpWndClass->lpfnWndProc)
	//	{
	//		NORMAL_EX_LOG("add wnd [lpszClassName = %s][lpszMenuName = %s]", lpWndClass->lpszClassName, lpWndClass->lpszMenuName);
	//		g_wnd_classA[lpWndClass] = lpWndClass;
	//	}
	//	else if(lpWndClass)
	//	{
	//		NORMAL_EX_LOG("not add wnd [lpszClassName = %s][lpszMenuName = %s]", lpWndClass->lpszClassName, lpWndClass->lpszMenuName);
	//	}
	//	return RegisterClassA(lpWndClass);
	//}


	//static ATOM  hook_RegisterClassW(_In_ CONST WNDCLASSW* lpWndClass)
	//{
	//	if (lpWndClass && lpWndClass->lpfnWndProc)
	//	{
	//		NORMAL_EX_LOG("add wnd [lpszClassName = %s][lpszMenuName = %s]", lpWndClass->lpszClassName, lpWndClass->lpszMenuName);
	//		g_wnd_classW[lpWndClass] = lpWndClass;
	//	}
	//	else if (lpWndClass)
	//	{
	//		NORMAL_EX_LOG("not add wnd [lpszClassName = %s][lpszMenuName = %s]", lpWndClass->lpszClassName, lpWndClass->lpszMenuName);
	//	}
	//	return RegisterClassW(lpWndClass);
	//}


	//static BOOL hook_UnregisterClassA(_In_ LPCSTR lpClassName, _In_opt_ HINSTANCE hInstance)
	//{

	//	return UnregisterClassA(lpClassName, hInstance);
	//}


	//static BOOL hook_UnregisterClassW(_In_ LPCWSTR lpClassName, _In_opt_ HINSTANCE hInstance)
	//{
	//	return UnregisterClassW(lpClassName, hInstance);
	//}

	//static inline HMODULE get_system_module(const char* system_path, const char* module)
	//{
	//	char base_path[MAX_PATH];

	//	strcpy(base_path, system_path);
	//	strcat(base_path, "\\");
	//	strcat(base_path, module);
	//	return GetModuleHandleA(base_path);
	//}
	bool cinput_device::init()
	{
		//g_hrawinput.resize(RAW_INPUT_SIZE);
		 // win
		//char system_path[MAX_PATH] = {0};

		//UINT ret = GetSystemDirectoryA(system_path, MAX_PATH);
		//if (!ret)
		//{
		//	ERROR_EX_LOG("Failed to get windows system path: %lu\n", GetLastError());
		//	//return false;
		//}
		//SYSTEM_LOG("[system_path = %s] ok ", system_path);
		//// hook mouse move  GetRawInputData
		//user32dll = get_system_module(system_path, "user32.dll");
		//if (!user32dll)
		//{
		//	WARNING_EX_LOG(" REGISTER mouse  failed !!!");
		//	//return false;
		//}
		//SYSTEM_LOG("REGISTER  mouse ok !!!");
		//void* get_raw_input_data_proc = GetProcAddress(user32dll, "GetRawInputData");
		//void* get_cursor_pos_proc = GetProcAddress(user32dll, "GetCursorPos");
		//void* set_cursor_pos_proc = GetProcAddress(user32dll, "SetCursorPos");
		/// <summary>
		/// ///////////////////
		/// </summary>
		/// <returns></returns>
		/*void* register_class_a_proc = GetProcAddress(user32dll, "RegisterClassA");
		void* register_class_w_proc = GetProcAddress(user32dll, "RegisterClassW");*/


		/*if (!get_raw_input_data_proc)
		{
			ERROR_EX_LOG("seatch mouse table not find GetRawInputData !!!");
		}
		else*/
		//{
		//	SYSTEM_LOG("    input device  begin ... ");
		//	DetourTransactionBegin();

		//	
		//	if (get_raw_input_data_proc)
		//	{
		//		RealGetRawInputData = (PFN_GetRawInputData)get_raw_input_data_proc;
		//		DetourAttach((PVOID*)&RealGetRawInputData,
		//			hook_get_raw_input_data);
		//	}
		//	if (get_cursor_pos_proc)
		//	{
		//		RealGetCursorPos = (PFN_GetCursorPos)get_cursor_pos_proc;
		//		DetourAttach((PVOID*)&RealGetCursorPos,
		//			hook_get_cursor_pos);
		//	}
		//	if (set_cursor_pos_proc)
		//	{
		//		RealSetCursorPos = (PFN_SetCursorPos)set_cursor_pos_proc;
		//		DetourAttach((PVOID*)&RealSetCursorPos,
		//			hook_set_cursor_pos);
		//	}


		//	/*if (register_class_a_proc)
		//	{
		//		RealRegisterClassA = (PFN_RegisterClassA)register_class_a_proc;
		//		DetourAttach((PVOID*)&RealRegisterClassA,
		//			hook_RegisterClassA);
		//	}

		//	if (register_class_w_proc)
		//	{
		//		RealRegisterClassW = (PFN_RegisterClassW)register_class_w_proc;
		//		DetourAttach((PVOID*)&RealRegisterClassW,
		//			hook_RegisterClassW);
		//	}*/

		//	SYSTEM_LOG("   input end  ... ");
		//	const LONG error = DetourTransactionCommit();
		//	const bool success = error == NO_ERROR;
		//	if (success)
		//	{
		//		NORMAL_EX_LOG("  input device");
		//		if (get_raw_input_data_proc)
		//		{
		//			NORMAL_EX_LOG("  input device");
		//		}
		//		if (get_cursor_pos_proc)
		//		{
		//			NORMAL_EX_LOG("  input device");
		//		}
		//		if (set_cursor_pos_proc)
		//		{
		//			NORMAL_EX_LOG("  input device");
		//		}
		//		/*if (register_class_a_proc)
		//		{
		//			NORMAL_EX_LOG(" input device");
		//		}
		//		if (register_class_w_proc)
		//		{
		//			NORMAL_EX_LOG(" input device");
		//		}*/
		//		NORMAL_EX_LOG("  input device ");
		//	}
		//	else
		//	{
		//		RealGetRawInputData = NULL;
		//		RealGetCursorPos = NULL;
		//		RealSetCursorPos = NULL;
		//		/*RealRegisterClassA = NULL;
		//		RealRegisterClassW = NULL;*/
		//		ERROR_EX_LOG("Failed to attach  mouse  : %ld", error);
		//	}
		//}

		

		REGISTER_INPUT_DEVICE(RequestQualityControl, &cinput_device::OnKeyChar);
		REGISTER_INPUT_DEVICE(KeyDown, &cinput_device::OnKeyDown);
		REGISTER_INPUT_DEVICE(KeyUp, &cinput_device::OnKeyUp);
		REGISTER_INPUT_DEVICE(KeyPress, &cinput_device::OnKeyPress);
		////////////////////////////////////////////////////////////
		REGISTER_INPUT_DEVICE(MouseEnter, &cinput_device::OnMouseEnter);
		REGISTER_INPUT_DEVICE(MouseLeave, &cinput_device::OnMouseLeave);
		REGISTER_INPUT_DEVICE(MouseDown, &cinput_device::OnMouseDown);
		REGISTER_INPUT_DEVICE(MouseUp, &cinput_device::OnMouseUp);
		REGISTER_INPUT_DEVICE(MouseMove, &cinput_device::OnMouseMove);
		REGISTER_INPUT_DEVICE(MouseWheel, &cinput_device::OnMouseWheel);
		REGISTER_INPUT_DEVICE(MouseDoubleClick, &cinput_device::OnMouseDoubleClick);
		return true;
	}
	void cinput_device::Destroy()
	{
		m_input_device.clear();
		m_all_consumer.clear();
	}
	bool cinput_device::set_point(uint32 x, uint32 y)
	{
		m_int_point.X = x;
		m_int_point.Y = y;
		return true;
	}
	bool cinput_device::OnMessage(const std::string & consumer_id, const webrtc::DataBuffer& Buffer)
	{
		//NORMAL_LOG("consumer_id = %s", consumer_id.c_str());
		m_mouse_id = consumer_id;
		const uint8* Data = Buffer.data.data();
		uint32 Size = static_cast<uint32>(Buffer.data.size());

		GET(EToStreamMsg, MsgType);
		
		M_INPUT_DEVICE_MAP::iterator iter =  m_input_device.find(MsgType);
		if (iter == m_input_device.end())
		{
			//RTC_LOG(LS_ERROR) << "input_device not find id = " << MsgType;
			//log --->  not find type 
			ERROR_EX_LOG("input_device msg_type = %d not find failed !!!", MsgType);
			return false;
		}
		
		/*if (MsgType != MouseEnter)
		{
			std::map<std::string, std::map<uint32, cmouse_info>>::const_iterator consumer_iter = m_all_consumer.find(consumer_id);
			if (consumer_iter == m_all_consumer.end())
			{
				WARNING_EX_LOG("consumer_id = %s not Enter !!!", consumer_id.c_str());
				return false;
			}

		} */
		//NORMAL_EX_LOG("move type =%d", MsgType);
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::microseconds microseconds;
		uint32_t elapse = 0;
		//备往：尽管GetFocus返回NULL，但可能另一线程的队列与拥有输入焦点的窗口相关。
		//便用GetForeyroundWindow函数来获得用户目前工作的窗口。
		//可以使用AttachThreadlnPut函数把线程的消息队列与另一线程的窗口关联起来。
		

		//HWND wnd = GetFocus();





		
		   //说明：SetForegroundWindow在debug模式一直成功，非debug模式会有失败的情况，解决方法是利用AttachThreadInput
        //AttachThreadInput这个函数可以使两个线程的输入队列共享。 
        //如果我们把当前的焦点的输入队列跟我们要显示的窗口的输入队列共享，
        //我们就可以让我们的窗口SetForegroundWindow 成功，然后成功 获得焦点，显示在前台
		/*
		HWND hForgroundWnd = GetForegroundWindow(); \
						DWORD dwForeID = ::GetWindowThreadProcessId(hForgroundWnd, NULL); \
	DWORD dwCurID = ::GetCurrentThreadId();		\
	::AttachThreadInput(dwCurID, dwForeID, TRUE);	\
	::ShowWindow(g_wnd, SW_SHOWNORMAL);		\
	::SetForegroundWindow(g_wnd);	\
	::AttachThreadInput(dwCurID, dwForeID, FALSE);	\
		*/
       /*
	   SetWindowPos(handle, -1, 0, 0, 0, 0, 0x001 | 0x002 | 0x040);    //将这个窗口永远置于最上面
    //---------------------------在需要激活窗体的地方添加如下代码-----------------------//
    PostMessage(handle, WM_LBUTTONDOWN, 0, 0);
    PostMessage(handle, WM_LBUTTONUP, 0, 0);
	   */
		//BringWindowToTop();

		 (this->*(iter->second))(Data, Size);
		 cur_time_ms = std::chrono::steady_clock::now();
		 dur = cur_time_ms - pre_time;
		 microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dur);
		 elapse = static_cast<uint32_t>(microseconds.count());
		 if (elapse > 900)
		 {
			 WARNING_EX_LOG("input_device  microseconds = %lu", microseconds);
		 }
		 return true;
		//return true;
	}


	bool cinput_device::OnRequestQualityControl(const uint8*& Data, uint32 size)
	{
		// 这边需要启动引擎推送视频流
		return true;
	}
	/**
	* 输入字符
	*/
	bool cinput_device::OnKeyChar(const uint8*& Data,   uint32 size)
	{
		NORMAL_LOG("KeyChar");
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_CHAR, Character, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_CHAR, Character, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}

		return true;
	}
	/**
	* 按下键
	*/
	bool cinput_device::OnKeyDown(const uint8*& Data,  uint32 Size)
	{
		 
		GET(FKeyCodeType, KeyCode);
		GET(FRepeatType, Repeat);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key down: %d, repeat: %d"), KeyCode, Repeat);
		// log key down -> repeat keyCode Repeat 
		FEvent KeyDownEvent(EventType::KEY_DOWN);
		KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u", KeyCode, Repeat);
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		// TODO@chensong 2022-01-20  keydown -> keycode -> repeat 
		/*if (mwin)
		{
			::PostMessageW(mwin, WM_KEYDOWN, KeyCode, Repeat != 0);
		}*/
		
		
		SET_POINT();
		WINDOW_CHILD();
		if (childwin)
		{
			//PostMessageW
	        /*
			这个 KeyDown与 KeyUP 是有讲究哈 ^_^
			发送1次按键结果出现2次按键的情况
发送一次WM_KEYDOWN及一次WM_KEYUP结果出现了2次按键，原因是最后一个参数lParam不规范导致，此参数0到15位为该键在键盘上的重复次数，经常设为1，即按键1次；16至23位为键盘的扫描码，通过API函数MapVirtualKey可以得到；24位为扩展键，即某些右ALT和CTRL；29一般为0；30位-[原状态]已按下为1否则0（KEYUP要设为1）；31位-[状态切换]（KEYDOWN设为0，KEYUP要设为1）。
资料显示第30位对于keydown在和shift等结合的时候通常要设置为1，未经验证。


			*/
			MESSAGE(childwin, WM_KEYDOWN, KeyCode, 0);
		}
		else if (mwin)
		{
			MESSAGE(mwin, WM_KEYDOWN, KeyCode, 0);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
		#endif //#if defined(_MSC_VER)
		//KeyDownEvent.GetKeyDown();
		//ProcessEvent(KeyDownEvent);
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_KEYDOWN, KeyCode, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_KEYDOWN, KeyCode, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
		return true;
	}

	/**
	* 松开键
	*/
	bool cinput_device::OnKeyUp(const uint8*& Data,   uint32 Size)
	{
		GET(FKeyCodeType, KeyCode);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key up: %d"), KeyCode);
		// log key up -> KeyCode
		FEvent KeyUpEvent(EventType::KEY_UP);
		KeyUpEvent.SetKeyUp(KeyCode);
		NORMAL_LOG("OnKeyUp==KeyCode = %u", KeyCode);
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		
		
		SET_POINT();
		WINDOW_CHILD();
		if (childwin)
		{
			MESSAGE(childwin, WM_KEYUP, KeyCode, 1);
		}
		else if (mwin)
		{
			MESSAGE(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
		#endif //#if defined(_MSC_VER)
		//ProcessEvent(KeyUpEvent);
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
		return true;
	}
	/**
	*keydown：按下键盘键
	*	keypress：紧接着keydown事件触发（只有按下字符键时触发）
	*	keyup：释放键盘键
	*/
	bool cinput_device::OnKeyPress(const uint8*& Data,   uint32 Size)
	{
		// TODO@chensong 2022-01-20  KeyPress -->>>>> net

		GET(FCharacterType, Character);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("key up: %d"), KeyCode);
		// log key up -> KeyCode
		FEvent KeyUpEvent(EventType::KEY_PRESS);
		KeyUpEvent.SetKeyUp(Character);
		NORMAL_LOG("OnKeyPress==KeyCode = %u", Character);
#if defined(_MSC_VER)
		WINDOW_MAIN();


		
		SET_POINT();
		WINDOW_CHILD();
		if (childwin)
		{
			MESSAGE(childwin, WM_CHAR, Character, 1);
		}
		else if (mwin)
		{
			//MESSAGE(mwin, WM_PR, Character, 0);
			MESSAGE(mwin, WM_CHAR, Character, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
#endif //#if defined(_MSC_VER)
		//ProcessEvent(KeyUpEvent);
		//WINDOW_MAIN();
		//SET_POINT(vec);
		//WINDOW_CHILD();
		//if (childwin)
		//{
		//	::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		//}
		//else if (mwin)
		//{
		//	::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
		return true;
	}

	bool cinput_device::OnMouseEnter(const uint8*& Data,   uint32 size)
	{
		// TODO@chensong 2022-01-20  OnMouseEnter -->>>>> net 
		/*if (!m_all_consumer.insert(std::make_pair(m_mouse_id, std::map<uint32, cmouse_info>())).second)
		{
			WARNING_EX_LOG("mouse enter insert [mouse_id = %s] failed !!!", m_mouse_id.c_str());
			return false;
		}*/
		NORMAL_EX_LOG("OnMouseEnter===>");
 #if defined(_MSC_VER)
		 
		//WINDOW_MAIN();
		//

		// 
		//if (mwin)
		//{
		//	TRACKMOUSEEVENT tme;
		//	tme.cbSize = sizeof(tme);	//结构体缓冲区大小
		//	tme.dwFlags = TME_HOVER;	//注册WM_MOUSEHOVER消息
		//	tme.dwHoverTime = 1; //WM_MOUSEHOVER消息触发间隔时间
		//	tme.hwndTrack = mwin; //当前窗口句柄
		//	::TrackMouseEvent(&tme); //注册发送消息
		//	//MESSAGE(mwin, WM_MOUSEHOVER, 0, 0);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}
 
#endif // _MSC_VER


		return true;
		 
	}

	/** 
	*鼠标离开控制范围？
	*/
	bool cinput_device::OnMouseLeave(const uint8*& Data,   uint32 size)
	{
		// TODO@chensong 2022-01-20  OnMouseLeave 
		std::map<std::string, std::map<uint32, cmouse_info>>::iterator iter =  m_all_consumer.find(m_mouse_id);
		/*if (iter == m_all_consumer.end())
		{
			WARNING_EX_LOG("mouse leave  not find  [mouse_id = %s] failed !!!", m_mouse_id.c_str());
			return false;
		}
		iter->second.clear();
		m_all_consumer.erase(iter);*/

		//std::map<uint32, cmouse_info>& cmouse_info = iter->second;
		NORMAL_EX_LOG("OnMouseLeave===>");
#if defined(_MSC_VER)

		//WINDOW_MAIN();
		// 
		//if (mwin)
		//{
		//	TRACKMOUSEEVENT tme;
		//	tme.cbSize = sizeof(tme);	//结构体缓冲区大小
		//	tme.dwFlags = TME_LEAVE;	//注册WM_MOUSEHOVER消息
		//	tme.dwHoverTime = 1; //WM_MOUSEHOVER消息触发间隔时间
		//	tme.hwndTrack = mwin; //当前窗口句柄
		//	::TrackMouseEvent(&tme); //注册发送消息
		//	//MESSAGE(mwin, WM_MOUSEHOVER, 0, 0);
		//}
		//else
		//{
		//	// log -> error 
		//	return false;
		//}

#endif // _MSC_VER
		return true; 
	}
 
	


	/**
	* 鼠标按下 
	* right <-> left 
	*/
	bool cinput_device::OnMouseDown(const uint8*& Data,   uint32 Size)
	{

		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseDown at (%d, %d), button %d"), PosX, PosY, Button);
		// log mousedown -> log posX , poxY -> Button 
		//NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		//ProcessEvent(MouseDownEvent);
		
		g_width = PosX;
		g_height = PosY;
		
		/*
		PosX = g_width;
		PosY = g_height;*/
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY );
		//g_move_init = true;
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		//MOUSE_INPUT(mwin);
		/*registerinputdevice(mwin);
		SetForegroundWindow(mwin);
		SetActiveWindow(mwin);*/
		g_main_mouse_down_up = mwin;
		SET_POINT();
		//WINDOW_CHILD();
		//WINDOW_BNTTON_DOWN(vec);
		/*if (mwin)
		{
		::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));
		}*/

		if (mwin)
		{
			CliENTTOSCREENPOINT(mwin, PosX, PosY);
			MESSAGE(mwin, active_type, 1/*MAKEWPARAM(0,0)*/, MAKELPARAM(CursorPoint.x, CursorPoint.y));//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		}
		#endif//#if defined(_MSC_VER)
		return true;
	
	}

	/** 
	*松开鼠标
	* 
	*/
	bool cinput_device::OnMouseUp(const uint8*& Data,   uint32 Size)
	{
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseUp at (%d, %d), button %d"), PosX, PosY, Button);
		// log mouseup posx, posy, button 
		//NORMAL_EX_LOG("Button = %u, PosX = %d, PoxY = %d ", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d", PosX, PosY );
		FEvent MouseDownEvent(EventType::MOUSE_UP);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32  active_type;
		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		
		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height,  active_type, PosX, PosY );
		//ProcessEvent(MouseDownEvent);
		//g_move_init = false;
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		//MOUSE_INPUT(mwin);
		/*registerinputdevice(mwin);
		SetForegroundWindow(mwin);
		SetActiveWindow(mwin);*/
		/*SET_POINT();
		WINDOW_CHILD();*/
		//WINDOW_BNTTON_UP(vec);
		
		if (mwin)
		{
			{
				CliENTTOSCREENPOINT(mwin, PosX, PosY);
				MESSAGE(mwin, active_type, MAKEWPARAM(0, 0), MAKELPARAM(CursorPoint.x, CursorPoint.y));
			}//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
			
			
		}
		/*if (mwin)
		{
			::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM( PosX, PosY));
		}*/
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		} 
		#endif //#if defined(_MSC_VER)
		return true;
	}



	/** 
	*鼠标移动
	*/
	bool cinput_device::OnMouseMove(const uint8*& Data,   uint32 Size)
	{
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		GET(FDeltaType, DeltaX);
		GET(FDeltaType, DeltaY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseMove to (%d, %d), delta (%d, %d)"), PosX, PosY, DeltaX, DeltaY);
		// log mousemove to posx, posy, [DeltaX, DeltaY]
		//NORMAL_EX_LOG("PosX = %d, PoxY = %d, DeltaY = %d", PosX, PosY, DeltaY);
		
		//RTC_LOG(LS_INFO) << "mousemove -->  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		_UnquantizeAndDenormalize(PosX, PosY);
		_UnquantizeAndDenormalize(DeltaX, DeltaY);
		//RTC_LOG(LS_INFO) << "mousemove <==>  PosX = " << PosX << ", PoxY = " << PosY << ", DeltaY = " << DeltaY;
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d, DeltaY = %d", g_width, g_height, PosX, PosY, DeltaY);
		
		FEvent MouseMoveEvent(EventType::MOUSE_MOVE);
		MouseMoveEvent.SetMouseDelta(PosX, PosY, DeltaX, DeltaY);
		int32_t width = g_width;
		int32_t height = g_height;
		g_width = PosX;
		g_height = PosY;

		/*
		PosX = g_width;
		PosY = g_height;*/
		
		#if defined(_MSC_VER)

		WINDOW_MAIN();
		//MOUSE_INPUT(mwin);
		/*registerinputdevice(mwin);
		SetForegroundWindow(mwin);
		SetActiveWindow(mwin);*/
		//WINDOW_BNTTON_UP(vec);
		
		if (mwin)
		{
			//TRACKMOUSEEVENT tme;
			//tme.cbSize = sizeof(tme);	//结构体缓冲区大小
			//tme.dwFlags = TME_HOVER;	//注册WM_MOUSEHOVER消息
			//tme.dwHoverTime = 1; //WM_MOUSEHOVER消息触发间隔时间
			//tme.hwndTrack = mwin; //当前窗口句柄
			//::TrackMouseEvent(&tme); //注册发送消息

			POINT CursorPoint;
			CursorPoint.x = PosX;
			CursorPoint.y = PosY;
			::ClientToScreen(mwin, &CursorPoint);
			//::SetFocus(mwin);
			//INPUT input[4];
			//hwndConsole = GetConsoleWindow();
			//hwndTX = FindWindow(TEXT("TXGuiFoundation"), TEXT("N3verL4nd"));
			/*if (hwndTX != NULL)
			{
				for (int i = 0; i < 10; i++)
				{
					SendMessage(hwndTX, WM_CHAR, 'A', 0);
				}*/

				//SetForegroundWindow(hwndTX);
				//memset(input, 0, sizeof(input));
				/*input[0].type = input[1].type = input[2].type = input[3].type = INPUT_KEYBOARD;
				input[0].ki.wVk = input[2].ki.wVk = VK_MENU;
				input[1].ki.wVk = input[3].ki.wVk = 0x53;
				input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;*/
				//SendInput(4, input, sizeof(INPUT));
				//SetForegroundWindow(hwndConsole);
			/*{
				INPUT Input = { 0 };
				Input.type = INPUT_MOUSE;
				Input.mi.dx = CursorPoint.x;
				Input.mi.dy = CursorPoint.y;
				Input.mi.mouseData = 0;
				Input.mi.dwFlags = MOUSEEVENTF_MOVE;
				Input.mi.time = 0;
				Input.mi.dwExtraInfo = 0;
				SendInput(1, &Input, sizeof(INPUT));
			}*/
			// TODO@chensong 20220612 完全为了兼容Win的input设备
			//mouse_event();
			//
			//GetDesktopWindow()->GetActiveWindow() 
				//GetForegroundWindow();
			//SetForegroundWindow(mwin);
			/*INPUT input;
			input.type = INPUT_HARDWARE;
			input.hi.uMsg = WM_INPUT;
			input.hi.wParamL = MAKEWPARAM(DeltaX, DeltaY);
			input.hi.wParamH = MAKELPARAM(CursorPoint.x, CursorPoint.y);*/
			//static int x = 0;
			//static int y = 0;
			//int xx = CursorPoint.x ;
			//int yy = CursorPoint.y ;
			//input.mi.dx = xx - x;
			//input.mi.dy = yy - y;
			//x = xx;
			//y = yy;
			//input.mi.mouseData = 0;
			//input.mi.dwFlags = /*MOUSEEVENTF_ABSOLUTE |*/ MOUSEEVENTF_MOVE;   //MOUSEEVENTF_ABSOLUTE 代表决对位置  MOUSEEVENTF_MOVE代表移动事件
			//input.mi.time = 0;
			//input.mi.dwExtraInfo = 0;
			//SendInput(1, &input, sizeof(INPUT));

			//::SetCursorPos(CursorPoint.x, CursorPoint.y);
		//	MESSAGE(mwin, WM_INPUT/*WM_INPUT 、 WM_NCMOUSEMOVE UE4 move dug TODO@chensong 20220611 */ /*WM_MOUSEMOVE*/, MAKEWPARAM(DeltaX, DeltaY), MAKELPARAM(CursorPoint.x, CursorPoint.y));
		//try sending 'W' 
			RAWINPUT raw = { 0 };
			//char c = 'W';
			//header 
			/*raw.header.dwSize = sizeof(raw);
			raw.header.dwType = RIM_TYPEMOUSE;
			raw.header.wParam = MAKEWPARAM(DeltaX, DeltaY);*/ //(wParam & 0xff =0 => 0) 
			//raw.header.hDevice = hDevice;
			//data 
			//raw.data.mouse.lLastX = DeltaX;
			//raw.data.mouse.lLastY = DeltaY;
			//raw.data.mouse.usFlags
			//raw.data.keyboard.Reserved = 0;
			//raw.data.keyboard.Flags = RI_KEY_MAKE;  //Key down 
			//raw.data.keyboard.MakeCode = static_cast<WORD>(MapVirtualKeyEx(c, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
			//raw.data.keyboard.Message = WM_KEYDOWN;
			//raw.data.keyboard.VKey = VkKeyScanEx(c, GetKeyboardLayout(0));
			//raw.data.keyboard.ExtraInformation = 0;   //??? 			//HGLOBAL raw_input_ptr = ::GlobalAlloc(GHND, sizeof(RAWINPUT));
			// *pRaw = reinterpret_cast<RAWINPUT*>(::GlobalLock(raw_input_ptr));
			//if (pRaw)
			{
				//char c = 'W';
			//header
				RAWINPUT pRaw;
				pRaw.header.dwSize = sizeof(RAWINPUT);
				pRaw.header.dwType = RIM_TYPEMOUSE;
				pRaw.header.wParam = 0; //(wParam & 0xff =0 => 0)
				pRaw.header.hDevice = 0;

				//data
				pRaw.data.mouse.lLastX = g_width - width ;
				pRaw.data.mouse.lLastY =    g_height - height;
				//pRaw->data.keyboard.Reserved = 0;
				//pRaw->data.keyboard.Flags = RI_KEY_MAKE;
				//pRaw->data.keyboard.MakeCode = static_cast<WORD>(MapVirtualKeyEx(c, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
				//pRaw->data.keyboard.Message = WM_KEYDOWN;
				//pRaw->data.keyboard.VKey = VkKeyScanEx(c, GetKeyboardLayout(0));
				//pRaw->data.keyboard.ExtraInformation = 0;
				//memcpy(g_hrawinput[g_write_index % RAW_INPUT_SIZE], &pRaw, sizeof(RAWINPUT));
				//g_hrawinput[g_write_index % RAW_INPUT_SIZE] = pRaw;
				
				 {
					clock_guard lock(g_mutex);
					/*if (g_hrawinput.size() > 300)
					{
						g_hrawinput.clear();
					}*/
					//g_hrawinput.push_back(pRaw);
					//DEBUG_LOG("MAKELPARAM(CursorPoint.x, CursorPoint.y) = %u", MAKELPARAM(CursorPoint.x, CursorPoint.y));
					g_hrawinput[MAKELPARAM(CursorPoint.x, CursorPoint.y)] = pRaw;
					//g_write_index++;
				}
				//::GlobalUnlock(raw_input_ptr);

				//UINT dataSz{ 0 };
				//Send the message ///*Raw input handle*/
				MESSAGE(mwin, WM_INPUT, (WPARAM)RIM_INPUT, MAKELPARAM(CursorPoint.x, CursorPoint.y));  //TODO: Handle to raw input 
			}
			
			
			MESSAGE(mwin, WM_MOUSEMOVE /*WM_MOUSEMOVE*//*WM_INPUT 、 WM_NCMOUSEMOVE UE4 move dug TODO@chensong 20220611 */ /*WM_MOUSEMOVE*/, MAKEWPARAM(DeltaX, DeltaY) , MAKELPARAM(CursorPoint.x, CursorPoint.y ));
			//MESSAGE(mwin, WM_INPUT /*WM_MOUSEMOVE*//*WM_INPUT 、 WM_NCMOUSEMOVE UE4 move dug TODO@chensong 20220611 */ /*WM_MOUSEMOVE*/, MAKEWPARAM(DeltaX, DeltaY), MAKELPARAM(CursorPoint.x, CursorPoint.y));


			//WM_MOUSEHOVER
			//WM_MOUSELEAVE
			//RAWINPUT raw = { 0 };
			//char c = 'W';
			////header
			//raw.header.dwSize = sizeof(raw);
			//raw.header.dwType = RIM_TYPEKEYBOARD;
			//raw.header.wParam = 0; //(wParam & 0xff =0 => 0)
			//raw.header.hDevice = hDevice;

			//data
			//raw.data.keyboard.Reserved = 0;
			//raw.data.keyboard.Flags = RI_KEY_MAKE;      //Key down
			//raw.data.keyboard.MakeCode = static_cast<WORD>(MapVirtualKeyEx(c, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
			//raw.data.keyboard.Message = WM_KEYDOWN;
			//raw.data.keyboard.VKey = VkKeyScanEx(c, GetKeyboardLayout(0));
			//raw.data.keyboard.ExtraInformation = 0;         //???

			//Send the message
			//SendMessage(mwin, WM_INPUT, 0, (LPARAM)raw/*Raw input handle*/);
		}
		else
		{
			// log -> error 
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
		#endif // #if defined(_MSC_VER)
		//ProcessEvent(MouseMoveEvent);
		return true;
	}
	/** 
	* 鼠标双击
	*/
	bool cinput_device::OnMouseDoubleClick(const uint8*& Data,   uint32 Size)
	{
		//WM_LBUTTONDBLCLK
		GET(FButtonType, Button);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseDown at (%d, %d), button %d"), PosX, PosY, Button);
		// log mousedown -> log posX , poxY -> Button 
		//NORMAL_EX_LOG("Button = %d, PosX = %d, PoxY = %d", Button, PosX, PosY );
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseDownEvent(EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		uint32 active_type;

		MouseDownEvent.GetMouseClick(active_type, PosX, PosY);
		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		//ProcessEvent(MouseDownEvent);
		NORMAL_EX_LOG("g_width = %d, g_height = %d, ---> PosX = %d, PoxY = %d", g_width, g_height, PosX, PosY);

		//NORMAL_EX_LOG("active_type = %d, PosX = %d, PoxY = %d", active_type, PosX, PosY );
#if defined(_MSC_VER)
		WINDOW_MAIN();

		SET_POINT();
		WINDOW_CHILD();
		//WINDOW_BNTTON_DOWN(vec);
		/*if (mwin)
		{
		::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));
		}*/
		if (childwin)
		{
			CliENTTOSCREENPOINT(childwin, PosX, PosY);
			MESSAGE(childwin, WM_LBUTTONDBLCLK, MAKEWPARAM(0, 0), MAKELPARAM(CursorPoint.x, CursorPoint.y));//::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		}
		else if (mwin)
		{
			CliENTTOSCREENPOINT(mwin, PosX, PosY);
			MESSAGE(mwin, WM_LBUTTONDBLCLK, MAKEWPARAM(0, 0), MAKELPARAM(CursorPoint.x, CursorPoint.y));//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		}
#endif//#if defined(_MSC_VER)
		 
		return true;
	}

	/** 
	* 鼠标的滚轮滚动 
	*/
	bool cinput_device::OnMouseWheel(const uint8*& Data,   uint32 Size)
	{
		GET(FDeltaType, Delta);
		GET(FPosType, PosX);
		GET(FPosType, PosY);
		checkf(Size == 0, TEXT("%d"), Size);
		//UE_LOG(PixelStreamerInput, Verbose, TEXT("mouseWheel, delta %d"), Delta);
		// mouseWheel delta -> 
		_UnquantizeAndDenormalize(PosX, PosY);

		FEvent MouseWheelEvent(EventType::MOUSE_WHEEL);
		MouseWheelEvent.SetMouseWheel(Delta, PosX, PosY);
		//ProcessEvent(MouseWheelEvent);
		g_width = PosX;
		g_height = PosY;
		/*PosX = g_width;
		PosY = g_height;*/
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		NORMAL_EX_LOG(" PosX = %d, PoxY = %d", PosX, PosY);
		if (mwin)
		{
			CliENTTOSCREENPOINT(mwin, PosX, PosY);
			//::PostMessage(mwin, WM_MOUSEWHEEL, MAKEWPARAM(0, Delta) /* ascii码 */, MAKELPARAM(PosX, PosY));
			MESSAGE(mwin, WM_MOUSEWHEEL, MAKEWPARAM(0, Delta) /* ascii码 */, MAKELPARAM(CursorPoint.x, CursorPoint.y));
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log error 
			return false;
		}
		#endif // #if defined(_MSC_VER)
		return true;
	}

	// XY positions are the ratio (0.0..1.0) along a viewport axis, quantized
	// into an uint16 (0..65536). This allows the browser viewport and player
	// viewport to have a different size.
	void cinput_device::_UnquantizeAndDenormalize(uint16& InOutX, uint16& InOutY)
	{
		InOutX = InOutX / 65536.0f * m_int_point.X;
		InOutY = InOutY / 65536.0f * m_int_point.Y;
	}
	
	// XY deltas are the ratio (-1.0..1.0) along a viewport axis, quantized
	// into an int16 (-32767..32767). This allows the browser viewport and
	// player viewport to have a different size.
	void cinput_device::_UnquantizeAndDenormalize(int16& InOutX, int16& InOutY)
	{ 
		InOutX = InOutX / 32767.0f * m_int_point.X;
		InOutY = InOutY / 32767.0f * m_int_point.Y;
	}
}