/***********************************************************************************************
	created: 		2019-03-02
	
	author:			chensong
					
	purpose:		log
************************************************************************************************/


#ifndef _C_LOG_H_
#define _C_LOG_H_
#include "cnet_types.h"
#include <cstdio>
#include <cstdlib>
//#include "cnoncopyable.h"
#include "casync_log.h"
 
namespace chen {
	
	class  clog //: private cnoncopyable
	{
		 
	private:
		enum { EBuf_Size = 1024 };
	public:
		
		explicit clog();
		explicit clog(ELogLevelType level);
		explicit clog(ELogLevelType level, const char* func, int line);
		~clog();
	public:
		static bool init(ELogStorageType storage_type = ELogStorageScreenFilePost, const std::string& host = "127.0.0.1", uint32 port = 80);
		
		static void  var_log(ELogLevelType level, const char* format, ...);

		static void set_level(ELogLevelType level);
		static void destroy();
	public:
		clog& operator<<(bool);
		clog& operator<<(char);

		clog& operator<<(signed char);
		clog& operator<<(unsigned char);

		clog& operator<<(signed short);
		clog& operator<<(unsigned short);

		clog& operator<<(signed int);
		clog& operator<<(unsigned int);

		clog& operator<<(signed long);
		clog& operator<<(unsigned long);

		clog& operator<<(signed long long);
		clog& operator<<(unsigned long long);

		clog& operator<<(const char *);
		clog& operator<<(const std::string&);

		clog& operator<<(float);
		clog& operator<<(double);
	private:
		char m_data[EBuf_Size];
		int  m_len;
		ELogLevelType m_level;
	
	};


#define LOG clog


	//标准log 有时间前缀
#define LOG_SYSTEM LOG(ELogLevel_System)
#define LOG_FATAL  LOG(ELogLevel_Fatal, FUNCTION, __LINE__)
#define LOG_ERROR  LOG(ELogLevel_Error, FUNCTION, __LINE__)
#define LOG_WARN   LOG(ELogLevel_Warn, FUNCTION, __LINE__)
#define LOG_INFO   LOG(ELogLevel_Info)
#define LOG_DEBUG  LOG(ELogLevel_Debug)

#define VAR_LOG LOG::var_log

#define NORMAL_LOG(format, ...)		VAR_LOG(ELogLevel_Info, format, ##__VA_ARGS__)
#define ERROR_LOG(format, ...)		VAR_LOG(ELogLevel_Error, format, ##__VA_ARGS__)
#define WARNING_LOG(format, ...)	VAR_LOG(ELogLevel_Warn, format, ##__VA_ARGS__)
#define SYSTEM_LOG(format, ...)		VAR_LOG(ELogLevel_System, format, ##__VA_ARGS__)
#define DEBUG_LOG(format, ...)		VAR_LOG(ELogLevel_Debug, format, ##__VA_ARGS__)

#define NORMAL_EX_LOG(format, ...)	NORMAL_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)

#define WARNING_EX_LOG(format, ...)	WARNING_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)

#define ERROR_EX_LOG(format, ...)	ERROR_LOG("[%s][%d]" format, FUNCTION, __LINE__, ##__VA_ARGS__)
} // namespace chen

#endif //!#define _C_LOG_H_