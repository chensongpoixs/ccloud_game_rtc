#include "cclient.h"
#include "mediasoupclient.hpp"
#include "ortc.hpp"
#include "clog.h"
#include "ccfg.h"
#include "cwebsocket_mgr.h"
#include "cdesktop_capture.h"
#include "cdevice.h"
#include "pc/video_track_source.h"
namespace chen {


	cclient::cclient():m_id(100000),m_loaded(false), m_stoped(false), m_status(EMediasoup_None){}
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
		m_server_protoo_msg_call.insert(std::make_pair("newDataConsumer", &cclient::server_request_new_dataconsumer));
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
			case EMediasoup_WebSocket_Init: // None
			case EMediasoup_None:
			{  // websocket connect 
				// 1. connect suucer status -> 1
				//g_websocket_mgr.destroy();
				if (!g_websocket_mgr.init(ws_url, origin))
				{
					//RTC_LOG(LS_ERROR) << "weboscket connect failed !!! url = " << ws_url;
					WARNING_EX_LOG("weboscket connect url = %s failed !!!   ", ws_url.c_str());
					break;
				}
				g_websocket_mgr.start();

				_send_router_rtpcapabilities();
				m_status = EMediasoup_WebSocket;
				// 1.1 获取服务器的处理能力
				// 2. connect failed wait 5s..
				break;
			}
			case EMediasoup_Request_Create_Send_Webrtc_Transport: // None
			case EMediasoup_Request_Create_Recv_Webrtc_Transport:
			{
				
				// websocket connect success -> 
				// 1. wait  server 处理能力
				// 1.1 load -> 
				_send_create_webrtc_transport();
				// 1.2 创建Send WebRTC transport id -> 
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Connect_Webrtc_Transport:
			{
				// 1.  wait server call transport dtls info
				// 1.1 Send WebRTC Connect -> 
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Produce_Webrtc_Transport:
			{
				// 1. wait Send WebRTC info ->
				// 1.1 Send WebRTC Producer 
			//	m_send_transport->webrtc_transport_produce();
				m_send_transport->webrtc_connect_transport_setup_connect_server_call();
				_send_produce_webrtc_transport();
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Consumer_Webrtc_Transport:
			{
				m_recv_transport->webrtc_connect_recv_setup_call();
				break;
			}
			case EMediasoup_Request_Send_Connect_Set:
			{
				m_send_transport->webrtc_connect_transport_setup_connect("server");
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Recv_Connect_Set:
			{
				m_recv_transport->webrtc_connect_transport_setup_connect("client");
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Join_Room: 
			{
				// 1. wait Send WebRTC Producer 
				// 1. 1 Send Join Room 
				_send_join_room();
				m_status = EMediasoup_WebSocket;
				break;
			}
			//case 5:
			//{
			//	// 1. wait Join Room 
			//	// 
			//	m_status = EMediasoup_WebSocket;
			//	break;
			//}
			//case 6:
			//{
			//	// loop --> 信息
			//	m_status = EMediasoup_WebSocket;
			//	break;
			//}
			case EMediasoup_WebSocket: // wait server msg 
			{
				g_websocket_mgr.presssmsg(msgs);
				
				
				if (!msgs.empty() && g_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
				{
					_presssmsg(msgs);
				}

				if (g_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
				{
					m_status = EMediasoup_WebSocket_Close;
					msgs.clear();
				}
				break;
			}
			case EMediasoup_WebSocket_Close:
			{
				// 1.  send_transport destroy --> 
				// 2. recv_transport destroy --> 
				// 3. status -> reconnect wait --> 
				m_status = EMediasoup_WebSocket_Wait;
				break;
			}
			case EMediasoup_WebSocket_Wait:
			{
				// 10 sleep 
				// TODO@chensong 20220208 ---> 增加时间
				std::this_thread::sleep_for(std::chrono::milliseconds(g_cfg.get_uint32(ECI_WebSocket_Reconnect)));
				m_status = EMediasoup_WebSocket_Init;
				break;
			}
			
			default:
			{
				ERROR_EX_LOG("client not find status = %u", m_status);
			}
				break;
			}
		}
		
	}
	void cclient::_presssmsg(std::list<std::string> & msgs)
	{

		while (!msgs.empty() && g_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
		{
			std::string msg = std::move(msgs.front());
			msgs.pop_front();
			NORMAL_EX_LOG("server ====> msg = %s", msg.c_str());
			nlohmann::json response = nlohmann::json::parse(msg);

			if (response.find("notification") != response.end())
			{
				//NORMAL_EX_LOG("notification --> msg = %s", msg);
			}
			else if (response.find("response") != response.end())//response
			{
				uint64 id = response["id"];
				std::map<uint64, client_protoo_msg>::const_iterator iter = m_client_protoo_msg_call.find(id);
				if (iter == m_client_protoo_msg_call.end())
				{
					ERROR_EX_LOG("not find id = %u, msg = %s", id, msg.c_str());
				}
				else
				{
					(this->*(iter->second))(response);
					m_client_protoo_msg_call.erase(iter);
				}
			}
			else if (response.find("request") != response.end())
			{
				//服务器请求客户端响应的请求
				std::string method = response["method"];
				std::map<std::string, server_protoo_msg>::iterator iter =  m_server_protoo_msg_call.find(method);
				if (iter != m_server_protoo_msg_call.end())
				{
					
					(this->*(iter->second))(response);
					//server_request_new_dataconsumer(response);
				}
				else
				{
					WARNING_EX_LOG("server request client not find method  response = %s", response.dump().c_str());
				}
			}
			else
			{
				ERROR_EX_LOG(" not find msg type !!! msg = %s", msg.c_str());
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
		nlohmann::json nativeRtpCapabilities = deivce::GetNativeRtpCapabilities();

		m_extendedRtpCapabilities = mediasoupclient::ortc::getExtendedRtpCapabilities(nativeRtpCapabilities, routerRtpCapabilities);
		
		// Generate our receiving RTP capabilities for receiving media.
		m_recvRtpCapabilities = mediasoupclient::ortc::getRecvRtpCapabilities(m_extendedRtpCapabilities);


		m_sctpCapabilities = deivce::GetNativeSctpCapabilities();
		m_loaded = true;
		return true;
	}

	void cclient::transportofferasner(bool send, bool success)
	{
		if (success)
		{
			//
			if (send)
			{
				m_status = EMediasoup_Request_Send_Connect_Set;
			}
			else
			{
				m_status = EMediasoup_Request_Recv_Connect_Set;;
			}
			
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
		m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_RouterRtpCapabilities));
		

		return true;
	}


	bool cclient::_send_create_webrtc_transport()
	{
		
		
		if (EMediasoup_Request_Create_Send_Webrtc_Transport == m_status)
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
		}
		else
		{
			nlohmann::json data =
			{
				{"forceTcp",false},
				{"producing",false},
				{"consuming" , true},
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
		}
		
		m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_create_webrtc_transport));
		return true;
	}
	bool cclient::_send_connect_webrtc_transport(nlohmann::json dtlsparameters)
	{
		std::string str = dtlsparameters["role"];
		NORMAL_EX_LOG("connect webrtc -> role = %s", str.c_str());
		std::string transportId;
		if (str == "server")
		{
			transportId = m_send_transport->get_transportId();
		}
		else
		{
			transportId = m_recv_transport->get_transportId();
		}
		nlohmann::json data = 
		{
			{"transportId", transportId},
		{"dtlsParameters", dtlsparameters}
		};
		nlohmann::json connect_webrtc_transport =
		{
			{"request",true},
		 {"id" , ++m_id}, //  
		 {"method" , "connectWebRtcTransport"}, //方法
		 {"data" , data }
		};
		g_websocket_mgr.send(connect_webrtc_transport.dump());
		if (str == "server")
		{
			m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_connect_webrtc_transport));
		}
		else
		{
			m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_recv_connect_webrtc_transport));
		}
		
		m_status = EMediasoup_WebSocket;
		return true;
	}

