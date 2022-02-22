 
#include "mediasoupclient.hpp"
//#include <cpr/cpr.h>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
 
#include "cwebsocket_mgr.h"
#include "ccfg.h"
 
#include "cwebsocket_mgr.h"
#include "ccfg.h"
 
 
 
#include "cwindow_util.h"
#include <api/task_queue/global_task_queue_factory.h>
#include "cclient.h"
using json = nlohmann::json;


 
bool stoped = false;
//syz::ccloud_rendering ccloud_rendering_mgr;


void signalHandler(int signum)
{
	//RTC_LOG(LS_INFO) << "[INFO] interrupt signal (" << signum << ") received";

	//stop();
	s_client.stop();
}

int  main_test(int argc, char* argv[])
{ 
 

	
	const char* config_filename = "client.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	bool init = s_client.init(config_filename);
	if (!init)
	{
		//
		s_client.Destory();
		printf("g_client init failed config_name = %s", config_filename);
		
		return -1;
	}
	s_client.Loop();
	s_client.Destory();


	
	
	return 0;
}
















 