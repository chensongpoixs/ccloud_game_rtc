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
#include "ccloud_rendering.h"
using json = nlohmann::json;
Broadcaster *broadcaster;
bool stoped = false;


void stop()
{
	webrtc::g_websocket_mgr.destroy();
	// Remove broadcaster from the server.
	broadcaster->Stop();
	RTC_LOG(LS_INFO) << "[INFO] leaving!";
	delete broadcaster;
	broadcaster = nullptr;
	stoped = true;
	//std::exit(signum);
}
void signalHandler(int signum)
{
	RTC_LOG(LS_INFO) << "[INFO] interrupt signal (" << signum << ") received";

	stop();
	
}


int main(int argc, char* argv[])
{
	//while (1);
	const char* config_filename = "client.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}

	webrtc::ccloud_rendering ccloud_rendering_mgr;

	if (!ccloud_rendering_mgr.init(config_filename))
	{
		// error
		return -1;
	}
	ccloud_rendering_mgr.Loop();

	ccloud_rendering_mgr.Destroy();

	return 0;
	
	
	return 0;
}
