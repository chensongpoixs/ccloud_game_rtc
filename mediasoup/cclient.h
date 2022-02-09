#ifndef _C_CCLIENT_H_
#define _C_CCLIENT_H_
 
#include "cnet_types.h"
#include <string>
#include "json.hpp"
#include "ctransport.h"
namespace chen {


	enum EMediasoup_Type
	{
		EMediasoup_None = 0,
		EMediasoup_WebSocket_Init,
		//EMediasoup_Request_Rtp_Capabilities,
		EMediasoup_Request_Create_Send_Webrtc_Transport,
		EMediasoup_Request_Create_Recv_Webrtc_Transport,
		
		EMediasoup_Request_Join_Room, // TODO@chensong 20220208 --> 先加入房间 后在开始生产与消费的操作
		EMediasoup_Request_Connect_Webrtc_Transport,
		EMediasoup_Request_Send_Connect_Set,
		EMediasoup_Request_Recv_Connect_Set,
		EMediasoup_Request_Produce_Webrtc_Transport,
		EMediasoup_Request_Consumer_Webrtc_Transport,
		///////////////////////////////////////////////////////////
		EMediasoup_WebSocket,
		EMediasoup_WebSocket_Close,
		EMediasoup_WebSocket_Destory,
		EMediasoup_WebSocket_Wait,
		//EMediasoup_
	};


	class cclient
	{
	private:
		typedef bool(cclient::*server_protoo_msg)(const  nlohmann::json & msg);
		typedef bool (cclient::*client_protoo_msg)(const  nlohmann::json & msg);
	public:
		cclient();
		~cclient();


	public:
		bool init(const char * config_name);
		void Loop();


		void Destory();

		void transportofferasner(bool send, bool success);
	private:

		bool _load(nlohmann::json routerRtpCapabilities);


		
	//////////////////////////////////////////////////////////////////////////////////////////////
		// 客户端请求服务器
	public:
		bool _send_router_rtpcapabilities();
		bool _send_create_webrtc_transport();
		bool _send_connect_webrtc_transport(nlohmann::json dtlsparameters);
		bool _send_produce_webrtc_transport();

		bool _send_join_room();
	public:
		bool _server_RouterRtpCapabilities(const  nlohmann::json & msg);
		bool _server_create_webrtc_transport(const  nlohmann::json & msg);
		bool _server_connect_webrtc_transport(const  nlohmann::json & msg);

		bool _server_recv_connect_webrtc_transport(const  nlohmann::json & msg);

		bool _server_produce_webrtc_transport(const  nlohmann::json & msg);

		bool _server_join_room(const  nlohmann::json & msg);


		////////////////////////////////////////////////////////////////////////////////////////
		// 请求客户端响应的
	public:
		bool server_request_new_dataconsumer(const  nlohmann::json & msg);

	public:
		bool server_reply_new_dataconsumer(uint64 id);


	private:
		void _presssmsg(std::list<std::string> & msgs);
	private:
		uint64			m_id;
		bool			m_loaded;
		bool			m_stoped;
		EMediasoup_Type			m_status;
		nlohmann::json	m_extendedRtpCapabilities;
		nlohmann::json	m_recvRtpCapabilities;
		nlohmann::json	m_sctpCapabilities;;
		
		rtc::scoped_refptr<ctransport>	 m_send_transport{nullptr};
		rtc::scoped_refptr<ctransport>	 m_recv_transport{nullptr};
		std::map<uint64, client_protoo_msg> m_client_protoo_msg_call;
		std::map<std::string, server_protoo_msg> m_server_protoo_msg_call;

	};
}

#endif // !_C_CLIENT_H_