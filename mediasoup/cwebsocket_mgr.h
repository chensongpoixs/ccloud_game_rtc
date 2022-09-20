/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef C_WEBSOCKET_MGR_H
#define C_WEBSOCKET_MGR_H
#include <thread>

#include <atomic>
#include "wsclient.h"
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
#include <mutex>
#include <list>

namespace chen
{
	enum CWEBSOCKET_TYPE
	{
		CWEBSOCKET_INIT = 0,
		CWEBSOCKET_CONNECTING,
		CWEBSOCKET_CONNECTED,
		CWEBSOCKET_MESSAGE,
		CWEBSOCKET_CLOSE
	};
	class cwebsocket_mgr : public wsclient::WebSocketCallback 
	{
	private:
		typedef		std::lock_guard<std::mutex>			clock_guard;
		//typedef		std::mutex							clock;
	public:
		cwebsocket_mgr();
		~cwebsocket_mgr();
	public:
		bool init(std::string ws_url);
		bool startup();
		void destroy();
		
		void send(const std::string & message);
		void presssmsg(std::list<std::string>& msgs);
		CWEBSOCKET_TYPE get_status() const { return m_status; }


		virtual void OnMessage(const std::string& message);
		virtual void OnMessage(const std::vector<uint8_t>& message);
		virtual void OnClose();
	private:
		void _work_thread();

		void _clear_all_msg();
	private:
		//cwebsocket_mgr &operator= (const cwebsocket_mgr&);
		//cnoncopyable(cnoncopyable&&);
		cwebsocket_mgr(const cwebsocket_mgr&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cwebsocket_mgr& operator=(const cwebsocket_mgr&);
	private:
		std::thread						m_thread;
		std::atomic<bool>				m_stoped;
		std::atomic<CWEBSOCKET_TYPE>	m_status;
		wsclient::WebSocket::pointer	m_ws;
		std::mutex						m_mutex;
		std::list<std::string>			m_send_msgs;
		std::mutex						m_recv_msg_mutex;
		std::list<std::string>			m_recv_msgs;
	};

	//extern cwebsocket_mgr g_websocket_mgr;
}



#endif // C_WEBSOCKET_MGR_H