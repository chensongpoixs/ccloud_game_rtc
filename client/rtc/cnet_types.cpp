/***********************************************************************************************
	created: 		2019-03-01
	
	author:			chensong
					
	purpose:		net_types
************************************************************************************************/
#include "cnet_types.h"

namespace chen 
{
	inline void check_socket_types_size()
	{
		static_assert(sizeof(int8) == 1, "sizeof(int8) != 1");
		static_assert(sizeof(uint8) == 1, "sizeof(uint8) != 1");
		static_assert(sizeof(int16) == 2, "sizeof(int16) != 2");
		static_assert(sizeof(uint16) == 2, "sizeof(uint16) != 2");
		static_assert(sizeof(int32) == 4, "sizeof(int32) != 4");
		static_assert(sizeof(uint32) == 4, "sizeof(uint32) != 4");
		static_assert(sizeof(int64) == 8, "sizeof(int64) != 8");
		static_assert(sizeof(uint64) == 8, "sizeof(uint64) != 8");
	}	
}  // chen
