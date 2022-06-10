#include "cint2str.h"


namespace chen {

	static const char s_hex_digits[] = "0123456789ABCDEF";

	template<typename T>
	int int2str_hex(char* buf, T value)
	{
		T i = value;
		char* p = buf;

		do
		{
			int lsd = static_cast<int>(i % 16);
			i /= 16;
			*p++ = s_hex_digits[lsd];
		} while (i != 0);

		*p = '\0';
		std::reverse(buf, p);

		return static_cast<int>(p - buf);
	}

	std::string hexmem(const void* buf, size_t len)
	{
		std::string ret;
		char tmp[8];
		const uint8_t* data = (const uint8_t*)buf;
		for (size_t i = 0; i < len; ++i) {
			int sz = sprintf(tmp, "%.2x ", data[i]);
			ret.append(tmp, sz);
		}
		return ret;
	}
}