#define MSC_CLASS "mediasoupclient"

#include "mediasoupclient.hpp"
#include "Logger.hpp"
#include "version.hpp"
#include <rtc_base/helpers.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/time_utils.h>
#include <sstream>

namespace mediasoupclient
{
	void Initialize() // NOLINT(readability-identifier-naming)
	{
		MSC_TRACE();

		rtc::InitializeSSL();
		rtc::InitRandom(rtc::Time());
	}

	void Cleanup() // NOLINT(readability-identifier-naming)
	{
		MSC_TRACE();

		rtc::CleanupSSL();
	}

	 
} // namespace mediasoupclient
