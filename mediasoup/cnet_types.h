/***********************************************************************************************
				created: 		2019-03-01

				author:			chensong

				purpose:		net_types
************************************************************************************************/
#ifndef _C_NET_TYPES_H_
#define _C_NET_TYPES_H_
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <new>
//#include <codecvt>
#include <locale>
#include <string>

#define CPG_UNUSED(object) (void)object

namespace chen 
{
	typedef signed char		int8;
	typedef unsigned char	uint8;

	typedef signed short	int16;
	typedef unsigned short	uint16;

	typedef signed int		int32;
	typedef unsigned int	uint32;

#if defined(_MSC_VER)

	typedef signed long long	int64;
	typedef unsigned long long	uint64;

#elif defined(__GNUC__)  ||  defined(__APPLE__) 

	typedef signed long		int64;
	typedef unsigned long	uint64;
#else
#pragma error "unknow platform!!!"

#endif



#if defined(_MSC_VER)
#define FUNCTION __FUNCTION__
#elif defined(__linux__) ||  defined(__APPLE__)
#define FUNCTION __PRETTY_FUNCTION__
#else
#pragma error "unknow platform!!!"

#endif
}



#endif // !_C_NET_TYPES_H_
