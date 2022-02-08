#define MSC_CLASS "Transport"

#include "Transport.hpp"
 
 
#include "ortc.hpp"
#include "clog.h"
using json = nlohmann::json;

namespace mediasoupclient
{
	/* Transport */
	using namespace chen;
	Transport::Transport(
	  Listener* listener, const std::string& id, const json* extendedRtpCapabilities, const json& appData)
	  : extendedRtpCapabilities(extendedRtpCapabilities), listener(listener), id(id), appData(appData)
	{
		 
	}

	const std::string& Transport::GetId() const
	{
		 
		return this->id;
	}

	bool Transport::IsClosed() const
	{
		 

		return this->closed;
	}

	const std::string& Transport::GetConnectionState() const
	{
		 

		return PeerConnection::iceConnectionState2String[this->connectionState];
	}

	json& Transport::GetAppData()
	{
	 

		return this->appData;
	}

	void Transport::Close()
	{
		 

		if (this->closed)
			return;

		this->closed = true;

		// Close the handler.
		this->handler->Close();
	}

	json Transport::GetStats() const
	{
		 
		
		if (this->closed)
			ERROR_EX_LOG("Transport closed");
		else
			return this->handler->GetTransportStats();
	}

	void Transport::RestartIce(const json& iceParameters)
	{
		 
		if (this->closed)
			ERROR_EX_LOG("Transport closed");
		else
			return this->handler->RestartIce(iceParameters);
	}

	void Transport::UpdateIceServers(const json& iceServers)
	{
		 
		if (this->closed)
			ERROR_EX_LOG("Transport closed");
		else
			return this->handler->UpdateIceServers(iceServers);
	}

	void Transport::SetHandler(Handler* handler)
	{
		 
		this->handler = handler;
	}

	void Transport::OnConnect(json& dtlsParameters)
	{
		 
		if (this->closed)
			ERROR_EX_LOG("Transport closed");

		return this->listener->OnConnect(this, dtlsParameters).get();
	}

	void Transport::OnConnectionStateChange(
	  webrtc::PeerConnectionInterface::IceConnectionState connectionState)
	{
		 
		// Update connection state.
		this->connectionState = connectionState;

		return this->listener->OnConnectionStateChange(
		  this, PeerConnection::iceConnectionState2String[connectionState]);
	}

	/* SendTransport */

	SendTransport::SendTransport(
	  Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const json& sctpParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json* extendedRtpCapabilities,
	  const std::map<std::string, bool>* canProduceByKind,
	  const json& appData)

	  : Transport(listener, id, extendedRtpCapabilities, appData), listener(listener),
	    canProduceByKind(canProduceByKind)
	{
		 
		if (sctpParameters != nullptr && sctpParameters.is_object())
		{
			this->hasSctpParameters = true;
			auto maxMessageSizeIt   = sctpParameters.find("maxMessageSize");

			if (maxMessageSizeIt->is_number_integer())
				this->maxSctpMessageSize = maxMessageSizeIt->get<size_t>();
		}

		json sendingRtpParametersByKind = {
			{ "audio", ortc::getSendingRtpParameters("audio", *extendedRtpCapabilities) },
			{ "video", ortc::getSendingRtpParameters("video", *extendedRtpCapabilities) }
		};

		json sendingRemoteRtpParametersByKind = {
			{ "audio", ortc::getSendingRemoteRtpParameters("audio", *extendedRtpCapabilities) },
			{ "video", ortc::getSendingRemoteRtpParameters("video", *extendedRtpCapabilities) }
		};

		this->sendHandler.reset(new SendHandler(
		  dynamic_cast<SendHandler::PrivateListener*>(this),
		  iceParameters,
		  iceCandidates,
		  dtlsParameters,
		  sctpParameters,
		  peerConnectionOptions,
		  sendingRtpParametersByKind,
		  sendingRemoteRtpParametersByKind));

		Transport::SetHandler(this->sendHandler.get());
	}

