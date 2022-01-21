/***********************************************************************************************
created: 		2022-01-19

author:			chensong

purpose:		input_device
************************************************************************************************/

#ifndef _C_INPUT_DEVICE_H_
#define _C_INPUT_DEVICE_H_
//#include <winnt.h>
#include "cnet_types.h"
#include "api/data_channel_interface.h"
#include <map>
#include "cprotocol.h"
#include "cint_point.h"
namespace webrtc {
	 
	class cinput_device
	{
	private:
		typedef bool(cinput_device::*input_device_handler_type)(const uint8*& Data, uint32 size);
		typedef std::map<EToStreamMsg, input_device_handler_type>			M_INPUT_DEVICE_MAP;
	public:

		 cinput_device();
		~cinput_device();



	public:
		bool init();

		bool set_point(uint32 x, uint32 y);
		void Destroy();
	public:

		bool OnMessage(const webrtc::DataBuffer& Buffer);
	public:

		/**
		* 输入字符
		*/
		bool OnKeyChar( const uint8*& Data,   uint32 size);
		/**
		* 按下键
		*/
		 bool OnKeyDown(const uint8*& Data,   uint32 size);

		/**
		* 松开键
		*/
		bool OnKeyUp(const uint8* &Data,   uint32 size);
 
		/**
		*keydown：按下键盘键
		*	keypress：紧接着keydown事件触发（只有按下字符键时触发）
		*	keyup：释放键盘键
		*/
		bool OnKeyPress(const uint8*& Data,   uint32 size);
		/**
		*鼠标进入控制范围 ？
		*/
		bool OnMouseEnter(const uint8*& Data,   uint32 size);

		/** 
		*鼠标离开控制范围？
		*/
		bool OnMouseLeave(const uint8*& Data,   uint32 size);
		/**
		* 鼠标按下 
		* right <-> left 
		*/
		bool OnMouseDown(const uint8*& Data,   uint32 size);

		/** 
		*鼠标移动
		*/
		bool OnMouseMove(const uint8* &Data,   uint32 size);
		/** 
		*松开鼠标
		* 
		*/
		bool OnMouseUp(const uint8*& Data,   uint32 size);
		/** 
		* 鼠标双击
		*/
		bool OnMouseDoubleClick(const uint8*& Data,   uint32 size);
		
		/** 
		* 鼠标的滚轮滚动 
		*/
		bool OnMouseWheel(const uint8*& Data,   uint32 size);
		 
	private:
		void _UnquantizeAndDenormalize(uint16& InOutX, uint16& InOutY);

		void _UnquantizeAndDenormalize(int16& InOutX, int16& InOutY);
	private:
		cinput_device(const cinput_device&);
		cinput_device& operator =(const cinput_device&);
		
	private:
		M_INPUT_DEVICE_MAP					m_input_device;
		FIntPoint							m_int_point;
	};
	extern cinput_device   g_input_device_mgr;
}

#endif // _C_INPUT_DEVICE_H_