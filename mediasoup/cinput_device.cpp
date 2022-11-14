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
#include "cmediasoup_input_device_event.h"
#include "cclient.h"
#if defined(_MSC_VER)
// TODO@chensong 20220711  UE 鼠标控制权问题
#define _WIN32_WINNT 0x0400 
#include <Windows.h>
#include <WinUser.h>
#include <UserEnv.h>
#include <mutex>

#endif // WIN
namespace chen {
	 
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


	 


 
	 
#define REGISTER_INPUT_DEVICE(type, handler_ptr) if (false == m_input_device.insert(std::make_pair(type, handler_ptr)).second){	 ERROR_EX_LOG("[type = %s][handler_ptr = %s]", #type, #handler_ptr);	return false;}
	
 
	cinput_device::cinput_device(cclient * ptr) 
		:  m_input_device()
		,  m_int_point()
		,  m_client_ptr(ptr){}
	cinput_device::~cinput_device() {}
 
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
			WARNING_EX_LOG("input_device msg_type = %d not find failed !!!", MsgType);
			return false;
		}
		 
		 return (this->*(iter->second))(Data, Size);;
		 
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
		//FEvent KeyDownEvent(EventType::KEY_DOWN);
		//KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		//NORMAL_LOG("OnKeyDown==KeyCode = %u, Repeat = %u", KeyCode, Repeat);
		cmediasoup::MEvent KeyDownEvent(cmediasoup::EventType::KEY_DOWN);
		KeyDownEvent.SetKeyDown(KeyCode, Repeat != 0);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(KeyDownEvent);
		}

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
		//FEvent KeyUpEvent(EventType::KEY_UP);
		//KeyUpEvent.SetKeyUp(KeyCode);
		NORMAL_LOG("OnKeyUp==KeyCode = %u", KeyCode);
		cmediasoup::MEvent KeyUpEvent(cmediasoup::EventType::KEY_UP);
		KeyUpEvent.SetKeyUp(KeyCode);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(KeyUpEvent);
		}
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
		//FEvent KeyUpEvent(EventType::KEY_PRESS);
		//KeyUpEvent.SetKeyUp(Character);
		NORMAL_LOG("OnKeyPress==KeyCode = %u", Character);


		cmediasoup::MEvent KeyUpEvent(cmediasoup::EventType::KEY_PRESS);
		KeyUpEvent.SetKeyUp(Character);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(KeyUpEvent);
		}

		return true;
	}

	bool cinput_device::OnMouseEnter(const uint8*& Data,   uint32 size)
	{
		cmediasoup::MEvent MouseEvent(cmediasoup::EventType::MOUSE_ENTER);
		//KeyUpEvent.SetKeyUp(Character);
		NORMAL_EX_LOG("");
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(MouseEvent);
		}


		return true;
		 
	}

	/** 
	*鼠标离开控制范围？
	*/
	bool cinput_device::OnMouseLeave(const uint8*& Data,   uint32 size)
	{
		cmediasoup::MEvent MouseEvent(cmediasoup::EventType::MOUSE_LEAVE);
		//KeyUpEvent.SetKeyUp(Character);
		NORMAL_EX_LOG("");
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(MouseEvent);
		}
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

		
		
		 
		NORMAL_EX_LOG("    PosX = %d, PoxY = %d",   PosX, PosY );
		cmediasoup::MEvent MouseDownEvent(cmediasoup::EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(MouseDownEvent);
		}
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
		 
		NORMAL_EX_LOG("  PosX = %d, PoxY = %d",  PosX, PosY );
		
		 
		cmediasoup::MEvent MouseDownEvent(cmediasoup::EventType::MOUSE_UP);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(MouseDownEvent);
		}


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
		NORMAL_EX_LOG(" ---> PosX = %d, PoxY = %d, DeltaY = %d",  PosX, PosY, DeltaY);
		
		 
		 
		cmediasoup::MEvent MouseMoveEvent(cmediasoup::EventType::MOUSE_MOVE);
		MouseMoveEvent.SetMouseDelta(PosX, PosY, DeltaX, DeltaY);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(MouseMoveEvent);
		}

		 
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

		 
		NORMAL_EX_LOG("  ---> PosX = %d, PoxY = %d",  PosX, PosY);

		cmediasoup::MEvent MouseDownEvent(cmediasoup::EventType::MOUSE_DOWN);
		MouseDownEvent.SetMouseClick(Button, PosX, PosY);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(MouseDownEvent);
		}

		 
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

		 
		NORMAL_EX_LOG(" PosX = %d, PoxY = %d", PosX, PosY);
		cmediasoup::MEvent MouseWheelEvent(cmediasoup::EventType::MOUSE_WHEEL);
		MouseWheelEvent.SetMouseWheel(Delta, PosX, PosY);
		if (m_client_ptr)
		{
			m_client_ptr->input_device_callback(MouseWheelEvent);
		}
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