#ifndef _C_SEND_TRANSPORT_H_
#define _C_SEND_TRANSPORT_H_
#include "ctransport.h"
#include "ccapturer_tracksource.h"
#include <string>

namespace chen {
	class csend_transport : public ctransport
	{
	public:
		csend_transport(std::string transport_id, cclient *ptr)
			: ctransport(transport_id, ptr) 
			, m_track(nullptr)
			, m_capturer_ptr(nullptr)
			, m_transceiver(nullptr)
		{
			
		}

		bool init(const std::string &transport_id, const nlohmann::json& extendedRtpCapabilities, const nlohmann::json& iceParameters,
			const nlohmann::json& iceCandidates,
			const nlohmann::json& dtlsParameters,
			const nlohmann::json& sctpParameters);

		void Destroy();
	public:
		// 线程不安全的
		bool webrtc_video(unsigned char * rgba, int32_t width, int32_t height);
		bool webrtc_video(const webrtc::VideoFrame& frame);
		void Resume();
		void Pause();
		 bool webrtc_connect_transport_offer( );
		 bool webrtc_transport_produce(const std::string & producerId);
		bool webrtc_connect_transport_setup_connect_server_call();
		// TODO@chensong 20220210 比较神奇的地方 必须要拷贝  原来是栈上空间 是会释放的  这是webrtc线程异步操作的结果 
		std::string get_kind() { return m_track->kind(); }
		const nlohmann::json& get_sending_rtpParameters() const { return m_sendingRtpParametersByKind[m_track->kind()]; }
	public:
		// CreateSessionDescriptionObserver implementation.
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;
	private:
		rtc::scoped_refptr<webrtc::VideoTrackInterface> m_track;
		rtc::scoped_refptr<ProxyVideoTrackSource>		m_capturer_ptr;
		webrtc::RtpTransceiverInterface* m_transceiver;
	};

}

#endif // !_C_SEND_TRANSPORT_H_