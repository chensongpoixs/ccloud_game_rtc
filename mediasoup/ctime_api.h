/********************************************************************
created:	2019-03-24

author:		chensong

purpose:	time tools

*********************************************************************/

#ifndef _C_TIME_API_H
#define _C_TIME_API_H
#include "ctime_const.h"
#include <ctime>
namespace chen
{

	namespace ctime_base_api
	{
		// ����ʱ��
		void set_time_zone(int value);
		// ��Ϊ����ʱ��
		void set_time_adjust(int value);

		time_t get_gmt();

		void time_t_to_tm(time_t time, tm& out);
		tm time_t_to_tm(time_t time);

		void get_tm(tm& out);
		tm get_tm();

		// yyyy-MM-dd HH:mm:ss
		int time64_datetime_format(const tm& now_tm, char* out, char date_conn, char datetime_conn, char time_conn);
		//chen::ctime_base_api::time64_datetime_format(::time(NULL), buf, '-', ' ', ':');
		int time64_datetime_format(time_t time, char* out, char date_conn, char datetime_conn, char time_conn);
		int datetime_format(char* out, char date_conn, char datetime_conn, char time_conn);
		time_t get_today_stamp_time64(int hour);




	
		long long get_time_ms();

	}  // namespace ctime_base_api



}  // namespace chen

#endif //!#define _C_TIME_API_H
