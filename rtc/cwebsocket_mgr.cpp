#include "cwebsocket_mgr.h"
 
#include "clog.h"
namespace chen
{

	 
	


	//cwebsocket_mgr g_websocket_mgr;
	cwebsocket_mgr::cwebsocket_mgr()
		:m_stoped(true)
		, m_status(CWEBSOCKET_INIT)
		, m_ws(nullptr)
	{
#ifdef _WIN32
		INT rc;
		WSADATA wsaData;

		rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (rc)
		{
			printf("WSAStartup Failed.\n");
			 
		}


		
		//PathRemoveFileSpec(czFileName);
#endif
	}
	cwebsocket_mgr::~cwebsocket_mgr()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}


	

	bool cwebsocket_mgr::init(std::string ws_url )
	{
		if (!m_stoped)
		{
			return false;
		}
		
		m_status.store(CWEBSOCKET_CONNECTING);

		NORMAL_EX_LOG("ws_url = %s", ws_url.c_str());
		m_ws = wsclient::WebSocket::from_url(ws_url );
		//WebSocketCallback callback(m_ws);

		if (!m_ws)
		{
			//RTC_LOG(LS_ERROR) << "ws connect failed !!! ws_url = " << ws_url;
			ERROR_EX_LOG("ws connect mediasoup failed !!! ws_url = %s", ws_url.c_str());
			return false;
		}
		if (m_ws)
		{
			m_status.store(CWEBSOCKET_CONNECTED);
		}

		m_stoped.store(false);
		
		


		return true;
	}
	bool cwebsocket_mgr::startup()
	{
		if (m_status != CWEBSOCKET_CONNECTED || m_stoped || !m_ws)
		{
			WARNING_EX_LOG("websocket status = %d, m_stoped = %d", m_status.load(), m_stoped.load());
			return false;
		}
		m_status.store(CWEBSOCKET_MESSAGE);
		m_thread = std::thread(&cwebsocket_mgr::_work_thread, this);
		return true;
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
		_clear_all_msg();
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
	void cwebsocket_mgr::_clear_all_msg()
	{
		{
			clock_guard lock(m_recv_msg_mutex);
			m_recv_msgs.clear();
		}
		
		{
			clock_guard lock(m_mutex);
			m_send_msgs.clear();
		}
	}
	void cwebsocket_mgr::_work_thread()
	{
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;
		while (!m_stoped && m_ws->getReadyState() == wsclient::WebSocket::OPEN) 
		{
			pre_time = std::chrono::steady_clock::now();
			if (m_send_msgs.size())
			{

				clock_guard lock(m_mutex);
				while (m_send_msgs.size())
				{

					std::string & send_message = m_send_msgs.front();
					m_ws->send(send_message);
					//NORMAL_EX_LOG("websocket send msg = [%s]", send_message.c_str());
					m_send_msgs.pop_front();
				}


			}
			m_ws->poll();
			m_ws->dispatch(this);
			
			 
			cur_time_ms = std::chrono::steady_clock::now();
			dur = cur_time_ms - pre_time;
			ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
			elapse = static_cast<uint32_t>(ms.count());
			if (elapse < 150)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(150 - elapse));
			}
		}
		m_status.store(CWEBSOCKET_CLOSE);
		if (m_ws)
		{
			delete m_ws;
			m_ws = nullptr;
		}
		m_stoped.store(true);

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