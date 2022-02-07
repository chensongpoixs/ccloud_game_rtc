#include "cclient.h"
#include "mediasoupclient.hpp"
#include "ortc.hpp"
#include "clog.h"
#include "ccfg.h"
#include "cwebsocket_mgr.h"
namespace chen {


	cclient::cclient():m_id(100000),m_loaded(false), m_stoped(false){}
	cclient::~cclient(){}

	bool cclient::init(const char * config_name)
	{

		if (!LOG::init(ELogStorageScreenFile))
		{
			std::cerr << " log init failed !!!";
			return false;
		}
		bool init = g_cfg.init(config_name);
		if (!init)
		{
			//RTC_LOG(LS_ERROR) << "config init failed !!!" << config_name;
			ERROR_EX_LOG("config init failed !!! config_name = %s", config_name);
			return false;
		}
		return true;
	}
	void cclient::Loop()
	{
		std::string ws_url = "ws://" + g_cfg.get_string(ECI_MediaSoup_Host) + ":" + std::to_string(g_cfg.get_int32(ECI_MediaSoup_Http_Port)) + "/?roomId=" + g_cfg.get_string(ECI_Room_Name) + "&peerId=" + g_cfg.get_string(ECI_Client_Name);//ws://127.0.0.1:8888/?roomId=chensong&peerId=xiqhlyrn", "http://127.0.0.1:8888")
		std::string origin = "http://" + g_cfg.get_string(ECI_MediaSoup_Host) + ":" + std::to_string(g_cfg.get_int32(ECI_MediaSoup_Http_Port));
		std::list<std::string> msgs;
		
		while (!m_stoped)
		{
			switch (m_status)
			{
			case 0: // None
			{  // websocket connect 
				// 1. connect suucer status -> 1
				g_websocket_mgr.destroy();
				if (!g_websocket_mgr.init(ws_url, origin))
				{
					//RTC_LOG(LS_ERROR) << "weboscket connect failed !!! url = " << ws_url;
					WARNING_EX_LOG("weboscket connect url = %s failed !!!   ", ws_url.c_str());
					break;
				}
				g_websocket_mgr.start();

				_send_router_rtpcapabilities();
				m_status = 10;
				// 1.1 获取服务器的处理能力
				// 2. connect failed wait 5s..
				break;
			}
			case 1: // None
			{
				
				// websocket connect success -> 
				// 1. wait  server 处理能力
				// 1.1 load -> 
				_send_create_webrtc_transport();
				// 1.2 创建Send WebRTC transport id -> 
				m_status = 10;
				break;
			}
			case 2:
			{
				// 1.  wait server call transport dtls info
				// 1.1 Send WebRTC Connect -> 
				m_status = 10;
				break;
			}
			case 3:
			{
				// 1. wait Send WebRTC info ->
				// 1.1 Send WebRTC Producer 
			//	m_send_transport->webrtc_transport_produce();
				_send_produce_webrtc_transport();
				m_status = 10;
				break;
			}
			case 4: 
			{
				// 1. wait Send WebRTC Producer 
				// 1. 1 Send Join Room 
				m_status = 10;
				break;
			}
			case 5:
			{
				// 1. wait Join Room 
				// 
				m_status = 10;
				break;
			}
			case 6:
			{
				// loop --> 信息
				m_status = 10;
				break;
			}
			case 10: // wait server msg 
			{
				g_websocket_mgr.presssmsg(msgs);
				if (msgs.empty())
				{
					break;
				}
				while (!msgs.empty())
				{
					std::string msg = std::move(msgs.front());
					msgs.pop_front();

					nlohmann::json response = nlohmann::json::parse(msg);

					if (response.find("notification") == response.end())
					{
						NORMAL_EX_LOG("notification --> msg = %s", msg);
					}
					else if (response.find("response") == response.end())//response
					{
						uint64 id = response["id"];
						std::map<uint64, server_protoo_msg_call>::const_iterator iter =  m_server_protoo_msg_call.find(id);
						if (iter == m_server_protoo_msg_call.end())
						{
							ERROR_EX_LOG("not find id = %u, msg = %s", id, msg.c_str());
						}
						else
						{
							(this->*(iter->second))(response);
						}
					}
					else
					{
						ERROR_EX_LOG(" not find msg type !!! msg = %s", msg.c_str());
					}
				}
				break;
			}
			default:
				break;
			}
		}
		
	}


