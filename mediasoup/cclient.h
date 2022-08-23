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
//#include "cdesktop_capture.h"
#include "cmediasoup_mgr.h"
namespace chen {
	class DesktopCapture;
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


	struct cosg_webrtc_rgba
	{
		unsigned char* m_rgba_ptr;
		int32 m_width;
		int32 m_height;
		cosg_webrtc_rgba()
			: m_rgba_ptr(nullptr)
			, m_width(0)
			, m_height(0) {}
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
		bool init(uint32 gpu_index);
		void Loop(const std::string& mediasoupIp, uint16_t port, const std::string& roomName, const std::string& clientName
			, uint32_t reconnect_waittime);
		void stop();

		void Destory();

		//暂时没有使用该api
		void startup_ui();

		// 线程不安全的
		bool webrtc_video(unsigned char * rgba, uint32 fmt,  int32_t width, int32_t height);
		bool webrtc_texture(void * texture, uint32 fmt, int32_t width, int32_t height);
		bool webrtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32 fmt, int32_t width, int32_t height);
		bool webrtc_video(const webrtc::VideoFrame& frame);
		bool webrtc_run();

		void set_mediasoup_status_callback(cmediasoup::mediasoup_status_update_cb callback) { m_mediasoup_status_callback = callback ; }


		void transportofferasner(bool send, bool success);

		//webrtc连接失败回调用重新连接哈
		void webrtc_connect_failed_callback();
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
		void _osg_copy_rgba_thread();
	private:
		void _mediasoup_status_callback(uint32 status, uint32 error = 0);
	private:
		uint64			m_id;
		bool			m_loaded;
		bool			m_stoped;
		std::string				m_room_name;
		std::string				m_client_name;
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
		 
		std::thread						m_osg_work_thread;
		std::vector<cosg_webrtc_rgba>   m_frame_rgba_vec; // 
		uint64							m_osg_frame;
		uint64							m_webrtc_frame;
		std::thread						m_osg_copy_thread;


		std::shared_ptr<DesktopCapture> 		m_desktop_capture_ptr;

		cmediasoup::mediasoup_status_update_cb		m_mediasoup_status_callback;
		uint32							m_websocket_timer;
		bool							m_send_produce_video_msg;
	};
//#define  s_client chen::csingleton<chen::cclient>::get_instance()
}

#endif // !_C_CLIENT_H_