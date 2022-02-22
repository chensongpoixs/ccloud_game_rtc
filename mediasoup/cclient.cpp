#include "cclient.h"
#include "mediasoupclient.hpp"
#include "ortc.hpp"
#include "clog.h"
#include "ccfg.h"
#include "cwebsocket_mgr.h"

#include "cdevice.h"
#include "pc/video_track_source.h"
#include "crecv_transport.h"
#include "csend_transport.h"
#include "cinput_device.h"
namespace syz {

	///////////////////////////////////////mediasoup///////////////////////////////////////////////////////
	static const char * MEDIASOUP_REQUEST_METHOD_GETROUTERRTPCAPABILITIES = "getRouterRtpCapabilities";
	static const char * MEDIASOUP_REQUEST_METHOD_CREATEWEBRTCTRANSPORT = "createWebRtcTransport";
	static const char * MEDIASOUP_REQUEST_METHOD_CONNECTWEBRTCTRANSPORT = "connectWebRtcTransport";
	static const char * MEDIASOUP_REQUEST_METHOD_PRODUCE = "produce";
	static const char * MEDIASOUP_REQUEST_METHOD_JOIN = "join";
	
	
	////////////////////////////////////////websocket protoo /////////////////////////////////////////////////////////////////
	
	static const char * WEBSOCKET_PROTOO_METHOD = "method";
	static const char * WEBSOCKET_PROTOO_ID = "id";
	static const char * WEBSOCKET_PROTOO_DATA = "data";
	static const char * WEBSOCKET_PROTOO_REQUEST = "request";
	static const char * WEBSOCKET_PROTOO_RESPONSE = "response";
	static const char * WEBSOCKET_PROTOO_NOTIFICATION = "notification";
	static const char * WEBSOCKET_PROTOO_OK = "ok";