	bool cclient::_send_produce_webrtc_transport()
	{
		nlohmann::json rtpParameters = 
		{
			{"transportId", m_send_transport->get_transportId()},
			{"kind", m_send_transport->get_kind()}, 
			{"rtpParameters", m_send_transport->get_sending_rtpParameters()},
		{"appData", nlohmann::json::object()}
		};
		nlohmann::json produce_webrtc_transport =
		{
			{"request",true},
		{"id" , ++m_id}, //  
		{"method" , "produce"}, //方法
		{"data" , rtpParameters}
		};

		NORMAL_EX_LOG("produce_webrtc_transport = %s", produce_webrtc_transport.dump().c_str());
		g_websocket_mgr.send(produce_webrtc_transport.dump());
		m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_produce_webrtc_transport));
		//m_status = EMediasoup_WebSocket;
		return true;
	}
	bool cclient::_send_join_room()
	{
		nlohmann::json device = 
		{
			{"flag","native"},
		{"name" , "win"},
		{"version" , "1.0"}
		};
		nlohmann::json rtpParameters =
		{
			{"displayName", g_cfg.get_string(ECI_Client_Name)},
		{"device", device},
		{"rtpCapabilities", m_recvRtpCapabilities},
		{"sctpCapabilities", m_sctpCapabilities.dump()}
		};
		nlohmann::json join_room =
		{
			{"request",true},
		{"id" , ++m_id}, //  
		{"method" , "join"}, //方法
		{"data" , rtpParameters}
		};
		g_websocket_mgr.send(join_room.dump());
		m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_join_room));
		//m_status = EMediasoup_WebSocket;
		return true;
	}
	bool cclient::_server_RouterRtpCapabilities(const  nlohmann::json & msg)
	{
		NORMAL_EX_LOG("  router rtp capabilities --> ");
		_load(msg["data"]);
		m_status = EMediasoup_Request_Create_Recv_Webrtc_Transport;
		//m_status = EMediasoup_Request_Create_Send_Webrtc_Transport; // 
		return true;
	}

	bool cclient::_server_create_webrtc_transport(const  nlohmann::json & msg)
	{
		NORMAL_EX_LOG(" server create webrtc --> ");
		std::string  transport_id = msg["data"]["id"];

	//	auto data_json_values
		//nlohmann::json data_json_values = msg.get<nlohmann::json::object>("data");
		auto data = msg.find("data");//find
		if (!data->is_object())
		{
			// error --> 
			return false;
		}
		nlohmann::json new_data = data.value();
		auto iceParameters = new_data.find("iceParameters");
		auto iceCandidates = new_data.find("iceCandidates");
		auto dtlsParameters = new_data.find("dtlsParameters");
		auto sctpParameters = new_data.find("sctpParameters");
		nlohmann::json json_iceParameters = iceParameters.value();
		nlohmann::json json_iceCandidates = iceCandidates.value();
		nlohmann::json json_dtlsParameters = dtlsParameters.value();
		nlohmann::json json_sctpParameters = sctpParameters.value();

			NORMAL_EX_LOG("data.value().dump().c_str() = %s\n json_iceParameters = %s\n dtlsParameters = %s", new_data.dump().c_str(), json_iceParameters.dump().c_str(), dtlsParameters.value().dump().c_str());
	 
			/*if (!m_send_transport)
			{
				m_send_transport = new rtc::RefCountedObject<ctransport>(transport_id, this );
				m_send_transport->init(true, transport_id, m_extendedRtpCapabilities,
					json_iceParameters,
					json_iceCandidates,
					json_dtlsParameters,
					json_sctpParameters);
				m_status =  EMediasoup_Request_Create_Recv_Webrtc_Transport;
			}
			else */
				if (!m_recv_transport)
			{

				m_recv_transport = new rtc::RefCountedObject<ctransport>(transport_id, this);
				m_recv_transport->init(false, transport_id, m_extendedRtpCapabilities,
					json_iceParameters,
					json_iceCandidates,
					json_dtlsParameters,
					json_sctpParameters);
				m_status =  EMediasoup_Request_Join_Room;
			}
			else
			{
				ERROR_EX_LOG(" not find  transport id = %s", transport_id.c_str());
			}
		
		
				 
		
		//m_send_transport->webrtc_connect_transport_offer(nullptr);
		//m_send_transport->webrtc_connect_transport_setup_connect("server");
		//m_status = 2;
		return true;
	}
	bool cclient::_server_connect_webrtc_transport(const  nlohmann::json & msg)
	{
		m_status = EMediasoup_Request_Produce_Webrtc_Transport  ;
		return true;
	}
	bool cclient::_server_recv_connect_webrtc_transport(const  nlohmann::json & msg)
	{
		m_status = EMediasoup_Request_Consumer_Webrtc_Transport;
		return true;
	}
	bool cclient::_server_produce_webrtc_transport(const  nlohmann::json & msg)
	{
		std::string produce_id = msg["data"]["id"];
		NORMAL_EX_LOG("produce_id = %s", produce_id.c_str());
		
		m_send_transport->webrtc_transport_produce(produce_id);
		m_status = EMediasoup_WebSocket;
		return true;
	}

	bool cclient::_server_join_room(const  nlohmann::json & msg)
	{
		//m_send_transport->webrtc_connect_transport_offer(nullptr);
		m_status = EMediasoup_WebSocket; 
		return true;
	}



	bool cclient::server_request_new_dataconsumer(const  nlohmann::json & msg)
	{
		std::string transport_id = msg["data"]["id"];
		uint64 id = msg["id"].get<uint64>();
		/*if (transport_id == m_send_transport->get_transportId())
		{
			WARNING_EX_LOG("transport_id = %s ", transport_id.c_str());
			server_reply_new_dataconsumer(id);
			return false;
		}*/
		
		std::string dataProducerId = msg["data"]["dataProducerId"];
		uint16_t streamId = msg["data"]["sctpStreamParameters"]["streamId"].get<uint16_t>();
		m_recv_transport->webrtc_create_consumer(transport_id, dataProducerId, std::to_string(streamId));
		server_reply_new_dataconsumer(id);
		return true;
	}

	 
	bool cclient::server_reply_new_dataconsumer(uint64 id)
	{
		nlohmann::json  reply_datacosumer = 
		{
			{"response",true}, 
			{"id",id}, 
			{"ok" , true}, 
			{"data", nlohmann::json::object()}
		};
		g_websocket_mgr.send(reply_datacosumer.dump());

		return true;
	}
}