/***********************************************************************************************
					created: 		2018-10-02

					author:			chensong

					purpose:		async_log
************************************************************************************************/

#ifndef _C_ASYNC_LOG_H
#define _C_ASYNC_LOG_H
#include "clog_define.h"
#include <string>
#include <iostream>
//#include <cstring>
#include <iostream>
#include <fstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <ctime>
#include <cstdio>
#include <cstdlib>
//#include "cnoncopyable.h"
//#include "clog_color.h"
#include <thread>
#include <list>
#include "cnet_types.h"
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <ostream>

namespace chen
{
	struct clog_item;

	

	class casync_log //: private cnoncopyable
	{
		 
	private:
		typedef std::condition_variable					ccond;
		typedef std::atomic_bool						catomic_bool;
	public:
		explicit casync_log();
		~casync_log();
		bool init(ELogStorageType storage_type , const   std::string  & host, uint32 port);
		void destroy();
	public:
		void append_var(ELogLevelType level, const char* format, va_list ap);

		void set_level(ELogLevelType level);
	private:
		bool			_init_log();
	private:
		//工作线程
		void			_work_pthread();
		std::shared_ptr<clog_item>		_get_new_buf();
		void			_handler_log_item(const std::shared_ptr<clog_item> log_item_ptr);
		
		void			_handler_check_log_file();
	private: 
		std::string				m_host;
		uint32					m_port;
		int32					m_level_log;
		ELogStorageType			m_storage_type;
		catomic_bool			m_stoped;
		std::thread				m_thread;
		std::mutex				m_lock;
		ccond					m_condition;    /*是否有任务需要处理*/
		std::ofstream			m_fd;
		int32_t					m_date_time;
		std::string				m_path;
		std::list<std::shared_ptr<clog_item>>	m_log_item;
	};
}


#endif // !#define _C_ASYNC_LOG_H