	void cclient::Destory()
	{

	}
	bool cclient::_load(nlohmann::json routerRtpCapabilities)
	{
		if (m_loaded)
		{
			return true;
		}
		nlohmann::json nativeRtpCapabilities = mediasoupclient::Handler::GetNativeRtpCapabilities(nullptr);

		m_extendedRtpCapabilities = mediasoupclient::ortc::getExtendedRtpCapabilities(nativeRtpCapabilities, routerRtpCapabilities);
		
		// Generate our receiving RTP capabilities for receiving media.
		m_recvRtpCapabilities = mediasoupclient::ortc::getRecvRtpCapabilities(m_extendedRtpCapabilities);


		m_sctpCapabilities = mediasoupclient::Handler::GetNativeSctpCapabilities();
		m_loaded = true;
		return true;
	}

	void cclient::transportofferasner(bool success)
	{
		if (success)
		{
			m_send_transport->webrtc_connect_transport_setup_connect("server");
		}
	}
	bool cclient::_send_router_rtpcapabilities()
	{
	
		nlohmann::json router_rtpcapabilities =
		{
			{"request",true},
		{"id" , ++m_id}, //  
		{"method" , "getRouterRtpCapabilities"}, //方法
		{"data" , nlohmann::json::object()}
		};
		g_websocket_mgr.send(router_rtpcapabilities.dump());
		m_server_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_RouterRtpCapabilities));
		

		return true;
	}


	bool cclient::_send_create_webrtc_transport()
	{
		nlohmann::json data = 
		{
			{"forceTcp",false},
		{"producing",true},
		{"consuming" , false},
		{	"sctpCapabilities", m_sctpCapabilities }
 
		};
		nlohmann::json create_webrtc_transport =
		{
			{"request",true},
		{"id" , ++m_id}, //  
		{"method" , "createWebRtcTransport"}, //方法
		{"data" , data}
		};
		g_websocket_mgr.send(create_webrtc_transport.dump());
		m_server_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_create_webrtc_transport));
		return true;
	}
	bool cclient::_send_connect_webrtc_transport(nlohmann::json dtlsparameters)
	{
		 
		nlohmann::json connect_webrtc_transport =
		{
			{"request",true},
		 {"id" , ++m_id}, //  
		 {"method" , "connectWebRtcTransport"}, //方法
		 {"data" , dtlsparameters}
		};
		g_websocket_mgr.send(connect_webrtc_transport.dump());
		m_server_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_connect_webrtc_transport));
		m_status = 10;
		return true;
	}

	bool cclient::_send_produce_webrtc_transport()
	{
		nlohmann::json rtpParameters = 
		{
			{"transportId", m_send_transport->get_transportId()},
			{"kind", m_send_transport->get_kind()}, 
			{"rtpParameters", m_send_transport->get_sending_rtpParameters()}
		};
		nlohmann::json produce_webrtc_transport =
		{
			{"request",true},
		{"id" , ++m_id}, //  
		{"method" , "connectWebRtcTransport"}, //方法
		{"data" , rtpParameters}
		};
		g_websocket_mgr.send(produce_webrtc_transport.dump());
		m_server_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_connect_webrtc_transport));
		m_status = 10;
		return true;
	}

	bool cclient::_server_RouterRtpCapabilities(const  nlohmann::json & msg)
	{
		_load(msg["data"]);
		m_status = 1; // 
		return true;
	}

	bool cclient::_server_create_webrtc_transport(const  nlohmann::json & msg)
	{

		std::string  transport_id = msg['data']['id'];
		m_send_transport.reset(new rtc::RefCountedObject<ctransport>(transport_id, this) );
		m_send_transport->init(transport_id, m_extendedRtpCapabilities, msg['data']["iceParameters"], msg['data']["iceCandidates"], msg['data']["dtlsParameters"], msg['data']["sctpParameters"]);
		m_send_transport->webrtc_connect_transport_offer(nullptr);
		//m_send_transport->webrtc_connect_transport_setup_connect("server");
		//m_status = 2;
		return true;
	}
	bool cclient::_server_connect_webrtc_transport(const  nlohmann::json & msg)
	{
		m_status = 3;
		return true;
	}

	bool cclient::_server_produce_webrtc_transport(const  nlohmann::json & msg)
	{
		m_send_transport->webrtc_transport_produce(msg["data"]["id"]);
		m_status = 4;
		return true;
	}
}