/***********************************************************************************************
	created: 		2019-04-30
	
	author:			chensong
					
	purpose:		log
************************************************************************************************/
#include "clog.h"
#include <cstdarg>

namespace chen {
	static casync_log* g_log_ptr = nullptr;
	clog::clog()
		:m_len(0)
		, m_level(ELogLevel_None)
	{
	}
	clog::clog(ELogLevelType level)
		: m_len(0)
		, m_level(level)
	{
	}
	clog::clog(ELogLevelType level, const char * func, int line)
		: m_len(0)
		, m_level(level)
	{
		*this << '[' << func << ':' << line << "] ";
	}
	bool clog::init( ELogStorageType storage_type, const std::string& host , uint32 port   )
	{
		printf("host = %s, port = %lu, storage_type = %d\n", host.c_str(), port, storage_type);
		if (g_log_ptr)
		{
			printf("  casync_log != null  !!!" );
			return false;
		}
		g_log_ptr = new casync_log();
		if (!g_log_ptr)
		{
			printf(" alloc casync_log new fail ");
			return false;
		}
		if (!g_log_ptr->init(storage_type, host, port ))
		{
			printf( "log init error \n");
			return false;
		}
		return true;
	}
	
	void clog::var_log(ELogLevelType level, const char * format, ...)
	{
		if (!g_log_ptr)
		{
			printf("g_log_ptr == null\n");
			return;
		}
		//return;
		va_list argptr;
		va_start(argptr, format);
		g_log_ptr->append_var(level, format, argptr);
		va_end(argptr);
	}
	void clog::set_level(ELogLevelType level)
	{
		if (!g_log_ptr)
		{
			return;
		}
		g_log_ptr->set_level(level);
	}
	void clog::destroy()
	{
		if (g_log_ptr)
		{
			g_log_ptr->destroy();
		}
	}
	clog & clog::operator<<(bool value)
	{
		if (value)
		{
			return *this << '1';
		}
		else
		{
			return *this << '0';
		}
	}
	clog & clog::operator<<(char value)
	{
		if (m_len < EBuf_Size)
		{
			m_data[m_len++] = value;
		}
		return *this;
	}
	clog & clog::operator<<(signed char)
	{
		return *this;
	}
	clog & clog::operator<<(unsigned char)
	{
		return *this;
	}
	clog & clog::operator<<(signed short)
	{
		return *this;
	}
	clog & clog::operator<<(unsigned short)
	{
		return *this;
	}
	clog & clog::operator<<(signed int)
	{
		return *this;
	}
	clog & clog::operator<<(unsigned int)
	{
		return *this;
	}
	clog & clog::operator<<(signed long)
	{
		return *this;
	}
	clog & clog::operator<<(unsigned long)
	{
		return *this;
	}
	clog & clog::operator<<(signed long long)
	{
		return *this;
	}
	clog & clog::operator<<(unsigned long long)
	{
		return *this;
	}
	clog & clog::operator<<(const char *)
	{
		return *this;
	}
	clog & clog::operator<<(const std::string &)
	{
		return *this;
	}
	clog & clog::operator<<(float)
	{
		return *this;
	}
	clog & clog::operator<<(double)
	{
		return *this;
	}
	clog::~clog()
	{
		//if (g_log_ptr && m_len > 0 /*&& m_level <= g_log_ptr->get_level()*/)
		//{
		//	g_log_ptr->append_fix(m_level, m_data, m_len);
		//}
	}


} //namespace chen

