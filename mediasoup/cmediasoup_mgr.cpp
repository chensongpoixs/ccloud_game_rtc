#include "cmediasoup_mgr.h"
#include "cclient.h"
#include "clog.h"
namespace cmediasoup
{
	using namespace chen;
	cmediasoup_mgr::cmediasoup_mgr()
		: m_init(false)
		, m_webrtc_pause(false)
	{
	}
	cmediasoup_mgr::~cmediasoup_mgr()
	{
	}
	bool cmediasoup_mgr::init( )
	{
		//if (!m_stoped)
		m_init = s_client.init( );
		return m_init;
	}
	void cmediasoup_mgr::startup(const char* mediasoupIp, uint16_t port
		, const char* roomName, const char* clientName
		, uint32_t reconnect_waittime)
	{
		m_mediasoup_ip = mediasoupIp;
		m_mediasoup_port = port;
		m_room_name = roomName;
		m_client_name = clientName;
		m_reconnect_wait = reconnect_waittime;
		m_thread = std::thread(&cmediasoup_mgr::_mediasoup_thread, this);
	}
	void cmediasoup_mgr::destroy()
	{
		s_client.stop();
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		s_client.Destory();
		m_init  = false;
	}

	void cmediasoup_mgr::webrtc_video(unsigned char * rgba_ptr, int width, int height)
	{
		if (!m_init)
		{
			
			WARNING_EX_LOG("mediasoup_mgr  not init !!!");
			return;
		}
		if (m_webrtc_pause)
		{
			return;
		}
		s_client.webrtc_video(rgba_ptr, width, height);
	}
	void cmediasoup_mgr::webrtc_pause()
	{
		m_webrtc_pause = true;
	}
	void cmediasoup_mgr::webrtc_resume()
	{
		m_webrtc_pause = false;
	}
	void cmediasoup_mgr::set_mediasoup_status_callback(mediasoup_status_update_cb callback)
	{
		s_client.set_mediasoup_status_callback(callback);
	}
	bool cmediasoup_mgr::mediasoup_run()
	{
		if (!m_init)
		{
			 
			WARNING_EX_LOG("mediasoup_mgr  not init !!!");
			return false;
		}

		return s_client.webrtc_run();
		//return true;
	}

	void cmediasoup_mgr::_mediasoup_thread()
	{
		printf("[info]mediasoupip = %s, port = %u, roomname = %s, client_name = %s, reconnectwiat = %u\n", m_mediasoup_ip.c_str(),
			m_mediasoup_port, m_room_name.c_str(), m_client_name.c_str(), m_reconnect_wait);
		s_client.Loop(m_mediasoup_ip, m_mediasoup_port, m_room_name, m_client_name, m_reconnect_wait);
		printf("[%s][%d] mediasoup_thread exit !!! \n", __FUNCTION__, __LINE__);
	}
}