	/**
	 * Create a Producer.
	 */
	Producer* SendTransport::Produce(
	  Producer::Listener* producerListener,
	  webrtc::MediaStreamTrackInterface* track,
	  const std::vector<webrtc::RtpEncodingParameters>* encodings,
	  const json* codecOptions,
	  const json& appData)
	{
		 
		if (this->closed)
			ERROR_EX_LOG("SendTransport closed");
		else if (!track)
			ERROR_EX_LOG("missing track");
		else if (track->state() == webrtc::MediaStreamTrackInterface::TrackState::kEnded)
			ERROR_EX_LOG("track ended");
		else if (this->canProduceByKind->find(track->kind()) == this->canProduceByKind->end())
			ERROR_EX_LOG("cannot produce track kind");

		if (codecOptions)
		{
			ortc::validateProducerCodecOptions(const_cast<json&>(*codecOptions));
		}

		std::string producerId;
		std::vector<webrtc::RtpEncodingParameters> normalizedEncodings;

		if (encodings)
		{
			std::for_each(
			  encodings->begin(),
			  encodings->end(),
			  [&normalizedEncodings](const webrtc::RtpEncodingParameters& entry) {
				  webrtc::RtpEncodingParameters encoding;

				  encoding.active                   = entry.active;
				  encoding.max_bitrate_bps          = entry.max_bitrate_bps;
				  encoding.max_framerate            = entry.max_framerate;
				  encoding.scale_resolution_down_by = entry.scale_resolution_down_by;
				  encoding.network_priority         = entry.network_priority;

				  normalizedEncodings.push_back(encoding);
			  });
		}
		// 创建offer 的流程鸭
		// May throw.
		SendHandler::SendResult sendResult = this->sendHandler->Send(track, &normalizedEncodings, codecOptions);

		try
		{
			// This will fill rtpParameters's missing fields with default values.
			ortc::validateRtpParameters(sendResult.rtpParameters);

			// May throw.
			producerId =
			  this->listener->OnProduce(this, track->kind(), sendResult.rtpParameters, appData).get();
		}
		catch (...)
		{
			this->sendHandler->StopSending(sendResult.localId);

			throw;
		}

		Producer* producer = new Producer(
		  this,
		  producerListener,
		  producerId,
		  sendResult.localId,
		  sendResult.rtpSender,
		  track,
		  sendResult.rtpParameters,
		  appData);

		this->producers[producer->GetId()] = producer;

		return producer;
	}

	 
	void SendTransport::Close()
	{
		 
		if (this->closed)
			return;

		Transport::Close();
		
		// Close all Producers.
		for (auto& kv : this->producers)
		{
			auto* producer = kv.second;

			producer->TransportClosed();
		}

	 
		
	}

	void SendTransport::OnClose(Producer* producer)
	{
		 
		this->producers.erase(producer->GetId());

		if (this->closed)
			return;

		// May throw.
		this->sendHandler->StopSending(producer->GetLocalId());
	}

	 

	void SendTransport::OnReplaceTrack(const Producer* producer, webrtc::MediaStreamTrackInterface* track)
	{
		 
		return this->sendHandler->ReplaceTrack(producer->GetLocalId(), track);
	}

	void SendTransport::OnSetMaxSpatialLayer(const Producer* producer, uint8_t maxSpatialLayer)
	{
		 
		return this->sendHandler->SetMaxSpatialLayer(producer->GetLocalId(), maxSpatialLayer);
	}

	json SendTransport::OnGetStats(const Producer* producer)
	{
		 
		if (this->closed)
			ERROR_EX_LOG("SendTransport closed");

		return this->sendHandler->GetSenderStats(producer->GetLocalId());
	}

	/* RecvTransport */

	RecvTransport::RecvTransport(
	  Listener* listener,
	  const std::string& id,
	  const json& iceParameters,
	  const json& iceCandidates,
	  const json& dtlsParameters,
	  const json& sctpParameters,
	  const PeerConnection::Options* peerConnectionOptions,
	  const json* extendedRtpCapabilities,
	  const json& appData)
	  : Transport(listener, id, extendedRtpCapabilities, appData)
	{
		 
		this->hasSctpParameters = sctpParameters != nullptr && sctpParameters.is_object();

		this->recvHandler.reset(new RecvHandler(
		  dynamic_cast<RecvHandler::PrivateListener*>(this),
		  iceParameters,
		  iceCandidates,
		  dtlsParameters,
		  sctpParameters,
		  peerConnectionOptions));

		Transport::SetHandler(this->recvHandler.get());
	}

