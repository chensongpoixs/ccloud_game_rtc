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
#include "cdesktop_capture.h"
#include "cdataconsumer.h"
#include "csession_description.h"
#include "pc/video_track_source.h"
namespace chen {

	class CCapturerTrackSource : public webrtc::VideoTrackSource {
	public:
		static rtc::scoped_refptr<CCapturerTrackSource> Create() {
			/*const size_t kWidth = 640;
			const size_t kHeight = 480;
			const size_t kFps = 30;
			std::unique_ptr<webrtc::test::VcmCapturer> capturer;
			std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
			webrtc::VideoCaptureFactory::CreateDeviceInfo());
			if (!info) {
			return nullptr;
			}
			int num_devices = info->NumberOfDevices();
			for (int i = 0; i < num_devices; ++i) {
			capturer = absl::WrapUnique(
			webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, i));
			if (capturer) {
			return new
			rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));
			}
			}*/
			std::unique_ptr<  DesktopCapture> capturer(  DesktopCapture::Create(25,0));
			if (capturer) 
			{
				capturer->StartCapture();
				return new
					rtc::RefCountedObject<CCapturerTrackSource>(std::move(capturer));
			}
			return nullptr;
		}

	protected:
		explicit CCapturerTrackSource(
			std::unique_ptr< DesktopCapture> capturer)
			: VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

