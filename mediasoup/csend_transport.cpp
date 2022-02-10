#include "csend_transport.h"
#include "clog.h"
#include "sdptransform.hpp"
#include "sdp/Utils.hpp"
#include "ortc.hpp"
#include "csession_description.h"
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
			std::unique_ptr<  DesktopCapture> capturer(  DesktopCapture::Create(60,0));
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




	bool csend_transport::init(const std::string &transport_id, 
		const nlohmann::json& extendedRtpCapabilities, 
		const nlohmann::json& iceParameters,
		const nlohmann::json& iceCandidates,
		const nlohmann::json& dtlsParameters,
		const nlohmann::json& sctpParameters)
	{
		m_send = true;
		if (!ctransport::init(transport_id, extendedRtpCapabilities, iceParameters, iceCandidates, dtlsParameters, sctpParameters))
		{
			ERROR_EX_LOG("send transport init failed !!!");
			return false;
		}

		m_sendingRtpParametersByKind = {
			{ "video", mediasoupclient::ortc::getSendingRtpParameters("video", extendedRtpCapabilities) }
		};
		m_sendingRemoteRtpParametersByKind = {
			{"video", mediasoupclient::ortc::getSendingRemoteRtpParameters("video", extendedRtpCapabilities)}
		};
		m_track = m_peer_connection_factory->CreateVideoTrack(std::to_string(rtc::CreateRandomId()), CCapturerTrackSource::Create());

		return true;
	}


	 bool csend_transport::webrtc_connect_transport_offer(webrtc::MediaStreamTrackInterface* track)
	{
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

	 bool   csend_transport::webrtc_connect_transport_setup_connect_server_call()
	 {


		 json& sendingRtpParameters = m_sendingRtpParametersByKind[m_track->kind()];
		 {
			 //const webrtc::SessionDescriptionInterface* desc = m_peer_connection->local_description();
			 std::string offer = m_offer;

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

}
