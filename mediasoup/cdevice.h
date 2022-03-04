#ifndef _C_DEVICE_H_
#define _C_DEVICE_H_
//#include "cnoncopyable.h"
#include "json.hpp"
namespace chen {

#if defined(WEBRTC_WIN)
	//void test_win() {}
#endif
	namespace deivce
	{
		nlohmann::json  GetNativeSctpCapabilities();
		bool  GetNativeRtpCapabilities(nlohmann::json & nativeRtpCapabilities);
	}
}

#endif // _C_DEVICE_H_