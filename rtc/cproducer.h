#ifndef _C_PRODUCER_H_
#define _C_PRODUCER_H_
#include "ctransport.h"

#include <string>

namespace chen {
	class cproducer
	{
	public:
		cproducer();
		~cproducer();


		cproducer(const std::string & id, const std::string & localId, webrtc::RtpSenderInterface* rtpSender,
			webrtc::MediaStreamTrackInterface* track,
			const nlohmann::json& rtpParameters );


		void Destroy();
	public:

		const std::string& GetId() const;
		const std::string& GetLocalId() const;

		std::string  GetKind() const;

		webrtc::RtpSenderInterface*  GetRtpSender() const;

		webrtc::MediaStreamTrackInterface*  GetTrack() const;

		const nlohmann::json&  GetRtpParameters() const;

		bool  IsPaused() const;

		uint8_t  GetMaxSpatialLayer() const;
		/**
		* Pauses sending media.
		*/
		void Pause();

		/**
		* Resumes sending media.
		*/
		void Resume();
	private:
		cproducer(const cproducer&);
		cproducer& operator =(const cproducer&);


	private:
		// Id.
		std::string id;
		// localId.
		std::string localId;
		// Closed flag.
		//bool closed{ false };
		// Associated RTCRtpSender.
		webrtc::RtpSenderInterface* rtpSender{ nullptr };
		// Local track.
		webrtc::MediaStreamTrackInterface* track{ nullptr };
		// RTP parameters.
		nlohmann::json rtpParameters;
		// Paused flag.
	//	bool paused{ false };
		// Video Max spatial layer.
		uint8_t maxSpatialLayer{ 0 };
	};
}


#endif // !_C_PRODUCER_H_