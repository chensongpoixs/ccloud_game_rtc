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

#include <Windows.h>

#endif // WIN
namespace chen {
	int32_t  g_width = 150;
	int32_t  g_height = 150;
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
													long long ret_dispatch =	 ::DispatchMessage(&msg); \
													NORMAL_EX_LOG("move cur_ms = %u, [ret_dispatch = %s]", ms, std::to_string(ret_dispatch).c_str());


//#define MESSAGE(g_wnd, message_id, param1, param2) PostMessage(g_wnd, message_id, param1, param2);
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

 
	//static bool g_move_init = false;
	bool cinput_device::init()
	{
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
		 (this->*(iter->second))(Data, Size);
		 cur_time_ms = std::chrono::steady_clock::now();
		 dur = cur_time_ms - pre_time;
		 microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dur);
		 elapse = static_cast<uint32_t>(microseconds.count());
		 NORMAL_EX_LOG("mouse microseconds = %lu", microseconds);
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
			MESSAGE(childwin, WM_KEYDOWN, KeyCode, 1);
		}
		else if (mwin)
		{
			MESSAGE(mwin, WM_KEYDOWN, KeyCode, 1);
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
	bool cinput_device::OnKeyPress(const uint8*& Data,   uint32 size)
	{
		// TODO@chensong 2022-01-20  KeyPress -->>>>> net 
		return false;
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
		
		PosX = g_width;
		PosY = g_height;
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
		/*g_width = PosX;
		g_height = PosY;*/
		PosX = g_width;
		PosY = g_height;
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