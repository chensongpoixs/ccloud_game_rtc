/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#include "crtc_publisher.h"

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "api/audio/audio_mixer.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_options.h"
#include "api/create_peerconnection_factory.h"
#include "api/rtp_sender_interface.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
 
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "p2p/base/port_allocator.h"
#include "pc/video_track_source.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/rtc_certificate_generator.h"
#include "cclient.h"
//#include "ccapturer_track_source.h"
#include "ccapturer_tracksource.h"
#include "external_video_encoder_factory.h"

#include "api/rtp_transceiver_interface.h"
#include "media/base/rid_description.h"
#include "cinput_device.h"
namespace chen {

	class DummySetSessionDescriptionObserver
		: public webrtc::SetSessionDescriptionObserver {
	public:
		static DummySetSessionDescriptionObserver* Create() {
			return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
		}
		virtual void OnSuccess() { RTC_LOG(INFO) << __FUNCTION__; }
		virtual void OnFailure(webrtc::RTCError error) {
			RTC_LOG(INFO) << __FUNCTION__ << " " << ToString(error.type()) << ": "
				<< error.message();
		}
	};
	//const char kAudioLabel[] = "";
	
	const char kAudioLabel[] = "audio_label";
	const char kVideoLabel[] = "video_label";
	const char kStreamId[] = "stream_id";

	crtc_publisher::crtc_publisher(crtc_publisher::clistener * callback)
		: m_callback_ptr(callback)
	{
	}

	bool crtc_publisher::create_offer()
	{

		if (!InitializePeerConnection())
		{
			WARNING_EX_LOG("[%s][%d] init peer connect failed !!!\n", __FUNCTION__, __LINE__);
			return false;
		}
		webrtc::PeerConnectionInterface::RTCOfferAnswerOptions options;
		
		peer_connection_->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
		return true;
	}

	bool crtc_publisher::InitializePeerConnection()
	{
		  networkThread = rtc::Thread::CreateWithSocketServer();
		   signalingThread = rtc::Thread::Create();
		   workerThread = rtc::Thread::Create();
		 
			  networkThread->SetName("network_thread", nullptr);
		  signalingThread->SetName("signaling_thread", nullptr);
		  workerThread->SetName("worker_thread", nullptr);

		 if ( ! networkThread->Start()  ||  ! signalingThread->Start()   || ! workerThread->Start() )
		{
			 
		}
		//std::unique_ptr<rtc::Thread>   work_thread = rtc::Thread::Create();
		peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
			networkThread.get() /* network_thread */,  workerThread.get() /* worker_thread */,
			signalingThread.get() /* signaling_thread */,
			nullptr /* default_adm */,
			webrtc::CreateBuiltinAudioEncoderFactory(),
			webrtc::CreateBuiltinAudioDecoderFactory(),
			CreateBuiltinExternalVideoEncoderFactory(),
			//webrtc::CreateBuiltinVideoEncoderFactory(),
			webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
			nullptr /* audio_processing */);

		if (!peer_connection_factory_)
		{
			/*main_wnd_->MessageBox("Error", "Failed to initialize PeerConnectionFactory",
				true);
			DeletePeerConnection();*/
			return false;
		}
		
		// 设置SDP  ->马流是否加密哈DTLS
		if (!CreatePeerConnection(/*dtls=*/true))
		{
			/*main_wnd_->MessageBox("Error", "CreatePeerConnection failed", true);
			DeletePeerConnection();*/
		}

		//AddTracks();
		_add_tracks();

