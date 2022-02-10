#ifndef _C_SEND_TRANSPORT_H_
#define _C_SEND_TRANSPORT_H_
#include "ctransport.h"

#include <string>

namespace chen {
	class csend_transport : public ctransport
	{
	public:
		csend_transport(std::string transport_id, cclient *ptr)
			: ctransport(transport_id, ptr) {}

		bool init(const std::string &transport_id, const nlohmann::json& extendedRtpCapabilities, const nlohmann::json& iceParameters,
			const nlohmann::json& iceCandidates,
			const nlohmann::json& dtlsParameters,
			const nlohmann::json& sctpParameters);


		virtual bool webrtc_connect_transport_offer(webrtc::MediaStreamTrackInterface* track);

		bool webrtc_connect_transport_setup_connect_server_call();
	public:
		const std::string& get_kind() const { return m_track->kind(); }
	private:
	};

}

#endif // !_C_SEND_TRANSPORT_H_