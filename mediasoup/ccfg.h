/***********************************************************************************************
			created: 		2019-05-01

			author:			chensong

			purpose:		ccfg
************************************************************************************************/
#ifndef _C_CFG_H_
#define _C_CFG_H_
//#include "cconfig.h"
#include "cconfig.h"
//#include "csingleton.h"
namespace chen {
	enum ECNGIndex
	{
		ECI_MediaSoup_Host = 0,
		ECI_MediaSoup_Http_Port,
		ECI_MediaSoup_WebSocket_Port,
		ECI_Room_Name,
		ECI_Client_Name,
		ECI_Video_Fps,
		ECI_WebSocket_Reconnect,
		ECI_ReconnectWait,
		ECI_ProduceVideo,
		ECI_OsgWebrtcFrame,
		ECI_DesktopCapture,
		ECI_WebSocketTimers, //断线从新连接的次数
		ECI_RtcAvgRate, // 
		ECI_RtcMaxRate,
		ECI_RtcFrames,
		ECI_EncoderVideoGop,
		ECI_LogLevel,
		ECI_EncoderArraySize,
		ECI_EncoderPreset,
		ECI_EncoderPFrameCount,
		ECI_EncoderLowLatency,
		ECI_GpuVideoLock,
	//	ECI_Time_Zone,
	//	ECI_Time_Adjust,
	//	ECI_Log_DB_Host,
	//	ECI_Log_DB_Port,
	//	ECI_Log_DB_User,
	//	ECI_Log_DB_Passwd,
	//	ECI_Log_DB_Name,
	//	ECI_ResPath,
	//	ECI_LogPoolSize,
		ECI_Max,
	};
	class ccfg : public cconfig
	{
	public:
	    explicit	ccfg();
		virtual	~ccfg();
	public:
		bool init(const char *file_name);
		void destroy();
	};

	extern 	ccfg g_cfg;
} //namespace chen

#endif //!#define _C_CFG_H_

