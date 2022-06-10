#ifndef _C_DATA_CONSUMER_H_
#define _C_DATA_CONSUMER_H_
//#include "cnoncopyable.h"
#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include "cnet_types.h"
#include <api/data_channel_interface.h>
#include "json.hpp"
//#include "crecv_transport.h"
namespace chen {
	class crecv_transport;
	class cdataconsumer : public  webrtc::DataChannelObserver
	{
	public:
		cdataconsumer( crecv_transport * transport_ptr)
			:id("")
			, dataProducerId("")
			, dataChannel(nullptr)
			, m_transport(transport_ptr)
		{}
		//~cdataconsumer();
		bool init(const std::string& id,
			const std::string& dataProducerId,
			rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel,
			const nlohmann::json& sctpStreamParameters,
			const nlohmann::json& appData_);
		void Destroy();
	public:
		const std::string& GetId() const;
		std::string GetLocalId() const;
		const std::string& GetDataProducerId() const;
		const nlohmann::json& GetSctpStreamParameters() const;
		webrtc::DataChannelInterface::DataState GetReadyState() const;
		std::string GetLabel() const;
		std::string GetProtocol() const;
		const nlohmann::json& GetAppData() const;
 
	public:
		// The data channel state has changed.
		void OnStateChange() override;
		//  A data buffer was successfully received.
		void OnMessage(const webrtc::DataBuffer& buffer) override;
		// The data channel's buffered_amount has changed.
		void OnBufferedAmountChange(uint64_t sentDataSize) override;

	private:
		std::string id;
		std::string dataProducerId;
		rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel;
	 
		nlohmann::json sctpParameters;
		nlohmann::json appData;
		  crecv_transport * m_transport;
	};
}

#endif // !_C_DATA_CONSUMER_H_