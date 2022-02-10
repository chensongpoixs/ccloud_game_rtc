#ifndef _C_DEVICE_H_
#define _C_DEVICE_H_
//#include "cnoncopyable.h"
#include "json.hpp"
namespace chen {


	namespace deivce
	{
		nlohmann::json  GetNativeSctpCapabilities();
		bool  GetNativeRtpCapabilities(nlohmann::json & nativeRtpCapabilities);
	}
}

#endif // _C_DEVICE_H_