	/**
	 * Create a Consumer.
	 */
	Consumer* RecvTransport::Consume(
	  Consumer::Listener* consumerListener,
	  const std::string& id,
	  const std::string& producerId,
	  const std::string& kind,
	  json* rtpParameters,
	  const json& appData)
	{
		 
		if (this->closed)
			ERROR_EX_LOG("RecvTransport closed");
		else if (id.empty())
			ERROR_EX_LOG("missing id");
		else if (producerId.empty())
			ERROR_EX_LOG("missing producerId");
		else if (kind != "audio" && kind != "video")
			ERROR_EX_LOG("invalid kind");
		else if (!rtpParameters)
			ERROR_EX_LOG("missing rtpParameters");
		else if (!appData.is_object())
			ERROR_EX_LOG("appData must be a JSON object");
		else if (!ortc::canReceive(*rtpParameters, *this->extendedRtpCapabilities))
			ERROR_EX_LOG("cannot consume this Producer");

		// May throw.
		auto recvResult = this->recvHandler->Receive(id, kind, rtpParameters);

		auto* consumer = new Consumer(
		  this,
		  consumerListener,
		  id,
		  recvResult.localId,
		  producerId,
		  recvResult.rtpReceiver,
		  recvResult.track,
		  *rtpParameters,
		  appData);

		this->consumers[consumer->GetId()] = consumer;

		// If this is the first video Consumer and the Consumer for RTP probation
		// has not yet been created, create it now.
		if (!this->probatorConsumerCreated && kind == "video")
		{
			try
			{
				auto probatorRtpParameters =
				  ortc::generateProbatorRtpParameters(consumer->GetRtpParameters());
				std::string probatorId{ "probator" };

				// May throw.
				auto result = this->recvHandler->Receive(probatorId, kind, &probatorRtpParameters);

				NORMAL_EX_LOG("Consumer for RTP probation created");

				this->probatorConsumerCreated = true;
			}
			catch (std::runtime_error& error)
			{
				ERROR_EX_LOG("failed to create Consumer for RTP probation: %s", error.what());
			}
		}

		return consumer;
	}

	/**
	 * Create a DataConsumer.
	 */
	DataConsumer* RecvTransport::ConsumeData(
	  DataConsumer::Listener* listener,
	  const std::string& id,
	  const std::string& producerId,
	  const std::string& label,
	  const std::string& protocol,
	  const nlohmann::json& appData)
	{ 
		webrtc::DataChannelInit dataChannelInit;
		dataChannelInit.protocol = protocol;

		if (this->closed)
			ERROR_EX_LOG("RecvTransport closed");
		else if (id.empty())
			ERROR_EX_LOG("missing id");
		else if (producerId.empty())
			ERROR_EX_LOG("missing producerId");
		else if (!this->hasSctpParameters)
			ERROR_EX_LOG("Cannot use DataChannels with this transport. SctpParameters are not set.");

		// This may throw.
		auto recvResult = this->recvHandler->ReceiveDataChannel(label, dataChannelInit);

		auto dataConsumer = new DataConsumer(
		  listener, this, id, producerId, recvResult.dataChannel, recvResult.sctpStreamParameters, appData);

		this->dataConsumers[dataConsumer->GetId()] = dataConsumer;

		return dataConsumer;
	}

	void RecvTransport::Close()
	{
		 
		if (this->closed)
			return;

		Transport::Close();

		// Close all Consumers.
		for (auto& kv : this->consumers)
		{
			auto* consumer = kv.second;

			consumer->TransportClosed();
		}

		// Close all DataConsumers.
		for (auto& kv : this->dataConsumers)
		{
			auto* dataConsumer = kv.second;

			dataConsumer->TransportClosed();
		}
	}

	void RecvTransport::OnClose(Consumer* consumer)
	{
		 
		this->consumers.erase(consumer->GetId());

		if (this->closed)
			return;

		// May throw.
		this->recvHandler->StopReceiving(consumer->GetLocalId());
	}

	void RecvTransport::OnClose(DataConsumer* dataConsumer)
	{
		 
		this->dataConsumers.erase(dataConsumer->GetId());
	}

	json RecvTransport::OnGetStats(const Consumer* consumer)
	{
		 
		if (this->closed)
			ERROR_EX_LOG("RecvTransport closed");

		return this->recvHandler->GetReceiverStats(consumer->GetLocalId());
	}
} // namespace mediasoupclient
