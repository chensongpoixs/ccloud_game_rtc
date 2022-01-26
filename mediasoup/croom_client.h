/***********************************************************************************************
created: 		2022-01-26

author:			chensong

purpose:		roomclient
************************************************************************************************/
#ifndef _C_ROOM_CLIENT_H_
#define _C_ROOM_CLIENT_H_
#include "mediasoupclient.hpp"
#include "cnet_types.h"
#include "json.hpp"
namespace chen {

	class croom_client : public
		mediasoupclient::SendTransport::Listener,
		mediasoupclient::RecvTransport::Listener,
		mediasoupclient::Producer::Listener,
		mediasoupclient::DataProducer::Listener,
		mediasoupclient::DataConsumer::Listener
	{
	public:
		 
		typedef bool(croom_client::*server_msg_call)(const  nlohmann::json & msg);
	public:
		croom_client();
		~croom_client();

	public:
		void request_router_rtp_capabilities();
		void request_create_webrtc_transport();

	public:
		bool server_call_router_rtp_capabilitite(const  nlohmann::json & msg);

		bool server_call_create_webrtc_transport(const  nlohmann::json & msg);
	private:
		croom_client(const croom_client&);
		croom_client& operator =(const croom_client&);
	private:
		uint64					m_id;
		mediasoupclient::Device m_device;
		std::map<uint64, server_msg_call> m_server_call;
		mediasoupclient::SendTransport* sendTransport{ nullptr };
		mediasoupclient::RecvTransport* recvTransport{ nullptr };
		mediasoupclient::DataProducer* dataProducer{ nullptr };
		mediasoupclient::DataConsumer* dataConsumer{ nullptr };
	};

}

#endif // _C_ROOM_CLIENT_H_