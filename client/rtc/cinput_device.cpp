/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
************************************************************************************************/

#include "cinput_device.h"
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include "cprotocol.h"
#include "cinput_device_event.h"
#include "rtc_base/logging.h"
#include "clog.h"
#include "cinput_device_hook.h"
#include "input_helper.h"
#if defined(_MSC_VER)
#include <Windows.h>
#include "cwindow_util.h"
#endif // WIN
namespace chen {
	int32_t  g_width = 150;
	int32_t  g_height = 150;
	using FKeyCodeType = uint8;
    	#ifdef _MSC_VER
	static HWND g_main_mouse_down_up = NULL;
	static HWND g_child_mouse_down_up = NULL;
    using FCharacterType = TCHAR;
#elif defined(__GNUC__) ||defined(__APPLE__)
	static void * g_main_mouse_down_up = NULL;
	static void * g_child_mouse_down_up = NULL;
    using FCharacterType = TCHAR;
#else
// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
    
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

#if defined(_MSC_VER)
	////PostMessage
#define MESSAGE(g_wnd, message_id, param1, param2) MSG msg;  \
														  msg.hwnd = g_wnd;			\
												          msg.message = message_id;	\
														  msg.wParam = param1;		\
														  msg.lParam = param2;		\
			std::chrono::steady_clock::time_point cur_time_ms = std::chrono::steady_clock::now();     \
											long long ms = static_cast<long long >(cur_time_ms.time_since_epoch().count() / 1000000); \
												 msg.time = static_cast<DWORD>(ms);	\
														  msg.pt.x = g_width; msg.pt.y = g_height; \
														::TranslateMessage(&msg);         \
													long long ret_dispatch =	 ::DispatchMessage(&msg); 
													//NORMAL_EX_LOG("move cur_ms = %u, [ret_dispatch = %s]", ms, std::to_string(ret_dispatch).c_str());


//#define MESSAGE(g_wnd, message_id, param1, param2) PostMessage(g_wnd, message_id, param1, param2);
	//使用全局变量操作的哈 
#define SET_POINT() POINT pt; pt.x = g_width; pt.y = g_height;


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

#define REGISTER_KEYDOWN(key, value) m_keydown_map.insert(std::make_pair(key, value));
    static bool hook_input_device_logger(unsigned int level, const char *format, ...)
    {
        bool status = false;

        va_list args;
        switch (level) {
            case LOG_LEVEL_INFO:
            {
                va_start(args, format);
                status = vfprintf(stdout, format, args) >= 0;
                va_end(args);
                break;
            }

            case LOG_LEVEL_WARN:
            {
                break;
            }
            case LOG_LEVEL_ERROR:
            {
                va_start(args, format);
                status = vfprintf(stderr, format, args) >= 0;
                va_end(args);
                break;
            }
            default:
            {//ERROR_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)
//                WARNING_EX_LOG(f);
                break;
            }
        }

        return true;
    }

///	cinput_device   g_input_device_mgr;
	cinput_device::cinput_device() 
		:  m_input_device()
		,  m_int_point(){}
	cinput_device::~cinput_device() {}

 
	//static bool g_move_init = false;
	bool cinput_device::init()
	{
        SYSTEM_LOG(" input device hook set log  ...");
        hook_set_logger_proc(hook_input_device_logger);
        SYSTEM_LOG(" input device load ...");
        load_input_device();
        SYSTEM_LOG(" input device register mouse key ...");
		REGISTER_INPUT_DEVICE(RequestQualityControl, &cinput_device::OnKeyChar);
		REGISTER_INPUT_DEVICE(KeyDown, &cinput_device::OnKeyDown);
		REGISTER_INPUT_DEVICE(KeyUp, &cinput_device::OnKeyUp);
//		REGISTER_INPUT_DEVICE(KeyPress, &cinput_device::OnKeyPress);
		////////////////////////////////////////////////////////////
		REGISTER_INPUT_DEVICE(MouseEnter, &cinput_device::OnMouseEnter);
		REGISTER_INPUT_DEVICE(MouseLeave, &cinput_device::OnMouseLeave);
		REGISTER_INPUT_DEVICE(MouseDown, &cinput_device::OnMouseDown);
		REGISTER_INPUT_DEVICE(MouseUp, &cinput_device::OnMouseUp);
		REGISTER_INPUT_DEVICE(MouseMove, &cinput_device::OnMouseMove);
		REGISTER_INPUT_DEVICE(MouseWheel, &cinput_device::OnMouseWheel);
		REGISTER_INPUT_DEVICE(MouseDoubleClick, &cinput_device::OnMouseDoubleClick);
        SYSTEM_LOG("  input device init ok !!!");
		return true;
	}
	void cinput_device::Destroy()
	{
		m_input_device.clear();
        SYSTEM_LOG("input device register mouse destroy ok !!!");
		m_all_consumer.clear();
        unload_input_device();
        SYSTEM_LOG("input device unload destroy ok !!!");

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
#elif defined(__linux__)
        static uiohook_event event  ;
        event.type = EVENT_KEY_PRESSED;
        event.mask = 0X200;
//        event.data.keyboard.keychar = CHAR_UNDEFINED;
        unsigned char  code =  XKeysymToKeycode(helper_disp, KeyCode);
//        unsigned char code = XKeysymToKeycode(helper_disp, XStringToKeysym(KeyCode));
        NORMAL_EX_LOG("code = %u", code);
        event.data.keyboard.keycode = code;
        event.data.keyboard.keychar = KeyCode;// VC_ESCAPE;
        event.data.keyboard.rawcode = KeyCode;
        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
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
#elif defined(__linux__)
        static uiohook_event event;
        event.type = EVENT_KEY_RELEASED;
        event.mask = 0X200;
//        event.data.keyboard.keychar = CHAR_UNDEFINED;
//        unsigned char  code =  XKeysymToKeycode(helper_disp, KeyCode);
        unsigned char  code =  XKeysymToKeycode(helper_disp, KeyCode);
//        unsigned char code = XKeysymToKeycode(helper_disp, XStringToKeysym(KeyCode));
        NORMAL_EX_LOG("code = %u", code);
        event.data.keyboard.keycode = code;
        event.data.keyboard.keychar = KeyCode;// VC_ESCAPE;
        event.data.keyboard.rawcode = KeyCode;
        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
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
		// log key up -> KeyCode222
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
#elif defined(__linux__)
        static uiohook_event event;
        event.type = EVENT_KEY_TYPED;
        event.mask = 0x00;
        event.data.keyboard.keychar = Character;
        event.data.keyboard.keycode = Character;// VC_ESCAPE;
        event.data.keyboard.keycode = Character;
        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
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
		/*
		g_width = PosX;
		g_height = PosY;
		*/
		
		 g_width = PosX ;
		 g_height = PosY ;
		NORMAL_EX_LOG("g_width = %d, g_height = %d, active_type = %d, PosX = %d, PoxY = %d", g_width, g_height, active_type, PosX, PosY );
		//g_move_init = true;
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		g_main_mouse_down_up = mwin;
		SET_POINT();
		//WINDOW_CHILD();
		//WINDOW_BNTTON_DOWN(vec);
		/*if (mwin)
		{
		::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));
		}*/

		if (g_main_mouse_down_up)
		{
			MESSAGE(g_main_mouse_down_up, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		}
#elif defined(__linux__)
        static uiohook_event event;
        event.type = EVENT_MOUSE_PRESSED;
        event.data.mouse.button = active_type; //MOUSE_BUTTON1;
        event.data.mouse.x = PosX;
        event.data.mouse.y = PosY;

        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
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
		/*WINDOW_MAIN();
		SET_POINT();
		WINDOW_CHILD();*/
		//WINDOW_BNTTON_UP(vec);
		
		if (g_main_mouse_down_up)
		{
			{
				MESSAGE(g_main_mouse_down_up, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));
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
#elif defined(__linux__)
        static uiohook_event event;
        event.type = EVENT_MOUSE_RELEASED;
        event.data.mouse.button = active_type; //MOUSE_BUTTON1;
        event.data.mouse.x = PosX;
        event.data.mouse.y = PosY;

        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
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
		
		g_width = PosX;
		g_height = PosY;
		/*
		PosX = g_width;
		PosY = g_height;*/
		
		#if defined(_MSC_VER)
		//WINDOW_MAIN();
		//WINDOW_BNTTON_UP(vec);
		
		if (g_main_mouse_down_up)
		{
			
			MESSAGE(g_main_mouse_down_up, WM_MOUSEMOVE, MAKEWPARAM(DeltaX, DeltaY), MAKEWPARAM(PosX, PosY));
			
		}
		else
		{
			// log -> error 
			WARNING_EX_LOG("not find main window failed !!!");
			return false;
		}
#elif defined(__linux__)
        static uiohook_event event;
        event.type = EVENT_MOUSE_MOVED;
        event.data.mouse.button = MOUSE_NOBUTTON; //MOUSE_BUTTON1;
        event.data.mouse.x = PosX;
        event.data.mouse.y = PosY;

        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
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
			MESSAGE(childwin, WM_LBUTTONDBLCLK, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));//::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		}
		else if (mwin)
		{
			MESSAGE(mwin, WM_LBUTTONDBLCLK, MAKEWPARAM(0, 0), MAKEWPARAM(PosX, PosY));//::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log -> error 
			return false;
		}
#elif defined(__linux__)
        static uiohook_event event;
        event.type = EVENT_MOUSE_CLICKED;
        event.data.mouse.button = active_type; //MOUSE_BUTTON1;
        event.data.mouse.x = PosX;
        event.data.mouse.y = PosY;

        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		 
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
		/*g_width = PosX;
		g_height = PosY;*/
		 g_width = PosX ;
		 g_height = PosY ;
		#if defined(_MSC_VER)
		WINDOW_MAIN();
		NORMAL_EX_LOG(" PosX = %d, PoxY = %d", PosX, PosY);
		if (mwin)
		{
			//::PostMessage(mwin, WM_MOUSEWHEEL, MAKEWPARAM(0, Delta) /* ascii码 */, MAKELPARAM(PosX, PosY));
			MESSAGE(mwin, WM_MOUSEWHEEL, MAKEWPARAM(0, Delta) /* ascii码 */, MAKELPARAM(PosX, PosY));
		}
		else
		{
			WARNING_EX_LOG("not find main window failed !!!");
			// log error 
			return false;
		}
#elif defined(__linux__)
        static uiohook_event event;
        event.type = EVENT_MOUSE_WHEEL;
        event.data.wheel.x = PosX;
        event.data.wheel.y = PosY;
        event.data.wheel.amount = Delta;
        event.data.wheel.rotation = Delta;
        hook_post_event(&event);
#else
        // 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
		return true;
	}

    bool cinput_device::init_keydown()
    {
//        m_keydown_map.insert()
//        REGISTER_KEYDOWN(0	, NUT);
//        REGISTER_KEYDOWN(1	, SOH);
//        REGISTER_KEYDOWN(2	, STX);
//        REGISTER_KEYDOWN(3	, ETX);
//        REGISTER_KEYDOWN(4	, EOT);
//        REGISTER_KEYDOWN(5	, ENQ);
//        REGISTER_KEYDOWN(6	, ACK);
//        REGISTER_KEYDOWN(7	, BEL);
//        REGISTER_KEYDOWN(8	, BS);
//        REGISTER_KEYDOWN(9	, HT);
//        REGISTER_KEYDOWN(10	, LF);
//        REGISTER_KEYDOWN(11	, VT);
//        REGISTER_KEYDOWN(12	, FF);
//        REGISTER_KEYDOWN(13	, CR);
//        REGISTER_KEYDOWN(14	, SO);
//        REGISTER_KEYDOWN(15	, SI);
//        REGISTER_KEYDOWN(16	, DLE);
//        REGISTER_KEYDOWN(17	, DCI);
//        REGISTER_KEYDOWN(18	, DC2);
//        REGISTER_KEYDOWN(19	, DC3);
//        REGISTER_KEYDOWN(20	, DC4);
//        REGISTER_KEYDOWN(21	, NAK);
//        REGISTER_KEYDOWN(22	, SYN);
//        REGISTER_KEYDOWN(23	, TB);
//        REGISTER_KEYDOWN(24	, CAN);
//        REGISTER_KEYDOWN(25	, EM);
//        REGISTER_KEYDOWN(26	, SUB);
//        REGISTER_KEYDOWN(27	, ESC);
//        REGISTER_KEYDOWN(32, (space);
//        REGISTER_KEYDOWN(	33,	!);
//        REGISTER_KEYDOWN(	34,	");
//        REGISTER_KEYDOWN(	35,	#);
//        REGISTER_KEYDOWN(	36,	$);
//        REGISTER_KEYDOWN(	37,	%);
//        REGISTER_KEYDOWN(	38,	&);
//        REGISTER_KEYDOWN(	39,	,);
//        REGISTER_KEYDOWN(	40,	();
//        REGISTER_KEYDOWN(	41,	));
//        REGISTER_KEYDOWN(	42,	*);
//        REGISTER_KEYDOWN(	43,	+);
//        REGISTER_KEYDOWN(	44,	,);
//        REGISTER_KEYDOWN(	45,	-);
//        REGISTER_KEYDOWN(	46,	.);
//        REGISTER_KEYDOWN(	47,	/);
        REGISTER_KEYDOWN(	48,	VC_0);
        REGISTER_KEYDOWN(	49,	VC_1);
        REGISTER_KEYDOWN(	50,	VC_2);
        REGISTER_KEYDOWN(	51,	VC_3);
        REGISTER_KEYDOWN(	52,	VC_4);
        REGISTER_KEYDOWN(	53,	VC_5);
        REGISTER_KEYDOWN(	54,	VC_6);
        REGISTER_KEYDOWN(	55,	VC_7);
        REGISTER_KEYDOWN(	56,	VC_8);
        REGISTER_KEYDOWN(	57,	VC_9);
//        REGISTER_KEYDOWN(	58,	:);
//        REGISTER_KEYDOWN(	59,	;);
//        REGISTER_KEYDOWN(64,	@);
//        REGISTER_KEYDOWN(65,	A);
//        REGISTER_KEYDOWN(66,	B);
//        REGISTER_KEYDOWN(67,	C);
//        REGISTER_KEYDOWN(68,	D);
//        REGISTER_KEYDOWN(69,	E);
//        REGISTER_KEYDOWN(70,	F);
//        REGISTER_KEYDOWN(71,	G);
//        REGISTER_KEYDOWN(72,	H);
//        REGISTER_KEYDOWN(73,	I);
//        REGISTER_KEYDOWN(74,	J);
//        REGISTER_KEYDOWN(75,	K);
//        REGISTER_KEYDOWN(76,	L);
//        REGISTER_KEYDOWN(77,	M);
//        REGISTER_KEYDOWN(78,	N);
//        REGISTER_KEYDOWN(79,	O);
//        REGISTER_KEYDOWN(80,	P);
//        REGISTER_KEYDOWN(81,	Q);
//        REGISTER_KEYDOWN(82,	R);
//        REGISTER_KEYDOWN(83,	S);
//        REGISTER_KEYDOWN(84,	T);
//        REGISTER_KEYDOWN(85,	U);
//        REGISTER_KEYDOWN(86,	V);
//        REGISTER_KEYDOWN(87,	W);
//        REGISTER_KEYDOWN(88,	X);
//        REGISTER_KEYDOWN(89,	Y);
//        REGISTER_KEYDOWN(90,	Z);
//        REGISTER_KEYDOWN(91,	[);
//        REGISTER_KEYDOWN(96,	、);
//        REGISTER_KEYDOWN(97,	a);
//        REGISTER_KEYDOWN(98,	b);
//        REGISTER_KEYDOWN(99,	c);
//        REGISTER_KEYDOWN(100,	d);
//        REGISTER_KEYDOWN(101,	e);
//        REGISTER_KEYDOWN(102,	f);
//        REGISTER_KEYDOWN(103,	g);
//        REGISTER_KEYDOWN(104,	h);
//        REGISTER_KEYDOWN(105,	i);
//        REGISTER_KEYDOWN(106,	j);
//        REGISTER_KEYDOWN(107,	k);
//        REGISTER_KEYDOWN(108,	l);
//        REGISTER_KEYDOWN(109,	m);
//        REGISTER_KEYDOWN(110,	n);
//        REGISTER_KEYDOWN(111,	o);
//        REGISTER_KEYDOWN(112,	p);
//        REGISTER_KEYDOWN(113,	q);
//        REGISTER_KEYDOWN(114,	r);
//        REGISTER_KEYDOWN(115,	s);
//        REGISTER_KEYDOWN(116,	t);
//        REGISTER_KEYDOWN(117,	u);
//        REGISTER_KEYDOWN(118,	v);
//        REGISTER_KEYDOWN(119,	w);
//        REGISTER_KEYDOWN(120,	x);
//        REGISTER_KEYDOWN(121,	y);
//        REGISTER_KEYDOWN(122,	z);
//        REGISTER_KEYDOWN(123,	{);

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