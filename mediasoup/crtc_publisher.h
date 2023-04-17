/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		api_rtc_publish


************************************************************************************************/

#ifndef _C_RTC_PUBLLISHER_H_
#define _C_RTC_PUBLLISHER_H_
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include <string>
#include "cnet_types.h"
namespace chen {
	//class cclient;
	class ProxyVideoTrackSource;
	class crtc_publisher : public webrtc::PeerConnectionObserver  , 
		public webrtc::CreateSessionDescriptionObserver
	{
	public:
		class clistener
		{
		public:
			virtual ~clistener() = default;
		public:
			virtual void send_create_offer_sdp(const std::string & sdp, bool create = true) = 0;
			
		protected:
		private:
		};
	public:
		//crtc_publisher(/*int32 w*/){}

		crtc_publisher(crtc_publisher::clistener * callback);

	public:

		bool create_offer();


		bool InitializePeerConnection();

		bool CreatePeerConnection(bool dtls);


		void set_remoter_description(std::string  sdp);

		void  rtc_texture(void * texture, uint32_t fmt, int width, int height);
		void onframe(const webrtc::VideoFrame & frame);
	protected:

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


	protected:

		// This callback transfers the ownership of the |desc|.
		  // TODO(deadbeef): Make this take an std::unique_ptr<> to avoid confusion
		  // around ownership.
		virtual void OnSuccess(webrtc::SessionDescriptionInterface* desc)  ;
		// The OnFailure callback takes an RTCError, which consists of an
		// error code and a string.
		// RTCError is non-copyable, so it must be passed using std::move.
		// Earlier versions of the API used a string argument. This version
		// is deprecated; in order to let clients remove the old version, it has a
		// default implementation. If both versions are unimplemented, the
		// result will be a runtime error (stack overflow). This is intentional.
		virtual void OnFailure(webrtc::RTCError error);
		virtual void OnFailure(const std::string& error);



	private:
		void  _add_tracks();
	protected:
		//~crtc_publisher() {}
	private:
		// Signaling and worker threads.
		crtc_publisher::clistener *			m_callback_ptr;
		std::unique_ptr<rtc::Thread> networkThread;
		std::unique_ptr<rtc::Thread> signalingThread;
		std::unique_ptr<rtc::Thread> workerThread;

		rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
		rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;
		 

		rtc::scoped_refptr<ProxyVideoTrackSource>   m_video_track_source_ptr{nullptr};
	};
}


#endif // _C_RTC_PUBLLISHER_H_