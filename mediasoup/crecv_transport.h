/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef _C_RECV_TRANSPORT_H_
#define _C_RECV_TRANSPORT_H_
#include "ctransport.h"

#include <string>

namespace chen {
	enum ERecv_Type
	{
		ERecv_None = 0,
		ERecv_Recving,
		ERecv_Success
	};
	class crecv_transport : public ctransport
	{
	private:
		struct cDataConsmer
		{
			std::string m_id;
			std::string m_lable;
			std::string m_stream;
			std::string m_dataconsumerId;
			cDataConsmer() : m_id(""), m_stream(""), m_lable(""), m_dataconsumerId("")
			{}
		};
	public: 
		crecv_transport(std::string transport_id, cclient *ptr)
			: ctransport(transport_id, ptr) , m_recving(ERecv_None) {}


	public:
		bool init(const std::string &transport_id, const nlohmann::json& extendedRtpCapabilities, const nlohmann::json& iceParameters,
			const nlohmann::json& iceCandidates,
			const nlohmann::json& dtlsParameters,
			const nlohmann::json& sctpParameters);

		void Destroy();
		

	public:
		void close_dataconsumer(const std::string & dataconsumer_id);

	public:
		// recv --> 
		bool webrtc_connect_recv_setup_call();

		void add_webrtc_consmer_transport();
		/// recv data conumser

		bool  webrtc_create_consumer(const std::string & id, const std::string & dataconsumerId, const std::string & label);

		bool webrtc_consumer_wait(const std::string & id, const std::string & dataconsumerId, const std::string & label);

		void webrtc_create_all_wait_consumer();
	public:
		// CreateSessionDescriptionObserver implementation.
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;
	private:
		ERecv_Type								m_recving;
		std::list<cDataConsmer>							m_dataconsmers;
	};
}

#endif // !_C_RECV_TRANSPORT_H_