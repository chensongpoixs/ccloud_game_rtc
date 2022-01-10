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
namespace webrtc {
	enum ECNGIndex
	{
		ECI_MediaSoup_Host = 0,
		ECI_MediaSoup_Http_Port,
		ECI_MediaSoup_WebSocket_Port,
		ECI_Room_Name,
		ECI_Client_Name,
		ECI_Video_Fps,
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

