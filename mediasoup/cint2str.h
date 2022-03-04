#ifndef _C_INT2STRE_H_
#define _C_INT2STRE_H_
#include <cstdlib>

#include <cstdio>
#include <string>
namespace chen {

	//convert int to hexadecimal string, add '\0' at end
	template<typename T>
	int int2str_hex(char* buf, T value);


	std::string hexmem(const void* buf, size_t len); 
	/*{
		string ret;
		char tmp[8];
		const uint8_t* data = (const uint8_t*)buf;
		for (size_t i = 0; i < len; ++i) {
			int sz = sprintf(tmp, "%.2x ", data[i]);
			ret.append(tmp, sz);
		}
		return ret;
	}*/
}

#endif //#ifndef _C_NET_TYPES_H_ _C_NET_TYPES_H_