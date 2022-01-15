#include "Broadcaster.hpp"
#include "mediasoupclient.hpp"
//#include <cpr/cpr.h>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include "httplib.h"
#include "cwebsocket_mgr.h"
#include "ccfg.h"
#include <WinUser.h>
#include <Windows.h>
#include "Broadcaster.hpp"
#include "cwebsocket_mgr.h"
#include "ccfg.h"
#include "httplib.h"
#include "peerConnectionUtils.hpp"
#include "ccloud_rendering.h"
using json = nlohmann::json;
Broadcaster *broadcaster;
bool stoped = false;
webrtc::ccloud_rendering ccloud_rendering_mgr;

void stop()
{
	ccloud_rendering_mgr.Destroy();
	//webrtc::g_websocket_mgr.destroy();
	//// Remove broadcaster from the server.
	//broadcaster->Stop();
	//RTC_LOG(LS_INFO) << "[INFO] leaving!";
	//delete broadcaster;
	//broadcaster = nullptr;
	//stoped = true;
	////std::exit(signum);
}
void signalHandler(int signum)
{
	RTC_LOG(LS_INFO) << "[INFO] interrupt signal (" << signum << ") received";

	//stop();
	
}


int main(int argc, char* argv[])
{
	
	const char* config_filename = "client.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	std::thread m_thread;

	if (!ccloud_rendering_mgr.init(config_filename))
	{
		// error
		return -1;
	}
	printf("init ok  2... \n");
	//while (true)
	{
		printf("main loop ........ \n");
		m_thread = std::thread(
			[=]() {
			printf("child ->  loop ........ \n");
			ccloud_rendering_mgr.Loop();
			printf("child ->  loop exit ........ \n");
		}
		);
		printf("main [g_thread_count = %d] loop min 2... \n", g_thread_count);
		std::this_thread::sleep_for(std::chrono::minutes(1));
		printf("main [g_thread_count = %d][] loop min 2... \n", g_thread_count);
		ccloud_rendering_mgr.Destroy();
		
		
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}
	printf("all stop\n");
	all_stop();
	printf("+++++++++++++++++all stop ok !!! \n");
	//std::this_thread::sleep_for(std::chrono::seconds(5));
	std::this_thread::sleep_for(std::chrono::minutes(1));

	mediasoupclient::Handler::all_close();
	std::this_thread::sleep_for(std::chrono::minutes(1));
	
	//while (true)
	{
		printf("main [g_thread_count = %d] ====================loop min 2... \n", g_thread_count);
		std::this_thread::sleep_for(std::chrono::seconds(10));
		printf("main exit [g_thread_count = %d] ====================loop min 2... \n", g_thread_count);
	}
	

	return 0;
	
	
	return 0;
}
