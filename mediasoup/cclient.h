#ifndef _C_CLIENT_H_
#define _C_CLIENT_H_
 
#include "cnet_types.h"
#include <string>
#include "json.hpp"
#include "ctransport.h"
namespace chen {

	class cclient
	{
	private:
		typedef bool(cclient::*server_protoo_msg_call)(const  nlohmann::json & msg);
	public:
		cclient();
		~cclient();


	public:
		bool init(const char * config_name);
		void Loop();


		void Destory();

		void transportofferasner(bool success);
	private:

		bool _load(nlohmann::json routerRtpCapabilities);


		

	public:
		bool _send_router_rtpcapabilities();
		bool _send_create_webrtc_transport();
		bool _send_connect_webrtc_transport(nlohmann::json dtlsparameters);
		bool _send_produce_webrtc_transport();
	public:
		bool _server_RouterRtpCapabilities(const  nlohmann::json & msg);
		bool _server_create_webrtc_transport(const  nlohmann::json & msg);
		bool _server_connect_webrtc_transport(const  nlohmann::json & msg);
		bool _server_produce_webrtc_transport(const  nlohmann::json & msg);
	private:
		uint64			m_id;
		bool			m_loaded;
		bool			m_stoped;
		int32			m_status;
		nlohmann::json	m_extendedRtpCapabilities;
		nlohmann::json	m_recvRtpCapabilities;
		nlohmann::json	m_sctpCapabilities;;
		
		std::unique_ptr<ctransport>	 m_send_transport{nullptr};
		std::map<uint64, server_protoo_msg_call> m_server_protoo_msg_call;

	};
}

#endif // !_C_CLIENT_H_