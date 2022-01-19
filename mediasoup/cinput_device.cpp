/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
************************************************************************************************/

#include "cinput_device.h"
#include "cwindow_util.h"




namespace webrtc {

#define SET_POINT(v) POINT pt; pt.x = v.x; pt.y = v.y;

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

#define WINDOW_BNTTON_UP(v)  uint32 active_type = WM_LBUTTONDOWN; switch (vec.button)   \
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

	cinput_device::cinput_device() {}
	cinput_device::~cinput_device() {}

 

	/**
	* 输入字符
	*/
	bool cinput_device::OnKeyChar(const TCHAR Character, const cvector& vec)
	{

		WINDOW_MAIN();
		SET_POINT(vec);
		WINDOW_CHILD();
		if (childwin)
		{
			::PostMessageW(childwin, WM_CHAR, Character, 1);
		}
		else if (mwin)
		{
			::PostMessageW(mwin, WM_CHAR, Character, 1);
		}
		else
		{
			// log -> error 
			return false;
		}

		return true;
	}
	/**
	* 按下键
	*/
	bool cinput_device::OnKeyDown(const int32 KeyCode, const uint32 CharacterCode, const cvector& vec)
	{

		WINDOW_MAIN();
		SET_POINT(vec);
		WINDOW_CHILD();
		if (childwin)
		{
			::PostMessageW(childwin, WM_KEYDOWN, KeyCode, 1);
		}
		else if (mwin)
		{
			::PostMessageW(mwin, WM_KEYDOWN, KeyCode, 1);
		}
		else
		{
			// log -> error 
			return false;
		}
		return true;
	}

	/**
	* 松开键
	*/
	bool cinput_device::OnKeyUp(const int32 KeyCode, const uint32 CharacterCode, const cvector& vec)
	{
		WINDOW_MAIN();
		SET_POINT(vec);
		WINDOW_CHILD();
		if (childwin)
		{
			::PostMessageW(childwin, WM_KEYUP, KeyCode, 1);
		}
		else if (mwin)
		{
			::PostMessageW(mwin, WM_KEYUP, KeyCode, 1);
		}
		else
		{
			// log -> error 
			return false;
		}
		return true;
	}

	/**
	* 鼠标按下 
	* right <-> left 
	*/
	bool cinput_device::OnMouseDown(const cvector& vec)
	{
		WINDOW_MAIN();
		WINDOW_BNTTON_DOWN(vec);
		if (mwin)
		{
			::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(vec.x, vec.y));
		}
		else
		{
			// log -> error 
			return false;
		}
		return true;
	
	}

	/** 
	*松开鼠标
	* 
	*/
	bool cinput_device::OnMouseUp(const cvector& vec)
	{
		WINDOW_MAIN();
		WINDOW_BNTTON_UP(vec);
		if (mwin)
		{
			::PostMessageW(mwin, active_type, MAKEWPARAM(0, 0), MAKEWPARAM(vec.x, vec.y));
		}
		else
		{
			// log -> error 
			return false;
		}
		return true;
		return true;
	}
	/** 
	* 鼠标双击
	*/
	bool cinput_device::OnMouseDoubleClick(const cvector& vec)
	{
		return true;
	}

	/** 
	* 鼠标的滚轮滚动 
	*/
	bool cinput_device::OnMouseWheel(const cvector& vec)
	{

		WINDOW_MAIN();
		if (mwin)
		{
			::PostMessage(mwin, WM_MOUSEWHEEL, MAKEWPARAM(0, vec.sheeldelta) /* ascii码 */, MAKELPARAM(vec.x, vec.y));
		}
		else
		{
			// log error 
			return false;
		}
		
		return true;
	}
}