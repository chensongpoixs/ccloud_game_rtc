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
namespace chen {

	class cSetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
	{
	public:
		cSetSessionDescriptionObserver() = default;
		~cSetSessionDescriptionObserver() override = default;

		std::future<void> GetFuture()
		{
			return  promise.get_future();
		}
		void Reject(const std::string& error)
		{
			 promise.set_exception(std::make_exception_ptr(std::runtime_error(error.c_str())));
		}

		/* Virtual methods inherited from webrtc::SetSessionDescriptionObserver. */
	public:
		void OnSuccess() override
		{
			 promise.set_value();
		}
		void OnFailure(webrtc::RTCError error) override
		{
			auto message = std::string(error.message());

			 Reject(message);
		}

	private:
		std::promise<void> promise;
	};
	ctransport::ctransport( std::string transport_id, cclient* ptr):m_transport_id ( transport_id), m_client_ptr(ptr)
	{}
	bool ctransport::init(const std::string &transport_id,  const nlohmann::json& extendedRtpCapabilities,  const nlohmann::json& iceParameters,
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



		m_remote_sdp.reset( new mediasoupclient::Sdp::RemoteSdp(iceParameters, iceCandidates, dtlsParameters, sctpParameters));
		m_transport_id = transport_id;
		
		m_sendingRtpParametersByKind = {
			{ "video", mediasoupclient::ortc::getSendingRtpParameters("video", extendedRtpCapabilities) }
		};
		m_sendingRemoteRtpParametersByKind ={
			{"video", mediasoupclient::ortc::getSendingRemoteRtpParameters("video", extendedRtpCapabilities)}
		};
		return true;
	}

	bool ctransport::webrtc_connect_transport_offer(webrtc::MediaStreamTrackInterface* track)
	{
		if (!track)
		{
			ERROR_EX_LOG("webrtc connect transport failed !!! track = nullptr" );
			return false;
		}
		if (track->state() == webrtc::MediaStreamTrackInterface::TrackState::kEnded)
		{
			ERROR_EX_LOG("webrtc connect transport failed !!! track state = %u", track->state());
			return false;
		}
		
		webrtc::RtpTransceiverInit transceiverInit;
		/*
		* Define a stream id so the generated local description is correct.
		* - with a stream id:    "a=ssrc:<ssrc-id> mslabel:<value>"
		* - without a stream id: "a=ssrc:<ssrc-id> mslabel:"
		*
		* The second is incorrect (https://tools.ietf.org/html/rfc5576#section-4.1)
		*/
		transceiverInit.stream_ids.emplace_back("0");


		webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>> result = m_peer_connection->AddTransceiver(track, transceiverInit);

		if (!result.ok())
		{
			ERROR_EX_LOG("webrtc connect transport  add transceiver fialed  !!!  " );
			return false;
		}
		m_transceiver = result.value();
		m_transceiver->SetDirection(webrtc::RtpTransceiverDirection::kSendRecv);

		webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;

		m_peer_connection->CreateOffer(this, options);


		return true;
	}
 
	bool ctransport::webrtc_connect_transport_setup_connect(const std::string & localDtlsRole)
	{
		const webrtc::SessionDescriptionInterface* desc = m_peer_connection ->local_description();
		std::string sdp;

		desc->ToString(&sdp);


		nlohmann::json  localSdpObject = sdptransform::parse(sdp);
		// Get our local DTLS parameters.
		nlohmann::json dtlsParameters = mediasoupclient::Sdp::Utils::extractDtlsParameters(localSdpObject);

		// Set our DTLS role.
		dtlsParameters["role"] = localDtlsRole;
		std::string remoteDtlsRole = localDtlsRole == "client" ? "server" : "client";
		 m_remote_sdp->UpdateDtlsRole(remoteDtlsRole);


		// send websocket connectWebRTCTransport --->>>
		 m_client_ptr->_send_connect_webrtc_transport(dtlsParameters);

		return true;
	}


	bool   ctransport::webrtc_connect_transport_setup_connect_server_call()
	{


		json& sendingRtpParameters = m_sendingRtpParametersByKind[m_track->kind()];
		{
			const webrtc::SessionDescriptionInterface* desc = m_peer_connection->local_description();
			std::string offer;

			desc->ToString(&offer);


			//m_peer_connection->SetLocalDescription(PeerConnection::SdpType::OFFER, offer);

			webrtc::SdpParseError error;
			webrtc::SessionDescriptionInterface* sessionDescription;
			rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
				new rtc::RefCountedObject<cSetSessionDescriptionObserver>());
			auto future = observer->GetFuture();
			std::string typeStr = "offer";
			sessionDescription = webrtc::CreateSessionDescription(typeStr, offer, &error);
			if (!sessionDescription)
			{
				ERROR_EX_LOG("webrtc connect transport setup connect server create offer  session description failed !!!");
				observer->Reject(error.description);
				future.get();
				return false;
			}
			m_peer_connection->SetLocalDescription(observer, sessionDescription);
			future.get();
		}