	/////////////////////////////////////////webrtc////////////////////////////////////////////////////////////
	static const char * WEBRTC_SERVER = "server";
	static const char * WEBRTC_CLIENT = "client";


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define  WEBSOCKET_PROTOO_CHECK_RESPONSE()  if (msg.find(WEBSOCKET_PROTOO_OK) == msg.end())\
	{\
		ERROR_EX_LOG(" [msg = %s] not find 'ok' failed !!!", msg.dump().c_str());\
		return false;\
	}\
	if (msg.find(WEBSOCKET_PROTOO_DATA) == msg.end())\
	{\
		ERROR_EX_LOG("  [msg = %s] not find 'data' failed !!!", msg.dump().c_str());\
		return false;\
	}\
	bool  ok = msg[WEBSOCKET_PROTOO_OK].get<bool>();\
	if (!ok)\
	{\
		ERROR_EX_LOG("  not ok  [msg = %s] !!!", msg.dump().c_str());\
		return false;\
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////

	const uint32_t TICK_TIME = 200;

	cclient::cclient()
		:m_id(100000)
		,m_loaded(false)
		, m_stoped(false)
		, m_status(EMediasoup_None)
		, m_produce_consumer(true)
		, m_ui_type(EUI_None) {}
	cclient::~cclient(){}

	
	bool cclient::init(const char * config_name)
	{
		printf("Log init ...\n");
		if (!LOG::init(ELogStorageScreenFile))
		{
			std::cerr << " log init failed !!!";
			return false;
		}
		SYSTEM_LOG("Log init ...\n");
		bool init = g_cfg.init(config_name);
		if (!init)
		{
			//RTC_LOG(LS_ERROR) << "config init failed !!!" << config_name;
			ERROR_EX_LOG("config init failed !!! config_name = %s", config_name);
			return false;
		}
		SYSTEM_LOG("config init ok !!!");
		if (!s_input_device.init())
		{
			ERROR_EX_LOG("init input_device mouble !!!  ");
			return false;
		}
		//一上来就推流
		m_produce_consumer = true;
		SYSTEM_LOG("input device  init ok !!!");
		m_server_protoo_msg_call.insert(std::make_pair("newDataConsumer", &cclient::server_request_new_dataconsumer));
		m_server_notification_protoo_msg_call.insert(std::make_pair("peerClosed", &cclient::_notification_peer_closed));
		SYSTEM_LOG("client init ok !!!");
		m_webrtc_connect = false;
		//m_osg_work_thread = std::thread(&cclient::_osg_thread, this);
		//SYSTEM_LOG("osg video capturer thread ok !!!");
		mediasoupclient::Initialize();
		return true;
	}
	void cclient::stop()
	{
		m_stoped = true;
		m_webrtc_connect = false;
	}
	void cclient::Loop()
	{
		std::string ws_url = "ws://" + g_cfg.get_string(ECI_MediaSoup_Host) + ":" + std::to_string(g_cfg.get_int32(ECI_MediaSoup_Http_Port)) + "/?roomId=" + g_cfg.get_string(ECI_Room_Name) + "&peerId=" + g_cfg.get_string(ECI_Client_Name);//ws://127.0.0.1:8888/?roomId=syzsong&peerId=xiqhlyrn", "http://127.0.0.1:8888")
		std::string origin = "http://" + g_cfg.get_string(ECI_MediaSoup_Host) + ":" + std::to_string(g_cfg.get_int32(ECI_MediaSoup_Http_Port));
		std::list<std::string> msgs;
		time_t cur_time = ::time(NULL);
		 
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;
		while (!m_stoped)
		{
			pre_time = std::chrono::steady_clock::now();
			switch (m_status)
			{
			case EMediasoup_WebSocket_Init: // None
			case EMediasoup_None:
			{  // websocket connect 
				// 1. connect suucer status -> 1
				//g_websocket_mgr.destroy();
				if (!m_websocket_mgr.init(ws_url ))
				{
					//RTC_LOG(LS_ERROR) << "weboscket connect failed !!! url = " << ws_url;
					WARNING_EX_LOG("weboscket connect url = %s failed !!!   ", ws_url.c_str());
					m_status = EMediasoup_Reset;
					continue;;
				}
				 
				if (!m_websocket_mgr.startup())
				{
					m_status = EMediasoup_Reset;
					continue;
				}
				if (!_send_router_rtpcapabilities())
				{
					m_status = EMediasoup_Reset;
					continue;
				}
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
				if (!_send_create_webrtc_transport())
				{
					m_status = EMediasoup_Reset;
					continue;
				}
				// 1.2 创建Send WebRTC transport id -> 
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Connect_Webrtc_Transport: //暂时该状态没有使用 TODO@syzsong  - 20220215 
			{
				// 1.  wait server call transport dtls info
				// 1.1 Send WebRTC Connect -> 
				m_send_transport->webrtc_connect_transport_offer( );
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
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Send_Connect_Set:
			{
				m_send_transport->webrtc_connect_transport_setup_connect(WEBRTC_SERVER);
				m_status = EMediasoup_WebSocket;
				break;
			}
			case EMediasoup_Request_Recv_Connect_Set:
			{
				m_recv_transport->webrtc_connect_transport_setup_connect(WEBRTC_CLIENT);
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
			case EMediasoup_WebSocket: // wait server msg 
			{
				
				m_websocket_mgr.presssmsg(msgs);
				
				
				if (!msgs.empty() && m_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
				{
					_presssmsg(msgs);
				}

				if (m_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
				{
					m_status = EMediasoup_Reset;
					msgs.clear();
				}
				if (m_reconnect_wait && g_cfg.get_int32(ECI_ReconnectWait) > 0)
				{
					if (m_reconnect_wait < ::time(NULL))
					{
						m_status = EMediasoup_Reset;
						m_reconnect_wait = 0;
					}
				}
				if (m_status == EMediasoup_WebSocket && g_cfg.get_int32(ECI_ProduceVideo) > 0 && m_webrtc_connect)
				{
					if (m_produce_video < ::time(NULL))
					{
						static bool video_produce = true;
						m_produce_video = ::time(NULL) + g_cfg.get_int32(ECI_ProduceVideo);
						if (video_produce)
						{
							m_send_transport->Pause();
							video_produce = false;
						}
						else
						{
							m_send_transport->Resume();
							video_produce = true;
						}
						
					}
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
				// TODO@syzsong 20220208 ---> 增加时间
				std::this_thread::sleep_for(std::chrono::milliseconds(g_cfg.get_uint32(ECI_WebSocket_Reconnect)));
				m_status = EMediasoup_WebSocket_Init;
				break;
			}
			case EMediasoup_Reset:
			case EMediasoup_Destory:
			{
				// destory all websocket, webrtc destroy --> ok !!
				if (m_recv_transport)
				{
					m_recv_transport->Destroy();
					m_recv_transport = nullptr;
				}
				if (m_send_transport)
				{
					m_send_transport->Destroy();
					m_send_transport = nullptr;
				}
				m_websocket_mgr.destroy();
				_clear_register();
				m_produce_consumer = true;
				m_peer_map.clear();

				// 有人的时候推流 
				//m_produce_consumer = false;
				//m_ui_type = EUI_None;
				cur_time = ::time(NULL) + g_cfg.get_uint32(ECI_WebSocket_Reconnect);
				NORMAL_EX_LOG("reconnect  wait [%u s]  ...", g_cfg.get_uint32(ECI_WebSocket_Reconnect));
				m_status = EMediasoup_Wait;
				break;
			}
			case  EMediasoup_Wait:
			{
				if (cur_time < ::time(NULL))
				{
					m_status = EMediasoup_WebSocket_Init;
				}
				break;
			}
			case EMediasoup_Exit:
			{

				break;
			}
			default:
			{
				ERROR_EX_LOG("client not find status = %u", m_status);
			}
				break;
			}
			if (!m_stoped)
			{
				cur_time_ms = std::chrono::steady_clock::now();
				dur = cur_time_ms - pre_time;
				ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				if (elapse < TICK_TIME)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME- elapse));
				}
				
			}
			
			
		}
		
	}
	void cclient::_presssmsg(std::list<std::string> & msgs)
	{
		nlohmann::json response;
		while (!msgs.empty() && m_websocket_mgr.get_status() == CWEBSOCKET_MESSAGE)
		{
			std::string msg = std::move(msgs.front());
			msgs.pop_front();
			NORMAL_EX_LOG("server ====> msg = %s", msg.c_str());
			
			try
			{
				response = nlohmann::json::parse(msg);
			}
			catch (const std::exception&)
			{
				ERROR_EX_LOG("websocket protoo [msg = %s] json parse failed !!!", msg.c_str());
				continue;
			}
			
			 
			if (response.find(WEBSOCKET_PROTOO_NOTIFICATION) != response.end())
			{
				//NORMAL_EX_LOG("notification --> msg = %s", msg);
				auto method_iter = response.find(WEBSOCKET_PROTOO_METHOD);
				if (method_iter == response.end())
				{
					WARNING_EX_LOG("notification websocket protoo not find method name  msg = %s", response.dump().c_str());
					continue;
				}
				std::string method = response[WEBSOCKET_PROTOO_METHOD];
				std::map<std::string, server_protoo_msg>::iterator iter = m_server_notification_protoo_msg_call.find(method);
				if (iter != m_server_notification_protoo_msg_call.end())
				{

					(this->*(iter->second))(response);
					//server_request_new_dataconsumer(response);
				}
				else
				{
					//_default_replay(response);
					//WARNING_EX_LOG("server request client not find method  response = %s", response.dump().c_str());
				}

			}
			else if (response.find(WEBSOCKET_PROTOO_RESPONSE) != response.end())//response
			{
				auto id_iter = response.find(WEBSOCKET_PROTOO_ID);
				if (id_iter == response.end())
				{
					WARNING_EX_LOG("websocket protoo response not find 'id' [response = %s] filed !!! ", response.dump().c_str());
					continue;
				}
				uint64 id = response["id"].get<uint64>();
				std::map<uint64, client_protoo_msg>::const_iterator iter = m_client_protoo_msg_call.find(id);
				if (iter == m_client_protoo_msg_call.end())
				{
					ERROR_EX_LOG("not find id = %u, msg = %s", id, msg.c_str());
				}
				else
				{
					if (!(this->*(iter->second))(response))
					{
						m_status = EMediasoup_Reset;
						return;
					}
					m_client_protoo_msg_call.erase(iter);
				}
			}
			else if (response.find(WEBSOCKET_PROTOO_REQUEST) != response.end())
			{
				//服务器请求客户端响应的请求
				auto method_iter = response.find(WEBSOCKET_PROTOO_METHOD);
				if (method_iter == response.end())
				{
					WARNING_EX_LOG("websocket protoo not find method name  msg = %s", response.dump().c_str());
					continue;
				}
				std::string method = response[WEBSOCKET_PROTOO_METHOD];
				std::map<std::string, server_protoo_msg>::iterator iter =  m_server_protoo_msg_call.find(method);
				if (iter != m_server_protoo_msg_call.end())
				{
					
					(this->*(iter->second))(response);
					//server_request_new_dataconsumer(response);
				}
				else
				{
					_default_replay(response);
					//WARNING_EX_LOG("server request client not find method  response = %s", response.dump().c_str());
				}
			}
			else
			{
				ERROR_EX_LOG(" not find msg type !!! msg = %s", msg.c_str());
			}
		}
	}
	void cclient::startup_ui()
	{
		if (!m_recv_transport)
		{
			WARNING_EX_LOG(" startup ui m_recv_transport == nullptr !!!");
			return;
		}
		if (!m_send_transport)
		{
			WARNING_EX_LOG(" startup ui m_send_transport == nullptr !!!");
			return;
		}
		if (m_ui_type == EUI_Starting || m_ui_type == EUI_StartEnd)
		{
			WARNING_EX_LOG("startup ui ... ui_type = %d", m_ui_type);
			return;
		}
		m_status = EMediasoup_Request_Connect_Webrtc_Transport;
		m_ui_type = EUI_Starting;
		m_produce_consumer = true;
	}
	void cclient::Destory()
	{
		 

		if (m_osg_work_thread.joinable())
		{
			m_osg_work_thread.join();
		}
		//m_stoped = true;
		if (m_send_transport)
		{
			m_send_transport->Destroy();
			m_send_transport = nullptr;
		}
		SYSTEM_LOG("m_send_transport ok !!!");
		if (m_recv_transport)
		{
			m_recv_transport->Destroy();
			m_recv_transport = nullptr;
		}
		SYSTEM_LOG("m_recv_transport ok !!!");
		
		m_websocket_mgr.destroy();
		SYSTEM_LOG("g_websocket_mgr ok !!!");
		m_peer_map.clear();
		_clear_register();
		m_produce_consumer = true;
		mediasoupclient::Cleanup();
		LOG::destroy();
	  
	}
	bool cclient::_load(nlohmann::json routerRtpCapabilities)
	{
		if (m_loaded)
		{
			return true;
		}
		nlohmann::json nativeRtpCapabilities;
		if (!deivce::GetNativeRtpCapabilities(nativeRtpCapabilities))
		{
			ERROR_EX_LOG("GetNativeRtpCapabilities failed !!!");
		
			return false;
		}
		try {

		
			if (!mediasoupclient::ortc::getExtendedRtpCapabilities(m_extendedRtpCapabilities, nativeRtpCapabilities, routerRtpCapabilities))
			{
				ERROR_EX_LOG("getExtendedRtpCapabilities failed !!!");
				return false;
			}
		
			// Generate our receiving RTP capabilities for receiving media.
			m_recvRtpCapabilities = mediasoupclient::ortc::getRecvRtpCapabilities(m_extendedRtpCapabilities);


			m_sctpCapabilities = deivce::GetNativeSctpCapabilities();
		}
		catch (...)
		{
			ERROR_EX_LOG("_load [routerRtpCapabilities = %s ]failed !!!", routerRtpCapabilities.dump().c_str());
			return false;
		}
		m_loaded = true;
		return true;
	}

	void cclient::transportofferasner(bool send, bool success)
	{
		if (success)
		{
			if (send)
			{
				m_status = EMediasoup_Request_Send_Connect_Set;
			}
			else
			{
				m_status = EMediasoup_Request_Recv_Connect_Set;;
			}
		}
		else
		{
			m_status = EMediasoup_Reset;
			ERROR_EX_LOG("- EMediasoup_Reset --recv send = %d failed !!!", send);
		}
	}
	bool cclient::_send_router_rtpcapabilities()
	{
		if (!_send_request_mediasoup(MEDIASOUP_REQUEST_METHOD_GETROUTERRTPCAPABILITIES, nlohmann::json::object()))
		{
			m_status = EMediasoup_Reset;
			WARNING_EX_LOG("send request router rtcpcapabilities failed !!!");
			return false;
		}
	
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
			if (!_send_request_mediasoup(MEDIASOUP_REQUEST_METHOD_CREATEWEBRTCTRANSPORT, data))
			{
				WARNING_EX_LOG("send request create send webrtc transport  failed !!!");
				return false;
			}
			 
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

			if (!_send_request_mediasoup(MEDIASOUP_REQUEST_METHOD_CREATEWEBRTCTRANSPORT, data))
			{
				WARNING_EX_LOG("send request create recv webrtc transport  failed !!!");
				return false;
			}
			 
		}
		
		m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_create_webrtc_transport));
		return true;
	}
	bool cclient::_send_connect_webrtc_transport(nlohmann::json dtlsparameters)
	{
		std::string str = dtlsparameters["role"];
		NORMAL_EX_LOG("connect webrtc -> role = %s", str.c_str());
		std::string transportId;
		if (str == WEBRTC_SERVER)
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
		 
	 
		
		if (str == WEBRTC_SERVER)
		{
			if (!_send_request_mediasoup(MEDIASOUP_REQUEST_METHOD_CONNECTWEBRTCTRANSPORT, data))
			{
				m_status = EMediasoup_Reset;
				WARNING_EX_LOG("send request connect recv webrtc transport  failed !!!");
				return false;
			}
			m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_connect_webrtc_transport));
		}
		else
		{
			if (!_send_request_mediasoup(MEDIASOUP_REQUEST_METHOD_CONNECTWEBRTCTRANSPORT, data))
			{
				m_status = EMediasoup_Reset;
				WARNING_EX_LOG("send request connect recv webrtc transport  failed !!!");
				return false;
			}
			m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_recv_connect_webrtc_transport));
		}
		
		//m_status = EMediasoup_WebSocket;
		return true;
	}

	bool cclient::_send_produce_webrtc_transport()
	{
		nlohmann::json data = nlohmann::json::object();
		
		
		std::string transportId = m_send_transport->get_transportId();
		std::string kind = m_send_transport->get_kind();
		nlohmann::json rtp = m_send_transport->get_sending_rtpParameters();
		data =
		{
			{"transportId", transportId},
			{"kind", kind},
			{"rtpParameters", rtp},
			{"appData", nlohmann::json::object()}
		};
		
		 
		if (!_send_request_mediasoup(MEDIASOUP_REQUEST_METHOD_PRODUCE, data))
		{
			m_status = EMediasoup_Reset;
			WARNING_EX_LOG("send request connect recv webrtc transport  failed !!!");
			return false;
		}
		 
		 
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

		if (!_send_request_mediasoup(MEDIASOUP_REQUEST_METHOD_JOIN, rtpParameters))
		{
			m_status = EMediasoup_Reset;
			WARNING_EX_LOG("send request join room   failed !!!");
			return false;
		}
		 
		m_client_protoo_msg_call.insert(std::make_pair(m_id, &cclient::_server_join_room));
		//m_status = EMediasoup_WebSocket;
		return true;
	}
	bool cclient::webrtc_video(unsigned char * rgba, int32_t width, int32_t height)
	{
		if (!m_webrtc_connect)
		{
			WARNING_EX_LOG("not connect webrtc video wait !!!");
			return false;
		}
		if (!m_send_transport)
		{
			WARNING_EX_LOG("m_send_transport == nullptr !!!");
			return false;
		}
		return m_send_transport->webrtc_video(rgba, width, height);
		 
	}
	bool cclient::webrtc_run()
	{
		if (!m_webrtc_connect)
		{
			WARNING_EX_LOG("not connect webrtc video wait !!!");
			return false;
		}
		return true;
	}
	bool cclient::_server_RouterRtpCapabilities(const  nlohmann::json & msg)
	{
		WEBSOCKET_PROTOO_CHECK_RESPONSE();
		
		nlohmann::json data_rtp = msg[WEBSOCKET_PROTOO_DATA];
		if (!mediasoupclient:: ortc::validateRtpCapabilities(data_rtp))
		{
			WARNING_EX_LOG("RouterRtpCapabilities validate rtp capabilities [data = %s] failed !!!", data_rtp.dump().c_str());
			m_status = EMediasoup_Reset;
			return false;
		}
		if (!_load(data_rtp))
		{
			WARNING_EX_LOG("RouterRtpCapabilities _load[data_rtp = %s] failed !!!", data_rtp.dump().c_str());
			m_status = EMediasoup_Reset;
			return false;
		}
		//m_status = EMediasoup_Request_Create_Recv_Webrtc_Transport;
		m_status = EMediasoup_Request_Create_Send_Webrtc_Transport; // 
		return true;
	}

	bool cclient::_server_create_webrtc_transport(const  nlohmann::json & msg)
	{
		//NORMAL_EX_LOG(" server create webrtc --> ");
		WEBSOCKET_PROTOO_CHECK_RESPONSE();
		  
		nlohmann::json new_data = msg[WEBSOCKET_PROTOO_DATA];
		if (new_data.find("id") == new_data.end())
		{
			ERROR_EX_LOG("_server_create_webrtc_transport [msg = %s] not find 'data.id' failed !!!", msg.dump().c_str());

			return false;
		}
		auto iceParameters = new_data.find("iceParameters");
		auto iceCandidates = new_data.find("iceCandidates");
		auto dtlsParameters = new_data.find("dtlsParameters");
		auto sctpParameters = new_data.find("sctpParameters");

		if (iceParameters == new_data.end())
		{
			ERROR_EX_LOG("_server_create_webrtc_transport [msg = %s] not find 'data.iceParameters' failed !!!", msg.dump().c_str());

			return false;
		}
		std::string  transport_id = new_data["id"];
		
		nlohmann::json json_iceParameters = iceParameters.value();
		nlohmann::json json_iceCandidates = iceCandidates.value();
		nlohmann::json json_dtlsParameters = dtlsParameters.value();
		nlohmann::json json_sctpParameters = sctpParameters.value();

			//NORMAL_EX_LOG("data.value().dump().c_str() = %s\n json_iceParameters = %s\n dtlsParameters = %s", new_data.dump().c_str(), json_iceParameters.dump().c_str(), dtlsParameters.value().dump().c_str());
	 
		if (!m_send_transport)
		{
			m_send_transport = new rtc::RefCountedObject<csend_transport>(transport_id, this);
			m_send_transport->init(transport_id, m_extendedRtpCapabilities,
				json_iceParameters,
				json_iceCandidates,
				json_dtlsParameters,
				json_sctpParameters);
			m_status = EMediasoup_Request_Create_Recv_Webrtc_Transport;
		}
		else if (!m_recv_transport)
		{

			m_recv_transport = new rtc::RefCountedObject<crecv_transport>(transport_id, this);
			m_recv_transport->init(transport_id, m_extendedRtpCapabilities,
				json_iceParameters,
				json_iceCandidates,
				json_dtlsParameters,
				json_sctpParameters);
			m_status = EMediasoup_Request_Join_Room;
		}
		else
		{
			ERROR_EX_LOG(" not find  transport id = %s", transport_id.c_str());
		}
		
		
				 
		
		 
		return true;
	}
	bool cclient::_server_connect_webrtc_transport(const  nlohmann::json & msg)
	{
		WEBSOCKET_PROTOO_CHECK_RESPONSE();
		m_status = EMediasoup_Request_Produce_Webrtc_Transport  ;
		return true;
	}
	bool cclient::_server_recv_connect_webrtc_transport(const  nlohmann::json & msg)
	{
		WEBSOCKET_PROTOO_CHECK_RESPONSE();
		m_status = EMediasoup_Request_Consumer_Webrtc_Transport;
		return true;
	}
	bool cclient::_server_produce_webrtc_transport(const  nlohmann::json & msg)
	{
		WEBSOCKET_PROTOO_CHECK_RESPONSE();

		nlohmann::json data = msg[WEBSOCKET_PROTOO_DATA];
		if (data.find("id") == data.end())
		{
			ERROR_EX_LOG("[msg = %s] not find  data.id failed !!!", msg.dump().c_str());
			return false;
		}
		std::string produce_id = data["id"]; 
		m_send_transport->webrtc_transport_produce(produce_id);
		
		m_status = EMediasoup_WebSocket;
		m_produce_consumer = false;
		m_recv_transport->webrtc_create_all_wait_consumer();
		m_produce_video = ::time(NULL) + g_cfg.get_int32(ECI_ProduceVideo);
		m_webrtc_connect = true;
 		return true;
	}

	bool cclient::_server_join_room(const  nlohmann::json & msg)
	{
		WEBSOCKET_PROTOO_CHECK_RESPONSE();
		m_send_transport->webrtc_connect_transport_offer( );
		//m_produce_consumer = false;
		if (g_cfg.get_int32(ECI_ReconnectWait) > 0)
		{
			m_reconnect_wait = ::time(NULL) + g_cfg.get_int32(ECI_ReconnectWait);
		}
		//m_status = EMediasoup_WebSocket; 
		return true;
	}



	bool cclient::server_request_new_dataconsumer(const  nlohmann::json & msg)
	{
		auto data_iter = msg.find(WEBSOCKET_PROTOO_DATA);
		if (data_iter == msg.end())
		{
			WARNING_EX_LOG("not find 'data'  ");
			return false;
		}
		auto id_iter = data_iter.value().find(WEBSOCKET_PROTOO_ID);
		if (id_iter ==  data_iter.value().end())
		{
			WARNING_EX_LOG("not find   'id' ");
			return false;
		}
		std::string transport_id =  msg["data"]["id"];
		uint64 id = msg["id"].get<uint64>();
	
		/*if (transport_id == m_send_transport->get_transportId())
		{
			WARNING_EX_LOG("transport_id = %s ", transport_id.c_str());
			server_reply_new_dataconsumer(id);
			return false;
		}*/

		nlohmann::json data = msg[WEBSOCKET_PROTOO_DATA];
		std::string peerId;
		auto peerIditer = data.find("peerId");
		if (peerIditer != data.end() && !peerIditer.value().is_null())
		{
			peerId = data["peerId"].get<std::string>();
			m_peer_map.insert(peerId);
		}
		
		std::string dataProducerId = msg["data"]["dataProducerId"];
		if (!peerId.empty())
		{
			if (!m_peerid_dataconsumer.insert(std::make_pair(peerId, transport_id)).second)
			{
				WARNING_EX_LOG("insert peerId = data failed !!!", peerId.c_str());
			}
		}
		uint16_t streamId = msg["data"]["sctpStreamParameters"]["streamId"].get<uint16_t>();
		if (m_produce_consumer)
		{
			m_recv_transport->webrtc_consumer_wait(transport_id, dataProducerId, std::to_string(streamId));
		}
		else
		{
			m_recv_transport->webrtc_create_consumer(transport_id, dataProducerId, std::to_string(streamId));
		}
		server_reply_new_dataconsumer(id);
		return true;
	}

	bool cclient::_notification_peer_closed(const nlohmann::json & msg)
	{

		auto data_iter = msg.find(WEBSOCKET_PROTOO_DATA);
		if (data_iter == msg.end())
		{
			WARNING_EX_LOG("not find 'data'  ");
			return false;
		}
		auto id_iter = data_iter.value().find("peerId");
		if (id_iter == data_iter.value().end())
		{
			WARNING_EX_LOG("not find   'peerId' ");
			return false;
		}
		std::string peerId = msg[WEBSOCKET_PROTOO_DATA]["peerId"];
		 
		
		m_peer_map.erase(peerId);
		NORMAL_EX_LOG("peerId = %s exit ", peerId.c_str());
		/*std::map < std::string, std::string>::iterator iter =  m_peerid_dataconsumer.find(peerId);
		if (iter != m_peerid_dataconsumer.end())
		{
			m_recv_transport->close_dataconsumer(iter->second);
			m_peerid_dataconsumer.erase(iter->first);
		}
		else
		{
			WARNING_EX_LOG("not find peerId = %s failed !!!", peerId.c_str());
		}*/
		return true;
	}
	bool cclient::server_reply_new_dataconsumer(uint64 id)
	{
		 
		if (!_reply_server(id))
		{
			WARNING_EX_LOG("reply new dataconsumer failed !!!");
		}
		return true;
	}
	bool cclient:: async_produce()
	{
		//m_send_transport->webrtc_connect_transport_offer(nullptr);
		//m_async_data_consumer_t = ::time(NULL) + 50;
		return true;
	}



	bool cclient::_send_request_mediasoup(  const std::string& method, const nlohmann::json & data)
	{
		if (m_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
		{
			WARNING_EX_LOG("websocket mgr status = %d !!!", m_websocket_mgr.get_status());
			return false;
		}
		nlohmann::json request_data =
		{
			{WEBSOCKET_PROTOO_REQUEST  ,true},
			{WEBSOCKET_PROTOO_ID , ++m_id}, //  
			{WEBSOCKET_PROTOO_METHOD , method}, //方法
			{WEBSOCKET_PROTOO_DATA , data}
		};
		m_websocket_mgr.send(request_data.dump());
		return true;
	}


	bool cclient::_reply_server(uint64 id)
	{
		if (m_websocket_mgr.get_status() != CWEBSOCKET_MESSAGE)
		{
			WARNING_EX_LOG("websocket mgr status = %d !!!", m_websocket_mgr.get_status());
			return false;
		}
		nlohmann::json  reply =
		{
			{WEBSOCKET_PROTOO_RESPONSE,true},
			{WEBSOCKET_PROTOO_ID,id},
			{WEBSOCKET_PROTOO_OK , true},
			{WEBSOCKET_PROTOO_DATA, nlohmann::json::object()}
		};
		m_websocket_mgr.send(reply.dump());
		return true;
	}
	void cclient::_clear_register()
	{
		m_client_protoo_msg_call.clear();
	}

	void cclient::_osg_thread()
	{
		 
	}
	bool cclient::_default_replay(const nlohmann::json & reply)
	{
		auto iter =  reply.find(WEBSOCKET_PROTOO_ID);
		if (iter != reply.end())
		{
			uint64 id = reply[WEBSOCKET_PROTOO_ID].get<uint64>();
			if (!_reply_server(id))
			{
				WARNING_EX_LOG("reply send failed  msg = %s", reply.dump().c_str());
				return false;
			}
			return true;
		}
		WARNING_EX_LOG("reply not find 'id' failed  msg = %s", reply.dump().c_str());
		return false;
	}
}