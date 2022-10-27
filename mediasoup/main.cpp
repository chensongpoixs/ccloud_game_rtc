#include <iostream>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include "cmediasoup_mgr.h"



 
//cmediasoup::cmediasoup_mgr g_mediasoup_mgr;


bool stoped = false;

void signalHandler(int signum)
{
	stoped = true;
	
}



int  main(int argc, char *argv[])
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	
	cmediasoup::cmediasoup_mgr::global_init();

	cmediasoup::cmediasoup_mgr g_mediasoup_mgr1;
	//cmediasoup::cmediasoup_mgr g_mediasoup_mgr2;
	g_mediasoup_mgr1.init(5);
	//g_mediasoup_mgr2.init(5);

	//g_mediasoup_mgr.set_mediasoup_status_callback(&mediasoup_callback);
	/*
	const char* mediasoupIp, uint16_t port
		, const char* roomName, const char* clientName
	
	*/
	g_mediasoup_mgr1.startup("127.0.0.1", 8888, "chensong1", "chensong1");
//	g_mediasoup_mgr2.startup("127.0.0.1", 8888, "chensong2", "chensong2");
	while (!stoped)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	g_mediasoup_mgr1.destroy();
	//g_mediasoup_mgr2.destroy();
	cmediasoup::cmediasoup_mgr::global_destroy();
	return 0;
}