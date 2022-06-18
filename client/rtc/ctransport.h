#ifndef _C_TRANSPORT_H_
#define _C_TRANSPORT_H_
#include "mediasoupclient.hpp"
#include "cnet_types.h"
#include "json.hpp"
#include <memory>
#include "sdp/RemoteSdp.hpp"
#include "api/create_peerconnection_factory.h"
#include "clog.h"
namespace chen {
	
	class cproducer;
	class cclient;
	class cdataconsumer;
	class ctransport : public webrtc::PeerConnectionObserver /*好玩东西给webrtc封装这个里面   */,
		public webrtc::CreateSessionDescriptionObserver 
	{
		
		
	public:
		ctransport( std::string transport_id, cclient *ptr);
		//ctransport() {}
		//~ctransport() {}
	public:

		bool init( const std::string &transport_id, const nlohmann::json& extendedRtpCapabilities,   const nlohmann::json& iceParameters,
			const nlohmann::json& iceCandidates,
			const nlohmann::json& dtlsParameters,
			const nlohmann::json& sctpParameters);
		
	 

		// connect --> webrtc 
		// bool webrtc_connect_transport_offer(webrtc::MediaStreamTrackInterface* track);
		bool webrtc_connect_transport_setup_connect(const std::string & localDtlsRole);
		// send connect localdes [[[[ --> produce -->
		//bool   webrtc_connect_transport_setup_connect_server_call();

		//bool webrtc_transport_produce(const std::string & producerId);


		// recv --> 
		//bool webrtc_connect_recv_setup_call();


		/*void add_webrtc_consmer_transport();
		/// recv data conumser
		
		bool  webrtc_create_consumer(const std::string & id, const std::string & dataconsumerId, const std::string & label);

		bool webrtc_consumer_wait(const std::string & id, const std::string & dataconsumerId, const std::string & label);
	
		void webrtc_create_all_wait_consumer();*/
	public:
		 const std::string&  get_transportId() const { return m_transport_id; }
		
		
	public:

		//
		// PeerConnectionObserver implementation.
		//

		void OnSignalingChange(
			webrtc::PeerConnectionInterface::SignalingState new_state) override;
		


		// 好家伙  webrtc封装太好 ^_^  接口定义 PeerConnectionObserver
		void OnAddTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
			const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
			streams) override;
		void OnRemoveTrack(
			rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
		void OnDataChannel(
			rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override 
		{
			NORMAL_EX_LOG("OnDataChannel");
		}
		void OnRenegotiationNeeded() override 
		{
			NORMAL_EX_LOG("OnRenegotiationNeeded");
		}
		void OnIceConnectionChange(
			webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
		
		void OnIceGatheringChange(
			webrtc::PeerConnectionInterface::IceGatheringState new_state) override 
		{
			NORMAL_EX_LOG("OnIceGatheringChange");
		}
		void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
		void OnIceConnectionReceivingChange(bool receiving) override 
		{
			NORMAL_EX_LOG("OnIceConnectionReceivingChange");
		}
	public:
		// CreateSessionDescriptionObserver implementation.
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;
	private:
		ctransport(const ctransport&);
		ctransport& operator =(const ctransport&);
	protected:
		rtc::scoped_refptr<webrtc::PeerConnectionInterface> m_peer_connection;
		rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peer_connection_factory;
		std::unique_ptr<rtc::Thread> m_networkThread;
		std::unique_ptr<rtc::Thread> m_signalingThread;
		std::unique_ptr<rtc::Thread> m_workerThread;
		std::unique_ptr<mediasoupclient::Sdp::RemoteSdp>	 m_remote_sdp;
		std::string											 m_transport_id;
		

		// TODO@chensong 20220207 
		nlohmann::json					m_sendingRtpParametersByKind;
		nlohmann::json					m_sendingRemoteRtpParametersByKind;
		

		std::unordered_map<std::string, std::shared_ptr<cproducer>> m_producers;

		cclient		*						m_client_ptr;
		std::string							m_offer;
		uint32								m_streamId;
		std::map<std::string, std::shared_ptr<cdataconsumer>> m_data_consumsers;

		rtc::scoped_refptr<webrtc::DataChannelInterface> webrtcDataChannel;
		//bool								m_send;
		//ERecv_Type								m_recving;
		
		//std::list<cDataConsmer>							m_dataconsmers;
		
	};
}
#endif // _C_TRANSPORT_H_