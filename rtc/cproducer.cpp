#include "cproducer.h"
#include "ctransport.h"
#include <string>

namespace chen {


	cproducer::cproducer()
	{}
	cproducer::~cproducer()
	{}


	cproducer::cproducer(const std::string & id, const std::string & localId, webrtc::RtpSenderInterface* rtpSender,
		webrtc::MediaStreamTrackInterface* track,
		const nlohmann::json& rtpParameters) : id(id), localId(localId),
		rtpSender(rtpSender), track(track), rtpParameters(rtpParameters)
	{
	}
	void cproducer::Destroy()
	{
	}
	const std::string& cproducer::GetId() const
	{
		return id;
	}
	const std::string& cproducer::GetLocalId() const
	{
		return localId;
	}

	std::string cproducer::GetKind() const
	{
		return track->kind();
	}

	webrtc::RtpSenderInterface* cproducer::GetRtpSender() const
	{ 
		return  rtpSender;
	}

	webrtc::MediaStreamTrackInterface* cproducer::GetTrack() const
	{ 
		return  track;
	}

	const nlohmann::json& cproducer::GetRtpParameters() const
	{ 
		return  rtpParameters;
	}

	bool cproducer::IsPaused() const
	{ 
		return !track->enabled();
	}

	uint8_t cproducer::GetMaxSpatialLayer() const
	{ 
		return  maxSpatialLayer;
	}
	void cproducer::Pause()
	{
		if (!track)
		{
			WARNING_EX_LOG("not find video track pause failed !!!");
			return;
		}
		if (!track->enabled())
		{
			return;
		}
		track->set_enabled(false);
	}

	void cproducer::Resume()
	{
		if (!track)
		{
			WARNING_EX_LOG("not find video track Resume failed !!!");
			return;
		}
		if (track->enabled())
		{
			return;
		}
		track->set_enabled(true);
	}
}