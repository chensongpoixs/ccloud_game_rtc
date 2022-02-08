#define MSC_CLASS "Producer"

#include "Producer.hpp"
 
 
#include "clog.h"
using json = nlohmann::json;

namespace mediasoupclient
{
	Producer::Producer(
	  Producer::PrivateListener* privateListener,
	  Producer::Listener* listener,
	  const std::string& id,
	  const std::string& localId,
	  webrtc::RtpSenderInterface* rtpSender,
	  webrtc::MediaStreamTrackInterface* track,
	  const json& rtpParameters,
	  const json& appData)
	  : privateListener(privateListener), listener(listener), id(id), localId(localId),
	    rtpSender(rtpSender), track(track), rtpParameters(rtpParameters), appData(appData)
	{
		 
	}

	const std::string& Producer::GetId() const
	{
		 

		return this->id;
	}

	const std::string& Producer::GetLocalId() const
	{
		 

		return this->localId;
	}

	bool Producer::IsClosed() const
	{
		 

		return this->closed;
	}

	std::string Producer::GetKind() const
	{
		 

		return this->track->kind();
	}

	webrtc::RtpSenderInterface* Producer::GetRtpSender() const
	{
		 

		return this->rtpSender;
	}

	webrtc::MediaStreamTrackInterface* Producer::GetTrack() const
	{
		 

		return this->track;
	}

	const json& Producer::GetRtpParameters() const
	{
		 

		return this->rtpParameters;
	}

	bool Producer::IsPaused() const
	{
		 

		return !this->track->enabled();
	}

	uint8_t Producer::GetMaxSpatialLayer() const
	{
		 

		return this->maxSpatialLayer;
	}

	json& Producer::GetAppData()
	{
		 

		return this->appData;
	}

	/**
	 * Closes the Producer.
	 */
	void Producer::Close()
	{
		 

		if (this->closed)
			return;

		this->closed = true;

		this->privateListener->OnClose(this);
	}

	json Producer::GetStats() const
	{
		using namespace chen;
		if (this->closed)
			ERROR_EX_LOG("Producer closed");

		return this->privateListener->OnGetStats(this);
	}

	/**
	 * Pauses sending media.
	 */
	void Producer::Pause()
	{
		using namespace chen;
		if (this->closed)
		{
			ERROR_EX_LOG("Producer closed");

			return;
		}

		this->track->set_enabled(false);
	}

	/**
	 * Resumes sending media.
	 */
	void Producer::Resume()
	{
		using namespace chen;
		if (this->closed)
		{
			ERROR_EX_LOG("Producer closed");

			return;
		}

		this->track->set_enabled(true);
	}

	/**
	 * Replaces the current track with a new one.
	 */
	void Producer::ReplaceTrack(webrtc::MediaStreamTrackInterface* track)
	{
		using namespace chen;

		if (this->closed)
			ERROR_EX_LOG("Producer closed");
		else if (track == nullptr)
			ERROR_EX_LOG("missing track");
		else if (track->state() == webrtc::MediaStreamTrackInterface::TrackState::kEnded)
			ERROR_EX_LOG("track ended");

		// Do nothing if this is the same track as the current handled one.
		if (track == this->track)
		{
			NORMAL_EX_LOG("same track, ignored");

			return;
		}

		// May throw.
		this->privateListener->OnReplaceTrack(this, track);

		// Keep current paused state.
		auto paused = IsPaused();

		// Set the new track.
		this->track = track;

		// If this Producer was paused/resumed and the state of the new
		// track does not match, fix it.
		if (!paused)
			this->track->set_enabled(true);
		else
			this->track->set_enabled(false);
	}

	/**
	 * Sets the max spatial layer to be sent.
	 */
	void Producer::SetMaxSpatialLayer(const uint8_t spatialLayer)
	{
		 
		using namespace chen;
		if (this->closed)
			ERROR_EX_LOG("Producer closed");
		else if (this->track->kind() != "video")
			ERROR_EX_LOG("not a video Producer");

		if (spatialLayer == this->maxSpatialLayer)
			return;

		// May throw.
		this->privateListener->OnSetMaxSpatialLayer(this, spatialLayer);

		this->maxSpatialLayer = spatialLayer;
	}

	/**
	 * Transport was closed.
	 */
	void Producer::TransportClosed()
	{
	 
		if (this->closed)
			return;

		this->closed = true;

		this->listener->OnTransportClose(this);
	}
} // namespace mediasoupclient
