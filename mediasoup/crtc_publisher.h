/***********************************************************************************************
created: 		2023-02-13

author:			chensong

purpose:		api_rtc_publish

输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/

#ifndef _C_RTC_PUBLLISHER_H_
#define _C_RTC_PUBLLISHER_H_
#include "api/media_stream_interface.h"
#include "api/peer_connection_interface.h"
#include <string>
#include "cnet_types.h"
#include "cdata_channel.h"
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
		rtc::scoped_refptr < cdata_channel>			m_data_channel_ptr;
	};
}


#endif // _C_RTC_PUBLLISHER_H_