/***********************************************************************************************
						created: 		2018-10-02

						author:			chensong

						purpose:		async_log
************************************************************************************************/

#include "casync_log.h"
//#include "ctime_api.h"
#include <cassert>
#include <thread>
#include <iostream>
#include <mutex>
#include "clog.h"
#include <sstream>
#include <string>
#include "ctime_api.h"
//#include "httplib.h"
#ifdef _MSC_VER
#include <io.h>
#include <direct.h>
#elif defined(__linux__) ||defined(__APPLE__)
#include <unistd.h>
#include <dirent.h>
#else
// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
namespace chen {

#pragma pack(push,1)
	struct clog_item
	{
		int				timestamp;
		signed char		level;
		std::string		data;
	};
#pragma pack(pop)
//	static const unsigned int LOG_ITEM_SIZE = sizeof(clog_item);
	static const unsigned int LOG_BUF_MAX_SIZE = 1024 * 50; // default 1024 * 1024 statck win too small

	enum ELogColorType
	{
		ELCT_White = 0,
		ELCT_Red,
		ELCT_Pink,
		ELCT_Yellow,
		ELCT_Blue,
		ELCT_Green,
		ELCT_Max
	};
	struct clog_name_color
	{
		const char* name;
		ELogColorType color;
	}
	static const g_log_name_colors[] =
	{
		{ "",		ELCT_White },
		{ "system", ELCT_Blue },
		{ "fatal",  ELCT_Red },
		{ "error",  ELCT_Pink },
		{ "warn",	ELCT_Yellow },
		{ "info",	ELCT_White },
		{ "debug",	ELCT_Green },
	};

	static void gen_log_file_name(char* p, const std::string& path, const std::string& name
		, const std::string& ext, ELogNameType name_type, int& day_stamp)
	{
		p += sprintf(p, "%s", path.c_str());
		char dateTime[ASCII_DATETIME_LEN] = { 0 };
		if (name_type == ELogName_Date)
		{
			ctime_base_api::datetime_format(dateTime, 0, -1, -1);
			 
		}
		else if (name_type == ELogName_DateTime)
		{
			ctime_base_api::datetime_format(dateTime, 0, 0, 0);
			 
		}
		else if (name_type == ELogName_AutoDate)
		{
			if (day_stamp == 0)
			{
				day_stamp = ctime_base_api::get_today_stamp_time64(0);
			}
			ctime_base_api::datetime_format(dateTime, 0, 0, 0);
			 
			day_stamp += ETC_Day;
		}
		sprintf(p, "%s_%s%s", dateTime, name.c_str(), ext.c_str());
	}
	 
	casync_log::casync_log()
		: m_host("127.0.0.1")
		, m_port(80)  
		, m_level_log(ELogLevel_Num)
		, m_storage_type(ELogStorageScreenFilePost)
		, m_stoped(false)
		, m_date_time(0)
		, m_path("./log")
	{ 
	}

	casync_log::~casync_log()
	{
		
	}
	bool casync_log::init(ELogStorageType storagetype, const   std::string  & host, uint32 port)
	{ 
		//NORMAL_EX_LOG("host = %s, port = %d, show_screen = %d", host.c_str(), port, show_screen);


#ifdef _MSC_VER
		if (::_access(m_path.c_str(), 0) == -1)
		{

			::_mkdir(m_path.c_str());
		}
#elif defined(__GNUC__) ||defined(__APPLE__)
		if (::access(m_path.c_str(), 0) == -1)
		{
			::mkdir(m_path.c_str(), 0777);
		}
#else
		// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
		return false;
#endif
		if (storagetype & ELogStorageFile)
		{
			char log_name[1024] = {0};
			gen_log_file_name(log_name, m_path + "/", "rtc_native", ".log", ELogName_AutoDate, m_date_time);
			m_fd.open(log_name, std::ios::out | std::ios::trunc);
			if (!m_fd.is_open())
			{
				std::cout << "not open log file dest url = " << log_name << std::endl;
				return false;
			}
		}
		m_host = host;
		m_port = port;
		m_storage_type = storagetype;
		
		std::thread td(&casync_log::_work_pthread, this);
		m_thread.swap(td);
		//std::move(m_thread, td);
		return true;
	}
	