	private:
		rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
			return capturer_.get();
		}
		//std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
		std::unique_ptr< DesktopCapture> capturer_;
	};



	
	ctransport::ctransport( std::string transport_id, cclient* ptr):m_transport_id ( transport_id), m_client_ptr(ptr) , m_streamId(0), m_recving(ERecv_None)
	{}
	bool ctransport::init(bool send, const std::string &transport_id,  const nlohmann::json& extendedRtpCapabilities,  
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
		m_send = send;
		if (send)
		{
			m_sendingRtpParametersByKind = {
				{ "video", mediasoupclient::ortc::getSendingRtpParameters("video", extendedRtpCapabilities) }
			};
			m_sendingRemoteRtpParametersByKind = {
				{"video", mediasoupclient::ortc::getSendingRemoteRtpParameters("video", extendedRtpCapabilities)}
			};
			m_track = m_peer_connection_factory->CreateVideoTrack(std::to_string(rtc::CreateRandomId()), CCapturerTrackSource::Create());
		}
		return true;
	}

	bool ctransport::webrtc_connect_transport_offer(webrtc::MediaStreamTrackInterface* track)
	{
		//track = createVideoTrack(std::to_string(rtc::CreateRandomId()));
		 if (!m_track)
		{
			ERROR_EX_LOG("webrtc connect transport failed !!! track = nullptr" );
			return false;
		}
		if (m_track->state() == webrtc::MediaStreamTrackInterface::TrackState::kEnded)
		{
			ERROR_EX_LOG("webrtc connect transport failed !!! track state = %u", m_track->state());
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


		webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>> result = m_peer_connection->AddTransceiver(m_track, transceiverInit);

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
		NORMAL_EX_LOG("localDtlsRole = %s", localDtlsRole.c_str());
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

		 NORMAL_EX_LOG("dtlsParameters = %s", dtlsParameters.dump().c_str());
		// send websocket connectWebRTCTransport --->>>
		 m_client_ptr->_send_connect_webrtc_transport(dtlsParameters);

		return true;
	}


	bool   ctransport::webrtc_connect_transport_setup_connect_server_call()
	{


		json& sendingRtpParameters = m_sendingRtpParametersByKind[m_track->kind()];
		{
			//const webrtc::SessionDescriptionInterface* desc = m_peer_connection->local_description();
			std::string offer = m_offer;

			//desc->ToString(&offer);


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
				//observer->OnSuccess();
				  future.get();
				   
			}
			//m_peer_connection->SetRemoteDescription();

		}



		// send to websocket produce --> 



		return true;
	}
	bool ctransport::webrtc_connect_recv_setup_call()
	{
		std::string offer = m_offer;

		webrtc::SdpParseError error;
		webrtc::SessionDescriptionInterface* sessionDescription;
		rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
			new rtc::RefCountedObject<cSetSessionDescriptionObserver>());

		std::string  typeStr = "answer";
		auto future = observer->GetFuture();

		sessionDescription = webrtc::CreateSessionDescription(typeStr, offer, &error);
		if (sessionDescription == nullptr)
		{
			/*MSC_WARN(
				"webrtc::CreateSessionDescription failed [%s]: %s",
				error.line.c_str(),
				error.description.c_str());*/

			observer->Reject(error.description);
			future.get();
			return false;
		}

		m_peer_connection->SetLocalDescription(observer, sessionDescription);
		if (m_dataconsmers.empty())
		{
			return true;
		}
		{
			cDataConsmer cdata = m_dataconsmers.front();
			m_dataconsmers.pop_front();
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_cosumsers.insert(std::make_pair(cdata.m_id, std::make_shared<cdataconsumer>()));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");
				return false;
			}
			/* clang-format off */
			nlohmann::json sctpStreamParameters =
			{
				{ "streamId", cdata.m_id               },
			{ "ordered",  false }
			};
			(pi.first->second)->init(cdata.m_id, cdata.m_dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");


		}

		while (!m_dataconsmers.empty())
		{
			cDataConsmer cdata = m_dataconsmers.front();
			m_dataconsmers.pop_front();

			webrtc::DataChannelInit dataChannelInit;
			dataChannelInit.protocol = "chat";
			dataChannelInit.negotiated = true;
			dataChannelInit.id = ++m_streamId;
			webrtcDataChannel = m_peer_connection->CreateDataChannel(cdata.m_lable, &dataChannelInit);
			if (!webrtcDataChannel.get())
			{
				ERROR_EX_LOG("create data channel failed !!!");
				return false;
			}
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_cosumsers.insert(std::make_pair(cdata.m_id, std::make_shared<cdataconsumer>()));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");
				return false;
			}
			/* clang-format off */
			nlohmann::json sctpStreamParameters =
			{
				{ "streamId", dataChannelInit.id                },
			{ "ordered",  dataChannelInit.ordered }
			};
			(pi.first->second)->init(cdata.m_id, cdata.m_dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");
		}
		m_recving = ERecv_Success;
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

	/////////////////////////////////////////////////////////////////

	bool  ctransport::webrtc_create_consumer(const std::string & id, const std::string & dataconsumerId, const std::string & label)
	{
		NORMAL_EX_LOG("id = %s, datacnsumerid = %s, label = %s", id.c_str(), dataconsumerId.c_str(), label.c_str());
		
		if (m_recving == ERecv_Success)
		{
			webrtc::DataChannelInit dataChannelInit;
			dataChannelInit.protocol = "chat";
			dataChannelInit.negotiated = true;
			dataChannelInit.id = ++m_streamId;
			webrtcDataChannel = m_peer_connection->CreateDataChannel(label, &dataChannelInit);
			if (!webrtcDataChannel.get())
			{
				ERROR_EX_LOG("create data channel failed !!!");
				return false;
			}
			std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_cosumsers.insert(std::make_pair(id, std::make_shared<cdataconsumer>()));
			if (!pi.second)
			{
				ERROR_EX_LOG("create data channel insert failed !!!");
				return false;
			}
			/* clang-format off */
			nlohmann::json sctpStreamParameters =
			{
				{ "streamId", dataChannelInit.id                },
			{ "ordered",  dataChannelInit.ordered }
			};
			(pi.first->second)->init(id, dataconsumerId, webrtcDataChannel, sctpStreamParameters, "stcp");
			return true;
		}
		else if (m_recving == ERecv_Recving)
		{
			cDataConsmer Datacs;
			Datacs.m_id = id;
			Datacs.m_lable = label;
			Datacs.m_dataconsumerId = dataconsumerId;
			m_dataconsmers.push_back(Datacs);
			return true;
		}
		cDataConsmer Datacs;
		Datacs.m_id = id;
		Datacs.m_lable = label;
		Datacs.m_dataconsumerId = dataconsumerId;
		m_dataconsmers.push_back(Datacs);
		webrtc::DataChannelInit dataChannelInit;
		dataChannelInit.protocol = "chat";
		dataChannelInit.negotiated = true;
		dataChannelInit.id = ++m_streamId;
		bool setremotesdp = false;
		if (!webrtcDataChannel)
		{
			setremotesdp = true;
		}
		webrtcDataChannel = m_peer_connection->CreateDataChannel(label, &dataChannelInit);
		if (!webrtcDataChannel.get())
		{
			ERROR_EX_LOG("create data channel failed !!!");
			return false;
		}
		
		m_remote_sdp->RecvSctpAssociation();
		std::string sdpoffer = m_remote_sdp->GetSdp();
		{
			webrtc::SdpParseError error;
			webrtc::SessionDescriptionInterface* sessionDescription;
			rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
				new rtc::RefCountedObject<cSetSessionDescriptionObserver>());

			std::string typeStr = "offer";
			auto future = observer->GetFuture();

			sessionDescription = webrtc::CreateSessionDescription(typeStr, sdpoffer, &error);
			if (sessionDescription == nullptr)
			{
				/*MSC_WARN(
				"webrtc::CreateSessionDescription failed [%s]: %s",
				error.line.c_str(),
				error.description.c_str());*/

				observer->Reject(error.description);
				future.get();
				return false;
			}


			m_peer_connection->SetRemoteDescription(observer, sessionDescription);
			future.get();
		}
		
		{
		 
			//chen::CreateSessionDescriptionObserver* sessionDescriptionObserver =
			//	new rtc::RefCountedObject<chen::CreateSessionDescriptionObserver>();
			//rtc::scoped_refptr<cSetSessionDescriptionObserver> observer(
				//new rtc::RefCountedObject<cSetSessionDescriptionObserver>());
			webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
			//auto future = sessionDescriptionObserver->GetFuture();
			m_peer_connection->CreateAnswer(this, options);
			//std::string offer = future.get();
			//m_offer = offer;
			m_recving = ERecv_Recving;
			NORMAL_EX_LOG("recv create answer !!!");
			//nlohmann::json localsdpobject =  sdptransform::parse(sdp);
			//m_client_ptr->transportofferasner(m_send, true);
		}
		//m_peer_connection->SetRemoteDescription()
		
		
		//std::pair<std::map<std::string, std::shared_ptr<cdataconsumer>>::iterator, bool> pi = m_data_cosumsers.insert(std::make_pair(id, std::make_shared<cdataconsumer>()));
		//if (!pi.second)
		//{
		//	ERROR_EX_LOG("create data channel insert failed !!!");
		//	return false;
		//}
		///* clang-format off */
		//nlohmann::json sctpStreamParameters =
		//{
		//	{ "streamId", dataChannelInit.id                },
		//{ "ordered",  dataChannelInit.ordered }
		//};
		//(pi.first->second)->init(id, dataconsumerId, webrtcDataChannel, sctpStreamParameters,  "stcp");
		// m_data_cosumsers.insert(std::make_pair(id, cdataconsumer())
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
		m_offer = sdp;
		//nlohmann::json localsdpobject =  sdptransform::parse(sdp);
		m_client_ptr->transportofferasner(m_send, true);
	}
	void ctransport::OnFailure(webrtc::RTCError error)
	{
		m_client_ptr->transportofferasner(m_send, false);
	}
}