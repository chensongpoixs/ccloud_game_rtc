/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#include "crtc_media_mgr.h"
#include "crtc_client.h"
namespace chen {
	crtc_media_mgr::crtc_media_mgr()
		: m_init(false)
		, m_rtc_pause(false)
	{
	}
	crtc_media_mgr::~crtc_media_mgr()
	{
	}
	bool crtc_media_mgr::init(uint32_t gpu_index)
	{
		m_init = s_rtc_client.init(gpu_index);
		return m_init;
	}
	void crtc_media_mgr::startup(const char * rtcIp, uint16_t rtcPort, const char * roomName, const char * clientName, uint32_t reconnectWaittime)
	{
		m_rtc_ip = rtcIp;
		m_rtc_port = rtcPort;
		m_room_name = roomName;
		m_client_name = clientName;
		m_reconnect_wait = reconnectWaittime;
		m_thread = std::thread(&crtc_media_mgr::_rtc_thread, this);
	}
	void crtc_media_mgr::destroy()
	{
		s_rtc_client.stop();
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		s_rtc_client.destroy();
		m_init = false;
	}
	void crtc_media_mgr::rtc_video(unsigned char * rgba_ptr, uint32_t fmt, int width, int height)
	{
	}
	void crtc_media_mgr::rtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32_t fmt, int width, int height)
	{
	}
	void crtc_media_mgr::rtc_texture(void * texture, uint32_t fmt, int width, int height)
	{
		s_rtc_client.rtc_texture(texture, fmt, width, height);
	}
	void crtc_media_mgr::rtc_pause()
	{
	}
	void crtc_media_mgr::rtc_resume()
	{
	}
	void crtc_media_mgr::set_rtc_status_callback(rtc_status_update_cb callback)
	{
	}
	void crtc_media_mgr::_rtc_thread()
	{
		SYSTEM_LOG("[info]rtcip = %s, port = %u, roomname = %s, client_name = %s, reconnectwiat = %u\n", m_rtc_ip.c_str(),
			m_rtc_port, m_room_name.c_str(), m_client_name.c_str(), m_reconnect_wait);
		s_rtc_client.Loop(m_rtc_ip, m_rtc_port, m_room_name, m_client_name, m_reconnect_wait);
		SYSTEM_LOG("[%s][%d] rtc_thread exit !!! \n", __FUNCTION__, __LINE__);
	}
}