/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
************************************************************************************************/

#ifndef _C_INPUT_DEVICE_H_
#define _C_INPUT_DEVICE_H_
#include <winnt.h>
#include "cnet_types.h"
namespace webrtc {
	namespace EMouseButtons
	{
		enum Type
		{
			Left = 0,
			Middle,
			Right,
			Thumb01,
			Thumb02,

			Invalid,
		};
	}
	 


	struct cvector
	{
		int32 x;
		int32 y;
		EMouseButtons::Type button;
		int32 sheeldelta;
		cvector() : x(0), y(0), button(EMouseButtons::Invalid), sheeldelta(0) {}
	};
	class cinput_device
	{
	public:

		 cinput_device();
		~cinput_device();

	public:

		/**
		* 输入字符
		*/
		static bool OnKeyChar( const TCHAR Character, const cvector& vec );
		/**
		* 按下键
		*/
		static bool OnKeyDown(const int32 KeyCode, const uint32 CharacterCode, const cvector& vec);

		/**
		* 松开键
		*/
		static bool OnKeyUp(const int32 KeyCode, const uint32 CharacterCode, const cvector& vec);
 
		/**
		* 鼠标按下 
		* right <-> left 
		*/
		static bool OnMouseDown(const cvector& vec);

		/** 
		*松开鼠标
		* 
		*/
		static bool OnMouseUp(const cvector& vec);
		/** 
		* 鼠标双击
		*/
		static bool OnMouseDoubleClick(const cvector& vec);
		
		/** 
		* 鼠标的滚轮滚动 
		*/
		static bool OnMouseWheel(const cvector& vec);
		 
		
	private:
		cinput_device(const cinput_device&);
		cinput_device& operator =(const cinput_device&);
		
	};
}

#endif // _C_INPUT_DEVICE_H_