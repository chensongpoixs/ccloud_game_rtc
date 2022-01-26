#include "croom_client.h"
#include "cwebsocket_mgr.h"
#include "clog.h"
 

namespace chen {
	croom_client::croom_client()
	{}
	croom_client::~croom_client()
	{}

 
	void croom_client::request_router_rtp_capabilities()
	{
		nlohmann::json getRouterRtpCapabilities =
		{
			
		{ "request",true     },
		{ "id",           ++m_id          },
		{ "method" , "getRouterRtpCapabilities"},
		{ "data", {{}} }
		};
		g_websocket_mgr.send(getRouterRtpCapabilities.dump());
		if (!m_server_call.insert(std::make_pair(m_id, &croom_client::server_call_router_rtp_capabilitite)).second)
		{
			ERROR_EX_LOG("insert getRouterRtpCapabilities server call failed id  = %lu", m_id);
		}
	}
	

 /////////////////
	bool croom_client::server_call_router_rtp_capabilitite(const  nlohmann::json & msg)
	{
		
		m_device.Load(msg);
		return true;
	}




	void croom_client::request_create_webrtc_transport()
	{
		nlohmann::json createWebRtcTransport =
		{

			{ "request",true     },
		{ "id",           ++m_id          },
		{ "method" , "createWebRtcTransport"},
		{ "data",
		{
			{"forceTcp", false} ,
		{"producing", true},
		{"consuming", false},
		{"sctpCapabilities", m_device.GetSctpCapabilities().dump()}
		}
		}
		};
		g_websocket_mgr.send(createWebRtcTransport.dump());
		if (!m_server_call.insert(std::make_pair(m_id, &croom_client::server_call_create_webrtc_transport)).second)
		{
			ERROR_EX_LOG("insert createWebRtcTransport server call failed id  = %lu", m_id);
		}
	}

	bool croom_client::server_call_create_webrtc_transport(const  nlohmann::json & msg)
	{
		auto sendTransportId = msg["id"].get<std::string>();
		sendTransport = m_device.CreateSendTransport(this,
			sendTransportId,
			msg["iceParameters"],
			msg["iceCandidates"],
			msg["dtlsParameters"],
			msg["sctpParameters"]);
		return true;
	}

}