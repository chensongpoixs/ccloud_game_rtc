#ifndef _C_CCLIENT_H_
#define _C_CCLIENT_H_
 
#include "cnet_types.h"
#include <string>
#include "json.hpp"
#include "ctransport.h"
#include "cwebsocket_mgr.h"
//#include "csend_transport.h"
//#include "crecv_transport.h"
#include "csingleton.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/ComputeBoundsVisitor>
#include <osgviewer/viewereventhandlers>
#include <osgEarthUtil/UTMGraticule>
#include <osgUtil/Optimizer>
namespace chen {

	class csend_transport;
	class crecv_transport;
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

		//EMediasoup_Produce_Video_Tr
		///////////////////////////////////////////////////////////
		EMediasoup_WebSocket,
		EMediasoup_WebSocket_Close,
		EMediasoup_WebSocket_Destory,
		EMediasoup_WebSocket_Wait,
		//////////////////////////////////////////

		EMediasoup_Reset, //
		EMediasoup_Destory,
		EMediasoup_Wait,
		EMediasoup_Exit,
		//EMediasoup_
	};



	enum  EUI_Type
	{
		EUI_None = 0,
		EUI_Starting,
		EUI_StartEnd

	};

	class cclient
	{
	private:
		typedef bool (cclient::*client_protoo_msg)(const  nlohmann::json & msg);
		typedef bool(cclient::*server_protoo_msg)(const  nlohmann::json & msg);
		 
	public:
		cclient();
		~cclient();


	public:
		bool init(const char * config_name);
		void Loop();
		void stop();

		void Destory();

		//暂时没有使用该api
		void startup_ui();

		// 线程不安全的
		bool webrtc_video(unsigned char * rgba, int32_t width, int32_t height);

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
	private:
		bool _notification_peer_closed(const nlohmann::json & msg);
	public:
		bool server_reply_new_dataconsumer(uint64 id);

		bool _default_replay(const nlohmann::json & reply);
	public:
		bool async_produce();
	private:
		void _presssmsg(std::list<std::string> & msgs);

	private:
		bool _send_request_mediasoup( const std::string& method, const nlohmann::json & data);
		bool _reply_server(uint64 id);

	private:
		void _clear_register();

		void _osg_thread();
	private:
		uint64			m_id;
		bool			m_loaded;
		bool			m_stoped;
		EMediasoup_Type			m_status;
		nlohmann::json	m_extendedRtpCapabilities;
		nlohmann::json	m_recvRtpCapabilities;
		nlohmann::json	m_sctpCapabilities;
		
		rtc::scoped_refptr<csend_transport>	 m_send_transport;
		rtc::scoped_refptr<crecv_transport>	 m_recv_transport ;
		std::map<uint64, client_protoo_msg> m_client_protoo_msg_call;
		std::map<std::string, server_protoo_msg> m_server_protoo_msg_call;
		std::map < std::string, server_protoo_msg> m_server_notification_protoo_msg_call;

		time_t							m_async_data_consumer_t;

		bool							m_produce_consumer; //首先生产 在消费
		std::map < std::string, std::string>	m_peerid_dataconsumer;
		std::set<std::string>			m_peer_map;
		time_t m_reconnect_wait ;
		EUI_Type						m_ui_type;
		cwebsocket_mgr					m_websocket_mgr;
		time_t							m_produce_video;
		bool							m_webrtc_connect;
		osg::ref_ptr<osgViewer::Viewer> m_viewer_ptr;
		std::thread						m_osg_work_thread;
	};
#define  s_client chen::csingleton<chen::cclient>::get_instance()
}

#endif // !_C_CLIENT_H_