		{
			 
			sendingRtpParameters["mid"] = m_transceiver->mid().value();
		
			const webrtc::SessionDescriptionInterface* desc = m_peer_connection->local_description();
			std::string offer;

			desc->ToString(&offer);
			const mediasoupclient::Sdp::RemoteSdp::MediaSectionIdx mediaSectionIdx = m_remote_sdp->GetNextMediaSectionIdx();
			nlohmann::json localSdpObject = sdptransform::parse(offer);



			nlohmann::json& offerMediaObject = localSdpObject["media"][mediaSectionIdx.idx];


			// Set RTCP CNAME.
			sendingRtpParameters["rtcp"]["cname"] = mediasoupclient::Sdp::Utils::getCname(offerMediaObject);

			// Set RTP encodings by parsing the SDP offer if no encodings are given.
			
			sendingRtpParameters["encodings"] = mediasoupclient::Sdp::Utils::getRtpEncodings(offerMediaObject);
			
			// If VP8 and there is effective simulcast, add scalabilityMode to each encoding.
			std::string  mimeType = sendingRtpParameters["codecs"][0]["mimeType"].get<std::string>();

			std::transform(mimeType.begin(), mimeType.end(), mimeType.begin(), ::tolower);

			// clang-format off
			if (
				sendingRtpParameters["encodings"].size() > 1 &&
				(mimeType == "video/vp8" || mimeType == "video/h264")
				)
				// clang-format on
			{
				for (auto& encoding : sendingRtpParameters["encodings"])
				{
					encoding["scalabilityMode"] = "S1T3";
				}
			}
			m_remote_sdp ->Send(
				offerMediaObject,
				mediaSectionIdx.reuseMid,
				sendingRtpParameters,
				 m_sendingRemoteRtpParametersByKind[m_track->kind()],
				nullptr);

			std::string answer = m_remote_sdp->GetSdp();


			{
				webrtc::SdpParseError error;
				webrtc::SessionDescriptionInterface* sessionDescription;
				rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
					new rtc::RefCountedObject<cSetSessionDescriptionObserver>());
			
				std::string typeStr = "answer";
				auto future         = observer->GetFuture();

				sessionDescription = webrtc::CreateSessionDescription(typeStr, answer, &error);
				if (sessionDescription == nullptr)
				{
					/*MSC_WARN(
						"webrtc::CreateSessionDescription failed [%s]: %s",
						error.line.c_str(),
						error.description.c_str());*/

					observer->Reject(error.description);
					future.get();
					ERROR_EX_LOG("webrtc connect transport setup connect server create answer session description failed !!!");
					return false;
				}

				m_peer_connection->SetRemoteDescription(observer, sessionDescription);

				  future.get();
				   
			}
			//m_peer_connection->SetRemoteDescription();

		}



		// send to websocket produce --> 



		return true;
	}



	bool ctransport::webrtc_transport_produce(const std::string & producerId)
	{
		//const std::string & id, const std::string & localId, webrtc::RtpSenderInterface* rtpSender,
		/*webrtc::MediaStreamTrackInterface* track,
			const nlohmann::json& rtpParameters*/
		std::shared_ptr<cproducer> producer_ptr = std::make_shared<cproducer>(producerId, m_transceiver->mid().value(), m_transceiver->sender(), m_track, m_sendingRtpParametersByKind[m_track->kind()]);
		if (!producer_ptr)
		{
			ERROR_EX_LOG("alloc producer failed !!!");
			return false;
		}

		

		if (!m_producers.insert(std::make_pair(producer_ptr->GetId(), producer_ptr)).second)
		{
			ERROR_EX_LOG("producer map ->  insert failed producer id = %s", producer_ptr->GetId().c_str());
			return false;
		}
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
	{}
	void ctransport::OnRemoveTrack(
		rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
	{}
	 
	 
	void ctransport::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
	{}
	 
 
	// CreateSessionDescriptionObserver implementation.
	void ctransport::OnSuccess(webrtc::SessionDescriptionInterface* desc)
	{
		std::string sdp;

		desc->ToString(&sdp);

		//nlohmann::json localsdpobject =  sdptransform::parse(sdp);
		m_client_ptr->transportofferasner(true);
	}
	void ctransport::OnFailure(webrtc::RTCError error)
	{
		m_client_ptr->transportofferasner(false);
	}
}