	void casync_log::append_var(ELogLevelType level, const char * format, va_list ap)
	{
		
		if (level < ELogLevel_None || level > ELogLevel_Num  || level >= m_level_log)
		{
			return;
		}

		if (m_stoped)
		{
			return;
		}
		 
		int32 cnt = -1;
		char buffer[LOG_BUF_MAX_SIZE] = {0};
		cnt = vsnprintf(buffer, LOG_BUF_MAX_SIZE, format, ap);
		if (cnt <= 0)
		{ 
			return;
		}
		{
			
			std::shared_ptr<clog_item>  log_item_ptr = _get_new_buf();
			if (!log_item_ptr)
			{
				return;
			}
			{
				std::lock_guard<std::mutex> lock{ m_lock };

				//得到一个新的log

				log_item_ptr->level = level;
				log_item_ptr->timestamp = static_cast<int>(ctime_base_api::get_gmt());
				log_item_ptr->data = buffer;
				m_log_item.push_back(log_item_ptr);
			}
		}
		
		{
			m_condition.notify_one();
		}
	}
	void casync_log::set_level(ELogLevelType level)
	{
		m_level_log = level;
	}
	bool casync_log::_init_log()
	{
		return true;
	}
	void casync_log::_work_pthread()
	{
		std::shared_ptr<clog_item>  log_item_ptr;
		std::list<std::shared_ptr<clog_item>> log_items;
		while (!m_stoped  )
		{
			//当又有数据就立即写入日志文件中
			if (m_log_item.size() <= 0)
			{
				std::unique_lock<std::mutex> lock(m_lock);
				//有数据或者停止才可以写入日志中
				m_condition.wait(lock, [this]() {return (m_log_item.size() > 0 || m_stoped); });
			}
			if (m_log_item.empty())
            {
                continue;
            }
            {
                std::lock_guard<std::mutex> lock{m_lock};
                log_items = m_log_item;
                m_log_item.clear();
            }
			if (m_storage_type & ELogStorageFile)
			{
				_handler_check_log_file();
			}
			
			while (!log_items.empty())
			{
				{
//					std::lock_guard<std::mutex> lock{ m_lock };
					log_item_ptr = log_items.front();
                    log_items.pop_front();
				}

				if (!log_item_ptr)
				{
					continue;
				}
                
				_handler_log_item(log_item_ptr);
				log_item_ptr = nullptr;
			}


			
			 
		}


		if (m_log_item.size() > 0)
		{
			while (!m_log_item.empty())
			{
				{
					//					std::lock_guard<std::mutex> lock{ m_lock };
					log_item_ptr = m_log_item.front();
					m_log_item.pop_front();
				}

				if (!log_item_ptr)
				{
					continue;
				}

				_handler_log_item(log_item_ptr);
				log_item_ptr = nullptr;
			}
		}
	}
	std::shared_ptr<clog_item>   casync_log::_get_new_buf()
	{
		std::shared_ptr<clog_item>  log_item_ptr = std::make_shared<clog_item>( clog_item());
		if (!log_item_ptr)
		{
			return NULL;
		}
		return log_item_ptr;
	}

	void casync_log::_handler_log_item(const std::shared_ptr<clog_item>   log_item_ptr)
	{
		std::ostringstream m_stream;
		 
		
		char dateTime[ASCII_DATETIME_LEN] = { 0 };
		ctime_base_api::time64_datetime_format(log_item_ptr->timestamp, dateTime, '-', ' ', ':');
 	
		m_stream << '[' << dateTime << "] [" << g_log_name_colors[log_item_ptr->level].name << "] ";
		m_stream << log_item_ptr->data;
		if ( (m_storage_type & ELogStoragePost) && log_item_ptr->data.length() > 0)
		{
			//m_stream  << "\n";
			////std::string host =webrtc::g_cfg.get_string(webrtc::ECI_MediaSoup_Host) ;
			//httplib::Client cli(m_host, m_port);
			//std::string url = "http://"+ m_host + ":" +std::to_string(m_port) +"/decoder/decoder.php";
			//auto res = cli.Post(url.c_str(), m_stream.str().c_str(), "application/json");
			//if (res && res->status == 200)
			//{
			//	// send ok	
			//}
			//else
			//{
			//	printf("[http][error]send host = %s , [port = %d]\n", m_host.c_str(), m_port);
			//	fflush(stdout);
			//	//fflush();
			//}
			
		}

		if ((m_storage_type & ELogStorageFile) && m_fd.is_open() && log_item_ptr->data.length() > 0)
		{
			m_fd.write(m_stream.str().c_str(), m_stream.str().length());
			 
			m_fd.write("\n", 1);
			m_fd.flush();

		}
		// ELogStorageScreenFilePost
		if ( (m_storage_type & ELogStorageScreen) && log_item_ptr->data.length() > 0)
		{
			std::cout.write(m_stream.str().c_str(), m_stream.str().length()); 
			std::cout.write("\n", 1);
			std::cout.flush();
		}
	}
	void			casync_log::_handler_check_log_file()
	{
		int32 day_time = ctime_base_api::get_today_stamp_time64(0);
		if (day_time > m_date_time)
		{
		
			if (m_fd.is_open())
			{
				m_fd.flush();
				m_fd.close();
			}

			char log_name[1024] = {0};
			gen_log_file_name(log_name, m_path + "/", "rtc_native", ".log", ELogName_AutoDate, m_date_time);
			m_fd.open(log_name, std::ios::out | std::ios::trunc);
			if (!m_fd.is_open())
			{
				std::cout << "not open log file dest url = " << log_name << std::endl;
				//return false;
			}
		

		}
	}
	void casync_log::destroy()
	{
		
		m_stoped.store(true);

		m_condition.notify_all();
		
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		if (m_fd.is_open())
		{
			m_fd.flush();
			m_fd.close();
		}
		
		m_log_item.clear();
		 
	}
}//namespace chen