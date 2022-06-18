#include "cdataconsumer.h"
#include "clog.h"
#include "crecv_transport.h"
#include "cinput_device.h"
namespace chen {


	bool cdataconsumer::init(const std::string& id_,
		const std::string& dataProducerId_,
		rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel_,
		const nlohmann::json& sctpStreamParameters_,
		const nlohmann::json& appData_)
	{
		id = id_;
		dataProducerId = dataProducerId_;
		dataChannel = dataChannel_;
		sctpParameters = sctpStreamParameters_;
		appData = appData_;
		dataChannel->RegisterObserver(this);
		return true;
	}
	void cdataconsumer::Destroy()
	{
		if (dataChannel)
		{
			//dataChannel->UnregisterObserver();
			//dataChannel->Close();
		}
	}
	/**
	* DataConsumer id.
	*/
	const std::string& cdataconsumer::GetId() const
	{
		 
		return this->id;
	}

	std::string cdataconsumer::GetLocalId() const
	{
		 
		return std::to_string(this->dataChannel->id());
	}

	/**
	* Associated DataProducer id.
	*/
	const std::string& cdataconsumer::GetDataProducerId() const
	{
		 
		return this->dataProducerId;
	}

	/**
	* SCTP stream parameters.
	*/
	const  nlohmann::json& cdataconsumer::GetSctpStreamParameters() const
	{
		 
		return this->sctpParameters;
	}

	/**
	* DataChannel readyState.
	*/
	webrtc::DataChannelInterface::DataState cdataconsumer::GetReadyState() const
	{
		 
		return this->dataChannel->state();
	}

	/**
	* DataChannel label.
	*/
	std::string cdataconsumer::GetLabel() const
	{
		 
		return this->dataChannel->label();
	}

	/**
	* DataChannel protocol.
	*/
	std::string cdataconsumer::GetProtocol() const
	{
		 
		return this->dataChannel->protocol();
	}

	/**
	* App custom data.
	*/
	const  nlohmann::json& cdataconsumer::GetAppData() const
	{ 
		return this->appData;
	}


	// The data channel state has changed.
	void cdataconsumer::OnStateChange()
	{
		 
		webrtc::DataChannelInterface::DataState state = this->dataChannel->state();

		switch (state)
		{
		case webrtc::DataChannelInterface::DataState::kConnecting:
			//this->listener->OnConnecting(this);
			NORMAL_EX_LOG("id = %s, OnConnecting", id.c_str());
			break;
		case webrtc::DataChannelInterface::DataState::kOpen:
			NORMAL_EX_LOG("id = %s, OnOpen", id.c_str());
			//this->listener->OnOpen(this);
			break;
		case webrtc::DataChannelInterface::DataState::kClosing:
			//this->listener->OnClosing(this);
			NORMAL_EX_LOG("id = %s, OnClosing", id.c_str());
			break;
		case webrtc::DataChannelInterface::DataState::kClosed:
			//this->listener->OnClose(this);
			NORMAL_EX_LOG("id = %s, OnClose", id.c_str());
			//m_transport->close();
			m_transport->close_dataconsumer(id);
			break;
		default:
			ERROR_EX_LOG("unknown state %s", webrtc::DataChannelInterface::DataStateString(state));
			break;
		}
	}

	//  A data buffer was successfully received.
	void cdataconsumer::OnMessage(const webrtc::DataBuffer& buffer)
	{
		std::string s = std::string(buffer.data.data<char>(), buffer.data.size());
		//NORMAL_EX_LOG("DataConsumer -------------- %s -----------------------------------", s.c_str());
		//this->listener->OnMessage(this, buffer);
		s_input_device.OnMessage(id, buffer);
	}

	// The data channel's buffered_amount has changed.
	void cdataconsumer::OnBufferedAmountChange(uint64_t /*sentDataSize*/)
	{
		NORMAL_EX_LOG("");
		// Should not happen on consumer.
	}
}