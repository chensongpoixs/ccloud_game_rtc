/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef _C_DATA_CHANNEL_H_
#define _C_DATA_CHANNEL_H_

#include "cnet_types.h"
#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include "cnet_types.h"
#include <api/data_channel_interface.h>
namespace chen {


	class cdata_channel : public  webrtc::DataChannelObserver , public rtc::RefCountInterface
	{
	public:
		cdata_channel(rtc::scoped_refptr<webrtc::DataChannelInterface> data);
		
		//~cdata_channel();
	public:
		// The data channel state have changed.
		virtual void OnStateChange() override;
		//  A data buffer was successfully received.
		virtual void OnMessage(const webrtc::DataBuffer& buffer) override;
		// The data channel's buffered_amount has changed.
		virtual void OnBufferedAmountChange(uint64_t sent_data_size) override;
	protected:
	private:
		rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel;
	};
}

#endif //_C_DATA_CHANNEL_H_