/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#include "cdata_channel.h"
#include "cinput_device.h"
#include "clog.h"
namespace chen {

	cdata_channel::cdata_channel(rtc::scoped_refptr<webrtc::DataChannelInterface> data)
		: dataChannel(data) 
	{
		  
		dataChannel->RegisterObserver(this);
	}

	/*cdata_channel::~cdata_channel()
	{
		if (dataChannel)
		{
			dataChannel->UnregisterObserver();
			dataChannel->Close();
		}
	}*/
	void cdata_channel::OnStateChange()
	{
		webrtc::DataChannelInterface::DataState state = this->dataChannel->state();

		switch (state)
		{
		case webrtc::DataChannelInterface::DataState::kConnecting:
			//this->listener->OnConnecting(this);
			NORMAL_EX_LOG(" , OnConnecting" );
			break;
		case webrtc::DataChannelInterface::DataState::kOpen:
			NORMAL_EX_LOG("id =  , OnOpen" );
			//this->listener->OnOpen(this);
			break;
		case webrtc::DataChannelInterface::DataState::kClosing:
			//this->listener->OnClosing(this);
			NORMAL_EX_LOG("id =  , OnClosing" );
			break;
		case webrtc::DataChannelInterface::DataState::kClosed:
			//this->listener->OnClose(this);
			NORMAL_EX_LOG(" , OnClose" );
			//m_transport->close();
			//m_transport->close_dataconsumer(id);
			if (dataChannel)
			{
				dataChannel->UnregisterObserver();
				dataChannel->Close();
			}
			break;
		default:
			ERROR_EX_LOG("unknown state %s", webrtc::DataChannelInterface::DataStateString(state));
			break;
		}
	}
	void cdata_channel::OnMessage(const webrtc::DataBuffer & buffer)
	{
		std::string s = std::string(buffer.data.data<char>(), buffer.data.size());
		NORMAL_EX_LOG("DataConsumer -------------- %s -----------------------------------", s.c_str());
		//this->listener->OnMessage(this, buffer);
		s_input_device.OnMessage(0, buffer);
	}
	void cdata_channel::OnBufferedAmountChange(uint64_t sent_data_size)
	{
	}
}