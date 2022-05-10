#include <iostream>
#include <csignal> // sigsuspend()
#include <cstdlib>
#include <iostream>
#include <string>
#include "cmediasoup_mgr.h"



 
cmediasoup::cmediasoup_mgr g_mediasoup_mgr;


bool stoped = false;

void signalHandler(int signum)
{
	stoped = true;
	
}



int  testmain(int argc, char *argv[])
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);


	g_mediasoup_mgr.init();

	//g_mediasoup_mgr.set_mediasoup_status_callback(&mediasoup_callback);
	/*
	const char* mediasoupIp, uint16_t port
		, const char* roomName, const char* clientName
	
	*/
	g_mediasoup_mgr.startup("127.0.0.1", 8888, argv[1], argv[1]);
	while (!stoped)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	g_mediasoup_mgr.destroy();
	return 0;
}