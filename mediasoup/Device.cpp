#define MSC_CLASS "Device"

#include "Device.hpp"
 
 
#include "ortc.hpp"
#include "clog.h"
using json = nlohmann::json;

namespace mediasoupclient
{
	using namespace chen;
	/**
	 * Whether the Device is loaded.
	 */
	bool Device::IsLoaded() const
	{
		 
		return this->loaded;
	}

	void Device::reset()
	{
		if (!loaded)
		{
			RTC_LOG(LS_WARNING) << "device loaded failed !!!";
			return;
		}
		loaded = false;
		extendedRtpCapabilities.clear();
		recvRtpCapabilities.clear();
		sctpCapabilities.clear();
		canProduceByKind =
		{
			{ "audio", false },
		{ "video", false }
		};
		 
	}

	void Device::close()
	{
	}

	/**
	 * RTP capabilities of the Device for receiving media.
	 */
	const json& Device::GetRtpCapabilities() const
	{
		 

		if (!this->loaded)
			ERROR_EX_LOG("not loaded");

		return this->recvRtpCapabilities;
	}

	/**
	 * SCTP capabilities of the Device for receiving media.
	 */
	const json& Device::GetSctpCapabilities() const
	{
		 
		if (!this->loaded)
			ERROR_EX_LOG("not loaded");

		return this->sctpCapabilities;
	}

	/**
	 * Initialize the Device.
	 */
	void Device::Load(json routerRtpCapabilities, const PeerConnection::Options* peerConnectionOptions)
	{
		 
		if (this->loaded)
		{
			ERROR_EX_LOG("already loaded");
		}

		// This may throw.
		ortc::validateRtpCapabilities(routerRtpCapabilities);

		// Get Native RTP capabilities.
		auto nativeRtpCapabilities = Handler::GetNativeRtpCapabilities(peerConnectionOptions);

		NORMAL_EX_LOG("got native RTP capabilities:\n%s", nativeRtpCapabilities.dump(4).c_str());

		// This may throw.
		ortc::validateRtpCapabilities(nativeRtpCapabilities);

		// Get extended RTP capabilities.
		this->extendedRtpCapabilities =
		  ortc::getExtendedRtpCapabilities(nativeRtpCapabilities, routerRtpCapabilities);

		NORMAL_EX_LOG("got extended RTP capabilities:\n%s", this->extendedRtpCapabilities.dump(4).c_str());

		// Check whether we can produce audio/video.
		this->canProduceByKind["audio"] = ortc::canSend("audio", this->extendedRtpCapabilities);
		this->canProduceByKind["video"] = ortc::canSend("video", this->extendedRtpCapabilities);

		// Generate our receiving RTP capabilities for receiving media.
		this->recvRtpCapabilities = ortc::getRecvRtpCapabilities(this->extendedRtpCapabilities);

		NORMAL_EX_LOG("got receiving RTP capabilities:\n%s", this->recvRtpCapabilities.dump(4).c_str());

		// This may throw.
		ortc::validateRtpCapabilities(this->recvRtpCapabilities);

		// Generate our SCTP capabilities.
		this->sctpCapabilities = Handler::GetNativeSctpCapabilities();

		NORMAL_EX_LOG("got receiving SCTP capabilities:\n%s", this->sctpCapabilities.dump(4).c_str());

		// This may throw.
		ortc::validateSctpCapabilities(this->sctpCapabilities);

		NORMAL_EX_LOG("succeeded");

		this->loaded = true;
	}

	/**
	 * Whether we can produce audio/video.
	 *
	 */
	bool Device::CanProduce(const std::string& kind)
	{
	 

		if (!this->loaded)
			ERROR_EX_LOG("not loaded");
		else if (kind != "audio" && kind != "video")
			ERROR_EX_LOG("invalid kind");

		return this->canProduceByKind[kind];
	}

	SendTransport* Device::CreateSendTransport(
	  SendTransport::Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const json& sctpParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json& appData) const
	{
		 
		if (!this->loaded)
			ERROR_EX_LOG("not loaded");
		else if (!appData.is_object())
			ERROR_EX_LOG("appData must be a JSON object");

		// Validate arguments.
		ortc::validateIceParameters(const_cast<json&>(iceParameters));
		ortc::validateIceCandidates(const_cast<json&>(iceCandidates));
		ortc::validateDtlsParameters(const_cast<json&>(dtlsParameters));

		if (!sctpParameters.is_null())
		{
			ortc::validateSctpParameters(const_cast<json&>(sctpParameters));
		}

		// Create a new Transport.
		auto* transport = new SendTransport(
		  listener,
		  id,
		  iceParameters,
		  iceCandidates,
		  dtlsParameters,
		  sctpParameters,
		  peerConnectionOptions,
		  &this->extendedRtpCapabilities,
		  &this->canProduceByKind,
		  appData);

		return transport;
	}

	SendTransport* Device::CreateSendTransport(
	  SendTransport::Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json& appData) const
	{
		 
		return Device::CreateSendTransport(
		  listener, id, iceParameters, iceCandidates, dtlsParameters, nullptr, peerConnectionOptions, appData);
	}

	RecvTransport* Device::CreateRecvTransport(
	  Transport::Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const json& sctpParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json& appData) const
	{
		 
		if (!this->loaded)
			ERROR_EX_LOG("not loaded");
		else if (!appData.is_object())
			ERROR_EX_LOG("appData must be a JSON object");

		// Validate arguments.
		ortc::validateIceParameters(const_cast<json&>(iceParameters));
		ortc::validateIceCandidates(const_cast<json&>(iceCandidates));
		ortc::validateDtlsParameters(const_cast<json&>(dtlsParameters));

		if (!sctpParameters.is_null())
			ortc::validateSctpParameters(const_cast<json&>(sctpParameters));

		// Create a new Transport.
		auto* transport = new RecvTransport(
		  listener,
		  id,
		  iceParameters,
		  iceCandidates,
		  dtlsParameters,
		  sctpParameters,
		  peerConnectionOptions,
		  &this->extendedRtpCapabilities,
		  appData);

		return transport;
	}

	RecvTransport* Device::CreateRecvTransport(
	  Transport::Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json& appData) const
	{
		 
		return Device::CreateRecvTransport(
		  listener, id, iceParameters, iceCandidates, dtlsParameters, nullptr, peerConnectionOptions, appData);
	}
} // namespace mediasoupclient
