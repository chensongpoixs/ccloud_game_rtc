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

#include "cdataconsumer.h"
#include "csession_description.h"
#include "pc/video_track_source.h"
#include "cinput_device.h"
namespace chen {



	




	bool csend_transport::init(const std::string &transport_id, 
		const nlohmann::json& extendedRtpCapabilities, 
		const nlohmann::json& iceParameters,
		const nlohmann::json& iceCandidates,
		const nlohmann::json& dtlsParameters,
		const nlohmann::json& sctpParameters)
	{
		 
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
		m_capturer_ptr = nullptr; // COSGCapturerTrackSource::Create();
		m_track = nullptr; //  m_peer_connection_factory->CreateVideoTrack(std::to_string(rtc::CreateRandomId()), m_capturer_ptr);
		m_transceiver = nullptr;
		return true;
	}
	bool csend_transport::webrtc_video(unsigned char * rgba, uint32 fmt, int32_t width, int32_t height)
	{
		if (m_capturer_ptr)
		{
			s_input_device.set_point(width, height);
			return m_capturer_ptr->OnFrame(rgba, fmt, width, height);
		}
		WARNING_EX_LOG("send transport capturer = nullptr");
		return  false;
	}
	bool csend_transport::webrtc_texture(void * texture, uint32 fmt, int32_t width, int32_t height)
	{
		if (m_capturer_ptr)
		{
			NORMAL_EX_LOG("");
			s_input_device.set_point(width, height);
			return m_capturer_ptr->OnFrameTexture(texture, fmt, width, height);
		}
		WARNING_EX_LOG("send transport capturer = nullptr");
		return  false;
	}
	bool csend_transport::webrtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32 fmt, int32_t width, int32_t height)
	{
		if (m_capturer_ptr)
		{
			s_input_device.set_point(width, height);
			return m_capturer_ptr->OnFrame(y_ptr, fmt, uv_ptr, width, height);
		}
		WARNING_EX_LOG("send transport capturer = nullptr");
		return  false;
	}
	bool csend_transport::webrtc_video(const webrtc::VideoFrame& frame)
	{
		if (m_capturer_ptr)
		{
			s_input_device.set_point(frame.width(), frame.height());
			return m_capturer_ptr->OnFrame(frame);
		}
		WARNING_EX_LOG("send transport capturer = nullptr");
		return  false;
	}
	void csend_transport::Destroy()
	{
		if (m_transceiver)
		{

			if (m_transceiver->sender())
			{
				m_transceiver->sender()->SetTrack(nullptr);
				m_peer_connection->RemoveTrack(m_transceiver->sender());
			}
			
			m_remote_sdp->CloseMediaSection(m_transceiver->mid().value());
			//m_transceiver = nullptr;
		}
		SYSTEM_LOG("m_transceiver ok !!!");
		//if (m_capturer_ptr)
		//{
		//	m_capturer_ptr->stop();
		//	//m_capturer_ptr = nullptr;
		//	SYSTEM_LOG("m_capturer_ptr ok !!!");
		//}
		//
		m_capturer_ptr = nullptr;
		if (m_peer_connection_factory)
		{
			m_peer_connection_factory->StopAecDump();
		}
		m_peer_connection_factory = nullptr;
		m_peer_connection = nullptr;
		SYSTEM_LOG("send m_peer_connection_factory ok !!!");
		/*if (m_networkThread)
		{
			m_networkThread->Stop();
			SYSTEM_LOG("m_networkThread  ok !!!");
		}
		if (m_signalingThread)
		{
			m_signalingThread->Stop();
			SYSTEM_LOG("m_signalingThread  ok !!!");
		}
		if (m_workerThread)
		{
			m_workerThread->Stop();
			SYSTEM_LOG("m_workerThread  ok !!!");
		}*/
	}
	void csend_transport::Resume()
	{
		/*if (!m_track.get())
		{
			return;
		}*/
		if (!m_transceiver || !m_track || m_capturer_ptr)
		{
			return;
		}
		for (std::unordered_map<std::string, std::shared_ptr<cproducer>>::iterator iter = m_producers.begin(); iter != m_producers.end(); ++iter)
		{
			if (iter->second)
			{
				iter->second->Resume();
			}
		}
		 
	}
	void csend_transport::Pause()
	{
		/*if (!m_track.get())
		{
			return;
		}*/
		if (!m_transceiver)
		{
			return;
		}
		for (std::unordered_map<std::string, std::shared_ptr<cproducer>>::iterator iter = m_producers.begin(); iter != m_producers.end(); ++iter)
		{
			if (iter->second)
			{
				iter->second->Pause();
			}
		}
		 
	}
	 bool csend_transport::webrtc_connect_transport_offer( )
	{
		 if (m_capturer_ptr || m_track)
		 {
			 WARNING_EX_LOG("alloc success ok !!!");
			 return false;
		 }
		 m_capturer_ptr = ProxyVideoTrackSource::Create();
		 if (!m_capturer_ptr)
		 {
			 WARNING_EX_LOG("ProxyVideoTrackSource alloc failed  !!! m_capturer_ptr = nullptr");
			 return false;
		 }
		 m_track =   m_peer_connection_factory->CreateVideoTrack(std::to_string(rtc::CreateRandomId()), m_capturer_ptr);
		 if (!m_track)
		 {
			 ERROR_EX_LOG("webrtc connect transport failed !!! track = nullptr");
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


	 bool csend_transport::webrtc_transport_produce(const std::string & producerId)
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
	 // CreateSessionDescriptionObserver implementation.
	 void csend_transport::OnSuccess(webrtc::SessionDescriptionInterface* desc)
	 {
		 std::string sdp;

		 desc->ToString(&sdp);
		 m_offer = sdp;
		 //nlohmann::json localsdpobject =  sdptransform::parse(sdp);
		 m_client_ptr->transportofferasner(true, true);
	 }
	 void csend_transport::OnFailure(webrtc::RTCError error)
	 {
		 m_client_ptr->transportofferasner(true, false);
	 }
}
