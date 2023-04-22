/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#ifndef _C_BASE64_H_
#define _C_BASE64_H_
#include <cstdlib>
#include <cctype>
#include <string>
#include <utility>
#include <cstdio>
#include <string>
namespace chen {
namespace  base64{
	inline
		char const*
		get_alphabet()
	{
		static char constexpr tab[] = {
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
		};
		return &tab[0];
	}

	inline
		signed char const*
		get_inverse()
	{
		static signed char constexpr tab[] = {
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //   0-15
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //  16-31
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, //  32-47
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, //  48-63
			-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //  64-79
			15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, //  80-95
			-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //  96-111
			41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, // 112-127
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 128-143
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 144-159
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 160-175
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 176-191
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 192-207
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 208-223
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 224-239
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // 240-255
		};
		return &tab[0];
	}


	/// Returns max chars needed to encode a base64 string
	inline
		std::size_t constexpr
		encoded_size(std::size_t n)
	{
		return 4 * ((n + 2) / 3);
	}

	/// Returns max bytes needed to decode a base64 string
	inline
		std::size_t constexpr
		decoded_size(std::size_t n)
	{
		return n / 4 * 3; // requires n&3==0, smaller
						  //return 3 * n / 4;
	}

	/** Encode a series of octets as a padded, base64 string.

	The resulting string will not be null terminated.

	@par Requires

	The memory pointed to by `out` points to valid memory
	of at least `encoded_size(len)` bytes.

	@return The number of characters written to `out`. This
	will exclude any null termination.
	*/
	template<class = void>
	std::size_t
		encode(void* dest, void const* src, std::size_t len)
	{
		char*      out = static_cast<char*>(dest);
		char const* in = static_cast<char const*>(src);
		auto const tab = base64::get_alphabet();

		for(auto n = len / 3; n--;)
		{
			*out++ = tab[ (in[0] & 0xfc) >> 2];
			*out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
			*out++ = tab[((in[2] & 0xc0) >> 6) + ((in[1] & 0x0f) << 2)];
			*out++ = tab[  in[2] & 0x3f];
			in += 3;
		}

		switch(len % 3)
		{
		case 2:
			*out++ = tab[ (in[0] & 0xfc) >> 2];
			*out++ = tab[((in[0] & 0x03) << 4) + ((in[1] & 0xf0) >> 4)];
			*out++ = tab[                         (in[1] & 0x0f) << 2];
			*out++ = '=';
			break;

		case 1:
			*out++ = tab[ (in[0] & 0xfc) >> 2];
			*out++ = tab[((in[0] & 0x03) << 4)];
			*out++ = '=';
			*out++ = '=';
			break;

		case 0:
			break;
		}

		return out - static_cast<char*>(dest);
	}

	/** Decode a padded base64 string into a series of octets.

	@par Requires

	The memory pointed to by `out` points to valid memory
	of at least `decoded_size(len)` bytes.

	@return The number of octets written to `out`, and
	the number of characters read from the input string,
	expressed as a pair.
	*/
	template<class = void>
	std::pair<std::size_t, std::size_t>
		decode(void* dest, char const* src, std::size_t len)
	{
		char* out = static_cast<char*>(dest);
		auto in = reinterpret_cast<unsigned char const*>(src);
		unsigned char c3[3], c4[4];
		int i = 0;
		int j = 0;

		auto const inverse = base64::get_inverse();

		while(len-- && *in != '=')
		{
			auto const v = inverse[*in];
			if(v == -1)
				break;
			++in;
			c4[i] = v;
			if(++i == 4)
			{
				c3[0] =  (c4[0]        << 2) + ((c4[1] & 0x30) >> 4);
				c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
				c3[2] = ((c4[2] & 0x3) << 6) +   c4[3];

				for(i = 0; i < 3; i++)
					*out++ = c3[i];
				i = 0;
			}
		}

		if(i)
		{
			c3[0] = ( c4[0]        << 2) + ((c4[1] & 0x30) >> 4);
			c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
			c3[2] = ((c4[2] & 0x3) << 6) +   c4[3];

			for(j = 0; j < i - 1; j++)
				*out++ = c3[j];
		}

		return {out - static_cast<char*>(dest),
			in - reinterpret_cast<unsigned char const*>(src)};
	}

} // base64


template<class = void>
std::string
base64_encode(std::uint8_t const* data,
	std::size_t len)
{
	std::string dest;
	dest.resize(base64::encoded_size(len));
	dest.resize(base64::encode(&dest[0], data, len));
	return dest;
}

inline
std::string
base64_encode(std::string const& s)
{
	return base64_encode(reinterpret_cast <
		std::uint8_t const*> (s.data()), s.size());
}

template<class = void>
std::string
base64_decode(std::string const& data)
{
	std::string dest;
	dest.resize(base64::decoded_size(data.size()));
	auto const result = base64::decode(
		&dest[0], data.data(), data.size());
	dest.resize(result.first);
	return dest;
}

}

#endif // _C_BASE64_H_