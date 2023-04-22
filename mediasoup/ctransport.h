﻿/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
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