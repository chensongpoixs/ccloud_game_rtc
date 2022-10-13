/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/

#include "mediasoupclient.hpp"
 

#include <rtc_base/helpers.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/time_utils.h>
#include <sstream>

namespace mediasoupclient
{
	void Initialize() // NOLINT(readability-identifier-naming)
	{
		 

		rtc::InitializeSSL();
		rtc::InitRandom(rtc::Time());
	}

	void Cleanup() // NOLINT(readability-identifier-naming)
	{
 

		rtc::CleanupSSL();
	}

	 
} // namespace mediasoupclient
