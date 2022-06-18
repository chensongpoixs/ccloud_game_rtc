/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	time tools

*********************************************************************/

#include "ctime_api.h"
#include <cstdio>
#include <chrono>
#include <iostream>
//#include "cwin32_system.h"
#ifdef _MSC_VER
#pragma warning (disable:4996)
#endif
namespace chen
{
	namespace ctime_base_api
	{
		// ʱ��
		static time_t g_time_zone = 0;

		static long long g_time_adjust = 0;

		static long long g_time_offset = 0;

		static std::chrono::steady_clock::time_point g_cur_time = std::chrono::steady_clock::now();

		struct ctime_init
		{
			ctime_init()
			{
				time_t now = time(0); // UTC
				struct tm *ptmgm = gmtime(&now); // further convert to GMT presuming now in local
				time_t gmnow = mktime(ptmgm);
				g_time_zone = now - gmnow;
				if (ptmgm->tm_isdst > 0) {
					g_time_zone = g_time_zone - 60 * 60;
				}
				g_time_offset = g_time_zone;
				std::cout << "timezone=" << g_time_zone / ETC_Hour << std::endl;
			}
		};

		//call before main
		static const ctime_init time_init;

		void set_time_zone(int value)
		{
			if (value < -12 || value > 12) return;
			g_time_zone = value * ETC_Hour;
			g_time_offset = g_time_adjust + g_time_zone;
		}

		void set_time_adjust(int value)
		{
			g_time_adjust = value;
			g_time_offset = g_time_adjust + g_time_zone;
		}


		time_t get_gmt()
		{
			return (::time(NULL) + g_time_adjust);
		}

		void time_t_to_tm(time_t time, tm& out)
		{
			time += g_time_offset;
#if defined(_MSC_VER)
			::gmtime_s(&out, &time);
#elif defined(__linux__)  || defined(__APPLE__)
			::gmtime_r(&time, &out);
#else
#			pragma error "Unknown Platform Not Supported. Abort! Abort!"
#endif // 
		}

		tm time_t_to_tm(time_t time)
		{
			tm out;
			time_t_to_tm(time, out);
			return out;
		}

		void get_tm(tm& out)
		{
			time_t_to_tm(get_gmt(), out);
		}

		tm get_tm()
		{
			tm out;
			get_tm(out);
			return out;
		}


		// yyyy-MM-dd HH:mm:ss
		int time64_datetime_format(const tm& now_tm, char* out, char date_conn, char datetime_conn, char time_conn)
		{
			int nCount = 0;

			if (date_conn > 0)
			{
				nCount += sprintf(out + nCount, "%04d%c%02d%c%02d", now_tm.tm_year + 1900, date_conn
					, now_tm.tm_mon + 1, date_conn, now_tm.tm_mday);
			}
			else if (date_conn == 0)
			{
				nCount += sprintf(out + nCount, "%04d%02d%02d", now_tm.tm_year + 1900, now_tm.tm_mon + 1
					, now_tm.tm_mday);
			}

			if (datetime_conn > 0)
			{
				out[nCount] = datetime_conn;
				++nCount;
				out[nCount] = '\0';
			}

			if (time_conn > 0)
			{
				nCount += sprintf(out + nCount, "%02d%c%02d%c%02d", now_tm.tm_hour, time_conn
					, now_tm.tm_min, time_conn, now_tm.tm_sec);
			}
			else if (time_conn == 0)
			{
				nCount += sprintf(out + nCount, "%02d%02d%02d", now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec);
			}
			return nCount;
		}
		int time64_datetime_format(time_t time, char* out, char date_conn, char datetime_conn, char time_conn)
		{
			tm now_tm;
			time_t_to_tm(time, now_tm);
			return time64_datetime_format(now_tm, out, date_conn, datetime_conn, time_conn);
		}


		int datetime_format(char* out, char date_conn, char datetime_conn, char time_conn)
		{
			return time64_datetime_format(get_gmt(), out, date_conn, datetime_conn, time_conn);
		}

		time_t get_today_stamp_time64(int hour)
		{
			tm now_tm;
			time_t now = get_gmt();
			time_t_to_tm(now, now_tm);
			return (now - (now_tm.tm_hour - hour) * ETC_Hour
				- now_tm.tm_min * ETC_Minute
				- now_tm.tm_sec
				);
		}
		long long get_time_ms()
		{
			//g_cur_time
			
			return  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - g_cur_time ).count();

		}
	}//namespace ctime_base_api

	namespace ctime_util
    {

/* Return the UNIX time in microseconds */
       /* long long get_ustime(void)
        {
            struct timeval tv;
            long long ust;

            gettimeofday(&tv, NULL);
            ust = ((long long) tv.tv_sec) * 1000000;
            ust += tv.tv_usec;
            return ust;
        }*/
    }
/* Return the UNIX time in milliseconds */
//    long long get_mstime(void)
//    { return get_ustime() / 1000; }
//    }
}//namespace chen 
