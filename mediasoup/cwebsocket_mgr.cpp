#include "cwebsocket_mgr.h"
#include "Broadcaster.hpp"
#include "clog.h"
namespace chen
{

	 
	


	cwebsocket_mgr g_websocket_mgr;
	cwebsocket_mgr::cwebsocket_mgr()
		:m_stoped(true)
		, m_status(CWEBSOCKET_INIT)
		, m_ws(nullptr)
	{

	}
	cwebsocket_mgr::~cwebsocket_mgr()
	{
	}


	bool cwebsocket_mgr::init(std::string ws_url, std::string origin)
	{
		if (!m_stoped)
		{
			return false;
		}
		m_stoped.store(false);
		m_status.store(CWEBSOCKET_CONNECTING);
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc) 
		{
			printf("WSAStartup Failed.\n");
			return false;
		}
#endif

		m_ws = wsclient::WebSocket::from_url(ws_url, origin);
		//WebSocketCallback callback(m_ws);

		if (!m_ws)
		{
			RTC_LOG(LS_ERROR) << "ws connect failed !!! ws_url = " << ws_url;
			return false;
		}
		if (m_ws)
		{
			m_status.store(CWEBSOCKET_CONNECTED);
		}

	
		
		


		return true;
	}
	void cwebsocket_mgr::start()
	{
		if (m_status != CWEBSOCKET_CONNECTED || m_stoped || !m_ws)
		{
			//assert(-1);
		}
		m_status.store(CWEBSOCKET_MESSAGE);
		m_thread = std::thread(&cwebsocket_mgr::_work_thread, this);
		
	}
	void cwebsocket_mgr::destroy()
	{
		NORMAL_EX_LOG("websocket destroy !!!");
		//RTC_LOG(LS_INFO) << "websocket destroy !!!";
		m_stoped.store(true);
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	void cwebsocket_mgr::send(const std::string & message)
	{
		clock_guard lock(m_mutex);
		m_send_msgs.push_back(message);
	}
	void cwebsocket_mgr::presssmsg(std::list<std::string>& msgs)
	{
		 
		{
			clock_guard lock(m_recv_msg_mutex);
			msgs = std::move(m_recv_msgs);
			m_recv_msgs.clear();
		}
	}
	void cwebsocket_mgr::_work_thread()
	{
		 
		while (!m_stoped && m_ws->getReadyState() == wsclient::WebSocket::OPEN) 
		{
			if (m_send_msgs.size())
			{

				clock_guard lock(m_mutex);
				while (m_send_msgs.size())
				{

					std::string & send_message = m_send_msgs.front();
					m_ws->send(send_message);
					NORMAL_EX_LOG("websocket send msg = [%s]", send_message.c_str());
					m_send_msgs.pop_front();
				}


			}
			m_ws->poll();
			m_ws->dispatch(this);
			
			 
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		m_status.store(CWEBSOCKET_CLOSE);
		if (m_ws)
		{
			delete m_ws;
			m_ws = nullptr;
		}
		m_stoped.store(true);
#ifdef _WIN32
		WSACleanup();
#endif
	}


	 void cwebsocket_mgr::OnMessage(const std::string& message)
	{
		 //NORMAL_EX_LOG("");
		 
		 {
			 clock_guard lock(m_recv_msg_mutex);
			 m_recv_msgs.push_back(message);
		 }

	}
	 void cwebsocket_mgr::OnMessage(const std::vector<uint8_t>& message)
	{
		 ERROR_EX_LOG("");
	}
	void cwebsocket_mgr::OnClose()
	{
		m_status.store(CWEBSOCKET_CLOSE);
	}
}