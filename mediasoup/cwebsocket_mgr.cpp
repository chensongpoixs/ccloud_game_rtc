/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
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