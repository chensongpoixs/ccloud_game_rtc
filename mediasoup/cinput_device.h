﻿/***********************************************************************************************
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
#include "csingleton.h"
#include <set>
#include "json.hpp"
#include <unordered_map>
namespace chen {
	 

	struct cmouse_info 
	{
		uint32  m_mouse_key;
		bool	m_key;
		cmouse_info()
			: m_mouse_key(0)
			, m_key(false) {}
	};

	
	class cinput_device
	{
	private:
		typedef bool(cinput_device::*input_device_handler_type)(const uint8*& Data, uint32 size);
		typedef bool(cinput_device::*rtc_input_device_handler_type)(const nlohmann::json & data);
		typedef std::unordered_map<EToStreamMsg, input_device_handler_type>			M_INPUT_DEVICE_MAP;
		typedef std::unordered_map<EToStreamMsg, rtc_input_device_handler_type>			M_RTC_INPUT_DEVICE_MAP;
	public:

		 cinput_device();
		~cinput_device();



	public:
		bool init();

		bool set_point(uint32 x, uint32 y);
		void Destroy();
	public:

		bool OnMessage(const std::string & consumer_id, const webrtc::DataBuffer& Buffer);
		bool OnMessage(const nlohmann::json & datachannel);
	public:
		
		/*
		* 控制信息
		*/
		bool OnRequestQualityControl(const uint8*& Data,   uint32 size);
		/**
		* 输入字符
		*/
		bool OnKeyChar( const uint8*& Data,   uint32 size);
		bool OnRtcKeyChar(const nlohmann::json & data );

		/**
		* 按下键
		*/
		 bool OnKeyDown(const uint8*& Data,   uint32 size);
		 bool OnRtcKeyDown(const nlohmann::json & data);
		/**
		* 松开键
		*/
		bool OnKeyUp(const uint8* &Data,   uint32 size);
		bool OnRtcKeyUp(const nlohmann::json & data);

		/**
		*keydown：按下键盘键
		*	keypress：紧接着keydown事件触发（只有按下字符键时触发）
		*	keyup：释放键盘键
		*/
		bool OnKeyPress(const uint8*& Data,   uint32 size);
		bool OnRtcKeyPress(const nlohmann::json & data);

		/**
		*鼠标进入控制范围 ？
		*/
		bool OnMouseEnter(const uint8*& Data,   uint32 size);
		bool OnRtcMouseEnter(const nlohmann::json & data);

		/** 
		*鼠标离开控制范围？
		*/
		bool OnMouseLeave(const uint8*& Data,   uint32 size);
		bool OnRtcMouseLeave(const nlohmann::json & data);
		/**
		* 鼠标按下  onclick事件
		* right <-> left   
		*/
		bool OnMouseDown(const uint8*& Data,   uint32 size);
		bool OnRtcMouseDown(const nlohmann::json & data);
		/** 
		*鼠标移动
		*/
		bool OnMouseMove(const uint8* &Data,   uint32 size);
		bool OnRtcMouseMove(const nlohmann::json & data);
		/** 
		*松开鼠标
		* 
		*/
		bool OnMouseUp(const uint8*& Data,   uint32 size);
		bool OnRtcMouseUp(const nlohmann::json & data);
		/** 
		* 鼠标双击
		*/
		bool OnMouseDoubleClick(const uint8*& Data,   uint32 size);
		bool OnRtcMouseDoubleClick(const nlohmann::json & data);
		/** 
		* 鼠标的滚轮滚动 
		*/
		bool OnMouseWheel(const uint8*& Data,   uint32 size);
		bool OnRtcMouseWheel(const nlohmann::json & data);
	private:
		void _UnquantizeAndDenormalize(uint16& InOutX, uint16& InOutY);

		void _UnquantizeAndDenormalize(int16& InOutX, int16& InOutY);
	private:
		cinput_device(const cinput_device&);
		cinput_device& operator =(const cinput_device&);
		
	private:
		M_INPUT_DEVICE_MAP					m_input_device;
		M_RTC_INPUT_DEVICE_MAP				m_rtc_input_device;
		FIntPoint							m_int_point;
		std::map<std::string, std::map<uint32, cmouse_info>>	m_all_consumer; 
		std::string							m_mouse_id; //当前操作的id
#if defined(_MSC_VER)
		HWND								m_main_win;
#endif // #if defined(_MSC_VER)


		
	};
	//extern cinput_device   g_input_device_mgr;
	#define 	s_input_device chen::csingleton<chen::cinput_device>::get_instance()
}

#endif // _C_INPUT_DEVICE_H_