/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef MSC_SCALABILITY_MODE_HPP
#define MSC_SCALABILITY_MODE_HPP
#include "json.hpp"
#include <string>

namespace mediasoupclient
{
	nlohmann::json parseScalabilityMode(const std::string& scalabilityMode);
} // namespace mediasoupclient

#endif