		return peer_connection_ != nullptr;
		//return true;
	}
	bool crtc_publisher::CreatePeerConnection(bool dtls)
	{
		webrtc::PeerConnectionInterface::RTCConfiguration config;
		config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan; //这个 
		config.enable_dtls_srtp = dtls; //是否加密
		//webrtc::PeerConnectionInterface::IceServer server;
		//server.uri = GetPeerConnectionString();
		//config.servers.push_back(server);
		printf("[%s][%d] config.sdp_semantics = %d\n", __FUNCTION__, __LINE__, config.sdp_semantics);

		peer_connection_ = peer_connection_factory_->CreatePeerConnection(config, nullptr, nullptr, this);
		printf("[%s][%d] fff  config.sdp_semantics = %d\n", __FUNCTION__, __LINE__, config.sdp_semantics);

		return peer_connection_ != nullptr;
	}
	void crtc_publisher::set_remoter_description(std::string sdp)
	{
		// Replace message type from "offer" to "answer"
		std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
			webrtc::CreateSessionDescription(webrtc::SdpType::kAnswer, sdp);
		peer_connection_->SetRemoteDescription(
			DummySetSessionDescriptionObserver::Create(),
			session_description.release());
	}
	void crtc_publisher::rtc_texture(void * texture, uint32_t fmt, int width, int height)
	{
		if (m_video_track_source_ptr)
		{
			s_input_device.set_point(width, height);
			m_video_track_source_ptr->OnFrameTexture(texture, fmt, width, height);
		}
	}
	void crtc_publisher::onframe(const webrtc::VideoFrame & frame)
	{
		if (m_video_track_source_ptr)
		{
			m_video_track_source_ptr->OnFrame(frame);
		}
	}
	void crtc_publisher::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams)
	{
		     


	}
	void crtc_publisher::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
	{
	}
	void crtc_publisher::OnIceCandidate(const webrtc::IceCandidateInterface * candidate)
	{
	}
	void crtc_publisher::OnSuccess(webrtc::SessionDescriptionInterface * desc)
	{
		// 得到本地视频基本信息 先设置本地 SDP 鸭
		peer_connection_->SetLocalDescription(DummySetSessionDescriptionObserver::Create(), desc);

		std::string sdp;
		desc->ToString(&sdp);
		NORMAL_EX_LOG("[%s][%d][][sdp = %s] ", __FUNCTION__, __LINE__, sdp.c_str());
		if (m_callback_ptr)
		{
			m_callback_ptr->send_create_offer_sdp(sdp);
		}
	}
	void crtc_publisher::OnFailure(webrtc::RTCError error)
	{

	}
	void crtc_publisher::OnFailure(const std::string & error)
	{

	}

	void crtc_publisher::_add_tracks()
	{
		if (!peer_connection_->GetSenders().empty())
		{
			return;  // Already added tracks.
		}
		webrtc::DataChannelInit dataChannelInit;
		dataChannelInit.ordered = true;
		dataChannelInit.protocol = "UDP";
		dataChannelInit.negotiated = false;
		dataChannelInit.id = 0;
		rtc::scoped_refptr<webrtc::DataChannelInterface> webrtcDataChannel  = peer_connection_->CreateDataChannel("rtc", &dataChannelInit);
		if (!webrtcDataChannel.get())
		{
			WARNING_EX_LOG("create data channel failed !!!");
			 
		}
		m_data_channel_ptr = new rtc::RefCountedObject<cdata_channel>(webrtcDataChannel);
		///////////////////////////////////////////////AUDIO///////////////////////////////////////////////////////////
		rtc::scoped_refptr<webrtc::AudioSourceInterface> audio_source_ptr = peer_connection_factory_->CreateAudioSource(cricket::AudioOptions());

		rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track_ptr = peer_connection_factory_->CreateAudioTrack(kAudioLabel, audio_source_ptr);

		auto result_or_error = peer_connection_->AddTrack(audio_track_ptr, { kStreamId });
		if (!result_or_error.ok())
		{
			RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection: "
				<< result_or_error.error().message();
		}
		//////////////////////////////////////////VIDEO////////////////////////////////////////////////////////////////
		/*rtc::scoped_refptr<ProxyVideoTrackSource> video_device*/m_video_track_source_ptr = ProxyVideoTrackSource::Create();
		if (m_video_track_source_ptr)
		{



			/*
			// For video
webrtc::PeerConnectionInterface::RTCConfiguration configuration;
peer_connection_ = peer_connection_factory_->CreatePeerConnection(configuration, nullptr, nullptr, this);
webrtc::RtpEncodingParameters encoding_params;
encoding_params.rid = "video";
webrtc::RtpTransceiverInit video_transceiver_init;
video_transceiver_init.direction = webrtc::RtpTransceiverDirection::kSendRecv;
video_transceiver_init.stream_ids.push_back("video_stream");
video_transceiver_init.send_encodings.push_back(encoding_params);
auto video_transceiver = peer_connection_->AddTransceiver(webrtc::VideoTrackInterface::Create(), offer_options.AddTransceiver(video_transceiver_init));
			*/

			rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_proxy_ptr = peer_connection_factory_->CreateVideoTrack(kVideoLabel, m_video_track_source_ptr);
			
			//video_track_proxy_ptr->
			//main_wnd_->StartLocalRenderer(video_track_proxy_ptr);
			/*webrtc::RtpEncodingParameters encoding_params;
			encoding_params.rid = "video";
			webrtc::RtpTransceiverInit video_transceiver_init;
			video_transceiver_init.direction = webrtc::RtpTransceiverDirection::kSendRecv;
			video_transceiver_init.stream_ids.push_back("video_stream");
			video_transceiver_init.send_encodings.push_back(encoding_params);*/

			 webrtc::RtpTransceiverInit transceiver_init;
			// transceiver_init
			// transceiver_init.send_encodings.emplace_back();
			// transceiver_init.send_encodings[0].fec.emplace_back();
			 
			//transceiver_init.direction = webrtc::RtpTransceiverDirection::kSendOnly;
			//transceiver_init.stream_ids.push_back(kStreamId);
			//transceiver_init.send_encodings.emplace_back();
			//webrtc::RtpEncodingParameters prams;
			//transceiver_init.send_encodings[0].rid = "rid0";
			/*	transceiver_init.send_encodings.emplace_back();
				transceiver_init.send_encodings[1].rid = "rid1";*/
			/*peer_connection_->AddTransceiver(webrtc::VideoTrackInterface::Create(), transceiver_init);
			result_or_error = peer_connection_->AddTrack(video_track_proxy_ptr, { kStreamId });*/
			//webrtc::RtpTransceiverInit transceiverInit;
			/*
			* Define a stream id so the generated local description is correct.
			* - with a stream id:    "a=ssrc:<ssrc-id> mslabel:<value>"
			* - without a stream id: "a=ssrc:<ssrc-id> mslabel:"
			*
			* The second is incorrect (https://tools.ietf.org/html/rfc5576#section-4.1)
			*/
		//	transceiverInit.stream_ids.emplace_back("0");


			webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::RtpTransceiverInterface>> result = peer_connection_->AddTransceiver(video_track_proxy_ptr, transceiver_init);

			if (!result_or_error.ok())
			{
				RTC_LOG(LS_ERROR) << "Failed to add video track to PeerConnection: "
					<< result_or_error.error().message();
			}

		}
		else {
			RTC_LOG(LS_ERROR) << "OpenVideoCaptureDevice failed";
		}
	}

}