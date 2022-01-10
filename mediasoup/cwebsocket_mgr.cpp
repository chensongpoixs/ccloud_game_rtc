#include "cwebsocket_mgr.h"
#include "Broadcaster.hpp"
namespace webrtc
{

	FILE * out_file_ptr = fopen("protoo_websocket.log", "wb+");
	class WebSocketCallback :public wsclient::WebSocketCallback
	{
	public:
		WebSocketCallback(wsclient::WebSocket*_ws)
			:ws(_ws)
		{
		}
		void OnMessage(const std::string& message) 
		{
			fprintf(out_file_ptr, "[%s]\n", message.c_str());
			fflush(out_file_ptr);
			RTC_LOG(LS_INFO) << "["<< __FUNCTION__ << "][" << __LINE__ <<"RX: " << message;
			//printf("RX: %s\n",message.c_str());
			//if (message == "world") 
				//ws->close();
		}

		void OnMessage(const std::vector<uint8_t>& message) 
		{
			
			std::ostringstream cmd;
			for (const uint8_t& value : message)
			{
				cmd << ", " << value;
			}
			fprintf(out_file_ptr, "vec[%s]\n", cmd.str().c_str());
			fflush(out_file_ptr);
			RTC_LOG(LS_INFO) << "["<< __FUNCTION__ << "][" << __LINE__ <<"RX: " << cmd.str().c_str();
			
		}
		void OnClose()
		{
			fprintf(out_file_ptr, "close()\n");
			fflush(out_file_ptr);
		}

		wsclient::WebSocket* ws;
	};


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

		assert(m_ws);
		if (m_ws)
		{
			m_status.store(CWEBSOCKET_CONNECTED);
		}

	
		//ws->send("goodbye");
		//ws->send("hello");
		


		return true;
	}
	void cwebsocket_mgr::start()
	{
		if (m_status != CWEBSOCKET_CONNECTED || m_stoped || !m_ws)
		{
			assert(-1);
		}

		m_thread = std::thread(&cwebsocket_mgr::_work_thread, this);
		m_status.store(CWEBSOCKET_MESSAGE);
	}
	void cwebsocket_mgr::destroy()
	{
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
	void cwebsocket_mgr::_work_thread()
	{
		WebSocketCallback callback(m_ws);
		while (!m_stoped && m_ws->getReadyState() == wsclient::WebSocket::OPEN) 
		{
			
			m_ws->poll();
			m_ws->dispatch(callback);
			if (m_send_msgs.size())
			{

				clock_guard lock(m_mutex);
				while (m_send_msgs.size())
				{
					std::string & send_message = m_send_msgs.front();
					m_ws->send(send_message);
					m_send_msgs.pop_front();
				}
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
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
}