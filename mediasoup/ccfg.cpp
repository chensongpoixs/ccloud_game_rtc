/***********************************************************************************************
				created: 		2019-05-01

				author:			chensong

				purpose:		ccfg
************************************************************************************************/
#include "ccfg.h"

namespace chen {
	ccfg g_cfg;
	
	ccfg::ccfg()
	{
	}


	ccfg::~ccfg()
	{
	}
	bool ccfg::init(const char * file_name)
	{
		if (!cconfig::init(ECI_Max, file_name))
		{
			return false;
		}
		// 默认值
		set_string(ECI_MediaSoup_Host, "mediasoup_ip", "127.0.0.1");
		set_uint32(ECI_MediaSoup_Http_Port, "mediasoup_http_port", 8888);
		set_uint32(ECI_MediaSoup_WebSocket_Port, "mediasoup_websocket_port", 8888);
		set_string(ECI_Room_Name, "room_name", "chensong");
		set_string(ECI_Client_Name, "client_name", "test");
		set_uint32(ECI_Video_Fps, "video_fps", 30);
		/*set_int32(ECI_Time_Adjust, "time_adjust", 0);
		set_string(ECI_Log_DB_Host, "log_db_ip", "127.0.0.1");
		set_int32(ECI_Log_DB_Port, "log_db_port", 3306);
		set_string(ECI_Log_DB_User, "log_db_user", "root");
		set_string(ECI_Log_DB_Passwd, "log_db_passwd", "20190813");
		set_string(ECI_Log_DB_Name, "log_db_name", "global_log");
		set_string(ECI_ResPath, "res_path", "../res/");*/
		//set_uint32(ECI_LogPoolSize, "log_pool_size", 10);
		//配置表的值
		load_cfg_file();

		return true;
	}
	void ccfg::destroy()
	{
		cconfig::destroy();
	}
}//namespace chen 
