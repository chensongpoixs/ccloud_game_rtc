#define MSC_CLASS "Consumer"

#include "Consumer.hpp"
 
 
#include "clog.h"
using json = nlohmann::json;

namespace mediasoupclient
{
	using namespace chen;
	Consumer::Consumer(
	  Consumer::PrivateListener* privateListener,
	  Consumer::Listener* listener,
	  const std::string& id,
	  const std::string& localId,
	  const std::string& producerId,
	  webrtc::RtpReceiverInterface* rtpReceiver,
	  webrtc::MediaStreamTrackInterface* track,
	  const json& rtpParameters,
	  const json& appData)
	  : privateListener(privateListener), listener(listener), id(id), localId(localId),
	    producerId(producerId), rtpReceiver(rtpReceiver), track(track), rtpParameters(rtpParameters),
	    appData(appData)
	{
		 
	}

	const std::string& Consumer::GetId() const
	{
		 

		return this->id;
	}

	const std::string& Consumer::GetLocalId() const
	{
		 

		return this->localId;
	}

	const std::string& Consumer::GetProducerId() const
	{
		 

		return this->producerId;
	}

	bool Consumer::IsClosed() const
	{
	 

		return this->closed;
	}

	const std::string Consumer::GetKind() const
	{
		 

		return this->track->kind();
	}

	webrtc::RtpReceiverInterface* Consumer::GetRtpReceiver() const
	{
		 

		return this->rtpReceiver;
	}

	webrtc::MediaStreamTrackInterface* Consumer::GetTrack() const
	{
	 

		return this->track;
	}

	const json& Consumer::GetRtpParameters() const
	{
		 

		return this->rtpParameters;
	}

	bool Consumer::IsPaused() const
	{
		 

		return !this->track->enabled();
	}

	json& Consumer::GetAppData()
	{
	 

		return this->appData;
	}

	/**
	 * Closes the Consumer.
	 */
	void Consumer::Close()
	{
		 

		if (this->closed)
			return;

		this->closed = true;

		this->privateListener->OnClose(this);
	}

	json Consumer::GetStats() const
	{
		if (this->closed)
			ERROR_EX_LOG("Consumer closed");

		return this->privateListener->OnGetStats(this);
	}

	/**
	 * Pauses sending media.
	 */
	void Consumer::Pause()
	{
		 

		if (this->closed)
		{
			ERROR_EX_LOG("Consumer closed");

			return;
		}

		this->track->set_enabled(false);
	}

	/**
	 * Resumes sending media.
	 */
	void Consumer::Resume()
	{
		 
		if (this->closed)
		{
			ERROR_EX_LOG("Consumer closed");

			return;
		}

		this->track->set_enabled(true);
	}

	/**
	 * Transport was closed.
	 */
	void Consumer::TransportClosed()
	{
		 
		if (this->closed)
			return;

		this->closed = true;

		this->listener->OnTransportClose(this);
	}
} // namespace mediasoupclient
