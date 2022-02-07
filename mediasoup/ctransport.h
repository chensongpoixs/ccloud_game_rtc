#ifndef _C_TRANSPORT_H_
#define _C_TRANSPORT_H_
#include "mediasoupclient.hpp"
#include "cnet_types.h"
#include "json.hpp"
#include <memory>

namespace chen {
	class cproducer;
	class cclient;
	class ctransport : public webrtc::PeerConnectionObserver /*好玩东西给webrtc封装这个里面   */,
		public webrtc::CreateSessionDescriptionObserver
	{
	public:
		ctransport( std::string transport_id, cclient *ptr);
		//ctransport() {}
		//~ctransport() {}
	public:

		bool init(const std::string &transport_id, const nlohmann::json& extendedRtpCapabilities,   const nlohmann::json& iceParameters,
			const nlohmann::json& iceCandidates,
			const nlohmann::json& dtlsParameters,
			const nlohmann::json& sctpParameters);
		


		// connect --> webrtc 
		bool webrtc_connect_transport_offer(webrtc::MediaStreamTrackInterface* track);
		bool webrtc_connect_transport_setup_connect(const std::string & localDtlsRole);

		bool   webrtc_connect_transport_setup_connect_server_call();

		bool webrtc_transport_produce(const std::string & producerId);



	public:
		const std::string & get_transportId() const { return m_transport_id; }
		const std::string& get_kind() const { return m_track->kind(); }
		nlohmann::json get_sending_rtpParameters() const { return m_sendingRtpParametersByKind[m_track->kind()]; }
	public:

		//
		// PeerConnectionObserver implementation.
		//

		void OnSignalingChange(
			webrtc::PeerConnectionInterface::SignalingState new_state) override {}


		// 好家伙  webrtc封装太好 ^_^  接口定义 PeerConnectionObserver
		void OnAddTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
			const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
			streams) override;
		void OnRemoveTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
		void OnDataChannel(
			rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
		void OnRenegotiationNeeded() override {}
		void OnIceConnectionChange(
			webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
		void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
		void OnIceConnectionReceivingChange(bool receiving) override {}
	public:
		// CreateSessionDescriptionObserver implementation.
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;
	private:
		ctransport(const ctransport&);
		ctransport& operator =(const ctransport&);
	private:
		rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peer_connection;
		rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peer_connection_factory;
		std::unique_ptr<rtc::Thread> m_networkThread;
		std::unique_ptr<rtc::Thread> m_signalingThread;
		std::unique_ptr<rtc::Thread> m_workerThread;
		std::unique_ptr<mediasoupclient::Sdp::RemoteSdp>	 m_remote_sdp;
		std::string											 m_transport_id;
		webrtc::MediaStreamTrackInterface* m_track;
		webrtc::RtpTransceiverInterface* m_transceiver;

		// TODO@chensong 20220207 
		nlohmann::json					m_sendingRtpParametersByKind;
		nlohmann::json					m_sendingRemoteRtpParametersByKind;
		

		std::unordered_map<std::string, std::shared_ptr<cproducer>> m_producers;
		cclient		*						m_client_ptr;
	};
}
#endif // _C_TRANSPORT_H_