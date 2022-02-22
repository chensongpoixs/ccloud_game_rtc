#include "ctransport.h"
#include "clog.h"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <rtc_base/ssl_adapter.h>
#include "sdptransform.hpp"
#include "sdp/Utils.hpp"
#include "ortc.hpp"
#include "cproducer.h"
#include <memory>
#include "cclient.h"
 
#include "cdataconsumer.h"
#include "csession_description.h"
#include "pc/video_track_source.h"
namespace syz {

	

	
	ctransport::ctransport( std::string transport_id, cclient* ptr):m_transport_id ( transport_id), m_client_ptr(ptr) , m_streamId(0)
	{}
	bool ctransport::init( const std::string &transport_id,  const nlohmann::json& extendedRtpCapabilities,  
		const nlohmann::json& iceParameters,
		const nlohmann::json& iceCandidates,
		const nlohmann::json& dtlsParameters,
		const nlohmann::json& sctpParameters)
	{
		
		m_networkThread = rtc::Thread::CreateWithSocketServer() ;
		m_signalingThread = rtc::Thread::Create();
		m_workerThread = rtc::Thread::Create();
		m_transport_id = transport_id;

		m_networkThread->SetName("network_thread", nullptr);
		m_signalingThread->SetName("signaling_thread", nullptr);
		m_workerThread->SetName("worker_thread", nullptr);

		if (!m_workerThread->Start() || !m_signalingThread->Start() || !m_networkThread->Start())
		{
			ERROR_EX_LOG(" webrtc network signalig  worker start failed !!!");
			return false;
		}

		m_peer_connection_factory = webrtc::CreatePeerConnectionFactory(
			m_networkThread.get(),
			m_workerThread.get(),
			m_signalingThread.get(),
			nullptr /*default_adm*/,
			webrtc::CreateBuiltinAudioEncoderFactory(),
			webrtc::CreateBuiltinAudioDecoderFactory(),
			webrtc::CreateBuiltinVideoEncoderFactory(),
			webrtc::CreateBuiltinVideoDecoderFactory(),
			nullptr /*audio_mixer*/,
			nullptr /*audio_processing*/);

		webrtc::PeerConnectionInterface::RTCConfiguration config;
		// Set SDP semantics to Unified Plan.
		config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
		m_peer_connection = m_peer_connection_factory->CreatePeerConnection(config, nullptr, nullptr, this);

		nlohmann::json iceParameters_ = iceParameters;
		 
		mediasoupclient::Sdp::RemoteSdp* removesdp_ptr = new mediasoupclient::Sdp::RemoteSdp(iceParameters_, iceCandidates, dtlsParameters, sctpParameters);
		m_remote_sdp.reset( removesdp_ptr );
		m_transport_id = transport_id;
		
		
		return true;
	}
	
	
	bool ctransport::webrtc_connect_transport_setup_connect(const std::string & localDtlsRole)
	{
		//NORMAL_EX_LOG("localDtlsRole = %s", localDtlsRole.c_str());
		//const webrtc::SessionDescriptionInterface* desc = m_peer_connection ->local_description();
		std::string sdp = m_offer;

		//desc->ToString(&sdp);


		nlohmann::json  localSdpObject = sdptransform::parse(sdp);
		// Get our local DTLS parameters.
		nlohmann::json dtlsParameters = mediasoupclient::Sdp::Utils::extractDtlsParameters(localSdpObject);

		// Set our DTLS role.
		dtlsParameters["role"] = localDtlsRole;
		//dtlsParameters["transportId"] = m_transport_id;
		std::string remoteDtlsRole = localDtlsRole == "client" ? "server" : "client";
		 m_remote_sdp->UpdateDtlsRole(remoteDtlsRole);

		// NORMAL_EX_LOG("dtlsParameters = %s", dtlsParameters.dump().c_str());
		// send websocket connectWebRTCTransport --->>>
		 m_client_ptr->_send_connect_webrtc_transport(dtlsParameters);

		return true;
	}


	

	
	
	
	//
	// PeerConnectionObserver implementation.
	//

	 

	// 好家伙  webrtc封装太好 ^_^  接口定义 PeerConnectionObserver
	void ctransport::OnAddTrack(
		rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
		const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>&
		streams)
	{
		NORMAL_EX_LOG("OnAddTrack");
	}
	void ctransport::OnRemoveTrack(
		rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
	{
		NORMAL_EX_LOG("OnRemoveTrack");
	}
	 
	 
	void ctransport::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
	{
		NORMAL_EX_LOG("OnIceCandidate");
	}
	 
 
	// CreateSessionDescriptionObserver implementation.
	void ctransport::OnSuccess(webrtc::SessionDescriptionInterface* desc)
	{
		std::string sdp;

		desc->ToString(&sdp);
		m_offer = sdp;
		//nlohmann::json localsdpobject =  sdptransform::parse(sdp);
		//m_client_ptr->transportofferasner(m_send, true);
	}
	void ctransport::OnFailure(webrtc::RTCError error)
	{
		//m_client_ptr->transportofferasner(m_send, false);
	}
}