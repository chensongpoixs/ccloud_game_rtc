#ifndef _C_HTTP_MGR_H_
#define _C_HTTP_MGR_H_
#include <iostream>
#include <cstdlib>


namespace chen {
	

	class chttp_mgr
	{
	public:
		chttp_mgr() 
		: m_port(80)
		, m_host("localhost")
		, m_room_name("chensong")
		, m_http_appfix(""){}
		~chttp_mgr();
		bool init();
		void Destroy();


	public:

		bool sync_mediasoup_router_rtpcapabilities(std::string & result);


		bool sync_mediasoup_broadcasters(std::string data);
		//////////////////////////////////////////////////////////////////////////
		bool sync_mediasoup_create_transport(const std::string & broadcasters_id, std::string data, std::string & result);
		
		bool sync_mediasoup_connect_transport(const std::string & broadcasters_id, const std::string &  transport_id, std::string data, std::string & result);

		///////////////////////////////////////
		bool sync_mediasoup_producers_transport(const std::string & broadcasters_id, const std::string &  transport_id,  std::string data, std::string & result);
		bool sync_mediasoup_produce_data_transport(const std::string & broadcasters_id, const std::string &  transport_id, std::string data, std::string & result);
		bool sync_mediasoup_consume_data_transport(const std::string & broadcasters_id, const std::string &  transport_id, std::string data, std::string & result);
		//////////////////////////////////////////////
		bool sync_mediasoup_exit(const std::string & broadcasters_id);



		bool sync_mediasoup_all_dataproducers(std::string &result);
	private:
		chttp_mgr(const chttp_mgr&);
		chttp_mgr& operator =(const chttp_mgr&);
	private:
		uint32_t		m_port;
		std::string		m_host;
		std::string		m_room_name;
		std::string		m_http_appfix;
	};


	extern chttp_mgr g_http_mgr;
}

#endif /// _C_HTTP_MGR_H_