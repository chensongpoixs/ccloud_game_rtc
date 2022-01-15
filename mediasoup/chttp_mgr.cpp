#include "chttp_mgr.h"
#include "ccfg.h"
#include "httplib.h"
#include  "Broadcaster.hpp"
namespace webrtc {
	chttp_mgr g_http_mgr;

	
	chttp_mgr::~chttp_mgr()
	{

	}


	bool chttp_mgr::init()
	{
		m_host = g_cfg.get_string(webrtc::ECI_MediaSoup_Host);
		m_port = g_cfg.get_uint32(webrtc::ECI_MediaSoup_Http_Port);
		m_room_name = g_cfg.get_string(ECI_Room_Name);
		m_http_appfix = "https://" + m_host + ":" + std::to_string(m_port) + "/rooms/" + m_room_name;

		return true;
	}
	void chttp_mgr::Destroy()
	{

	}
	bool chttp_mgr::sync_mediasoup_router_rtpcapabilities(std::string & result)
	{
		httplib::Client cli(m_host, m_port); 
		httplib::Result res = cli.Get(m_http_appfix.c_str() );
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!! router_rtpcapabilities\n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to router_rtpcapabilities"
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		result = res->body;
		return true;
	}
	bool chttp_mgr::sync_mediasoup_broadcasters(std::string data)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/broadcasters";

		httplib::Result res = cli.Post(url.c_str(), data, "application/json");
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!! create Broadcaster\n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to create Broadcaster"
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		
		return true;
	}


	bool chttp_mgr::sync_mediasoup_create_transport(const std::string & broadcasters_id, std::string data, std::string & result)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/broadcasters/" +broadcasters_id+ "/transports" ;

		httplib::Result res = cli.Post(url.c_str(), data, "application/json");
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!! create mediasoup WebRtcTransport\n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to create mediasoup WebRtcTransport"
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		result = res->body;
		return true;
	}
	bool chttp_mgr::sync_mediasoup_connect_transport(const std::string & broadcasters_id, const std::string & transport_id, std::string data, std::string & result)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/broadcasters/" + broadcasters_id + "/transports/" + transport_id + "/connect";
	
		httplib::Result res = cli.Post(url.c_str(), data, "application/json");
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!!\n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to connect  transport"
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		} 
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		result = res->body;
		return true;
	}

	bool chttp_mgr::sync_mediasoup_producers_transport(const std::string & broadcasters_id, const std::string &  transport_id, std::string data, std::string & result)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/broadcasters/" + broadcasters_id + "/transports/" + transport_id + "/producers";

		httplib::Result res = cli.Post(url.c_str(), data, "application/json");
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!!\n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to    producers"
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		result = res->body;
		return true;
	}
	bool chttp_mgr::sync_mediasoup_produce_data_transport(const std::string & broadcasters_id, const std::string &  transport_id, std::string data, std::string & result)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/broadcasters/" + broadcasters_id + "/transports/" + transport_id + "/produce/data";

		httplib::Result res = cli.Post(url.c_str(), data, "application/json");
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!!\n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to    /produce/data"
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		result = res->body;
		return true;
	}


	bool chttp_mgr::sync_mediasoup_consume_data_transport(const std::string & broadcasters_id, const std::string &  transport_id, std::string data, std::string & result)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/broadcasters/" + broadcasters_id + "/transports/" + transport_id + "/consume/data";

		httplib::Result res = cli.Post(url.c_str(), data, "application/json");
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!! /consume/data\n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to    /consume/data"
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		result = res->body;
		return true;
	}
	bool chttp_mgr::sync_mediasoup_exit(const std::string & broadcasters_id)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/broadcasters/" + broadcasters_id  ;

		httplib::Result res = cli.Get(url.c_str() );
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!! exit \n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to   exit "
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
	 
		return true;
	}

	bool chttp_mgr::sync_mediasoup_all_dataproducers(std::string &result)
	{
		httplib::Client cli(m_host, m_port);
		std::string url = m_http_appfix + "/AllDataProducers";

		httplib::Result res = cli.Get(url.c_str());
		if (!res)
		{
			RTC_LOG(LS_ERROR) << "[error] !!!! exit \n";
			return  false;
		}
		if (res->status != 200)
		{
			RTC_LOG(LS_ERROR) << "[ERROR] unable to   exit "
				<< " [status code:" << res->status << ", body:\"" << res->body << "\"]";
			return false;
		}
		//RTC_LOG(INFO) << __FUNCTION__ << __LINE__ << "[" << res->body << "]";
		result = res->body;
		return true;
	}
}