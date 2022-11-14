#include "cmediasoup_mgr.h"
#include "cclient.h"
#include "clog.h"
#include "cinput_device.h"
#include "clog.h"
#include "cclient.h"
#include "mediasoupclient.hpp"
#include "ortc.hpp"
#include "clog.h"
#include "ccfg.h"
#include "cwebsocket_mgr.h"
#include <iostream>
#include <io.h>
#define _CRT_SECURE_NO_WARNINGS
#include "cdevice.h"
#include "pc/video_track_source.h"
#include "crecv_transport.h"
#include "csend_transport.h"
#include "cinput_device.h"
#include "NvCodec/nvenc.h"
#include "build_version.h"
#include "cdesktop_capture.h"
namespace cmediasoup
{
	using namespace chen;


	static void check_file(const char* file_name)
	{

		if (::_access(file_name, 0) != 0)
		{
			FILE* fp = ::fopen(file_name, "wb+");
			if (!fp)
			{
				// return;
				return;
			}
			::fclose(fp);
			fp = NULL;
		}
	}

	cmediasoup_mgr::cmediasoup_mgr()
		: m_init(false)
		, m_webrtc_pause(false)
		, m_client_ptr(NULL)
	{
	}
	cmediasoup_mgr::~cmediasoup_mgr()
	{
	}
	bool cmediasoup_mgr::global_init()
	{
		using namespace chen;
		printf("Log init ...\n");
		if (!LOG::init(ELogStorageScreenFile))
		{
			std::cerr << " log init failed !!!";
			return false;
		}
	//	show_work_dir();
		//SYSTEM_LOG("git:branch:%s", BUILD_GIT_BRANCH_NAME);
		//SYSTEM_LOG("git:version:%u", BUILD_GIT_REVERSION);
		//SYSTEM_LOG("git:branch_hash:%s", BUILD_GIT_HASH);
		//SYSTEM_LOG("git:BUILD_TIME:%s", BUILD_TIME);
		SYSTEM_LOG("Log init ...\n");
		g_gpu_index = 0;
		g_dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
		SYSTEM_LOG("gpu index = %u", g_gpu_index);
		static const   char* config_file = "client.cfg";
		check_file(config_file);
		bool init = g_cfg.init(config_file);
		if (!init)
		{
			//	//RTC_LOG(LS_ERROR) << "config init failed !!!" << config_name;
			ERROR_EX_LOG("config init failed !!! config_name = %s", config_file);
			return false;
		}
		SYSTEM_LOG("config init ok !!!");
		// set log level
		LOG::set_level(static_cast<ELogLevelType>(g_cfg.get_uint32(ECI_LogLevel)));

		SYSTEM_LOG("set level = %u", g_cfg.get_uint32(ECI_LogLevel));

		mediasoupclient::Initialize();
		return true;
	}
	void cmediasoup_mgr::global_destroy()
	{
		mediasoupclient::Cleanup();
		SYSTEM_LOG("mediasoup destroy ok !!!");
		LOG::destroy();
	}
	bool cmediasoup_mgr::init(uint32_t gpu_index)
	{
		//if (!m_stoped)
		if (m_client_ptr)
		{
			return false;
		}
		cclient * client_ptr = new cclient();
		if (!client_ptr)
		{
			return false;
		}
		m_init = client_ptr->init(gpu_index);
		m_client_ptr = client_ptr;
		return m_init;
	}
	bool cmediasoup_mgr::startup(const char* mediasoupIp, uint16_t port
		, const char* roomName, const char* clientName
		, uint32_t reconnect_waittime)
	{
		if (!m_client_ptr)
		{
			WARNING_EX_LOG("not find client ptr !!!");
			return false;
		}
		m_mediasoup_ip = mediasoupIp;
		m_mediasoup_port = port;
		m_room_name = roomName;
		m_client_name = clientName;
		m_reconnect_wait = reconnect_waittime;
		m_thread = std::thread(&cmediasoup_mgr::_mediasoup_thread, this);
		return true;
	}
	void cmediasoup_mgr::destroy()
	{
		if (!m_client_ptr)
		{
			return;
		}
		cclient* client_ptr = static_cast<cclient * >(m_client_ptr);
		client_ptr->stop();
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		client_ptr->Destory();
		delete client_ptr;
		client_ptr = NULL;
		m_client_ptr = NULL;
		m_init  = false;
	}

	void cmediasoup_mgr::webrtc_video(unsigned char * rgba_ptr, uint32_t fmt, int width, int height)
	{
		if (!m_init|| !m_client_ptr)
		{
			
			//WARNING_EX_LOG("mediasoup_mgr  not init !!!");
			return;
		}
		if (m_webrtc_pause)
		{
			return;
		}
		cclient* client_ptr = static_cast<cclient*>(m_client_ptr);
		client_ptr->webrtc_video(rgba_ptr, fmt, width, height);
	}
	void cmediasoup_mgr::webrtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32_t fmt, int width, int height)
	{
		if (!m_init || !m_client_ptr)
		{

			//WARNING_EX_LOG("mediasoup_mgr  not init !!!");
			return;
		}
		if (m_webrtc_pause)
		{
			return;
		}
		NORMAL_EX_LOG("");
		cclient* client_ptr = static_cast<cclient*>(m_client_ptr);
		client_ptr->webrtc_video(y_ptr, uv_ptr, fmt, width, height);
	}
	void cmediasoup_mgr::webrtc_texture(void * texture, uint32_t fmt, int width, int height)
	{
		if (!m_init || !m_client_ptr)
		{

			//WARNING_EX_LOG("mediasoup_mgr  not init !!!");
			return;
		}
		if (m_webrtc_pause)
		{
			return;
		}
		NORMAL_EX_LOG("");
		cclient* client_ptr = static_cast<cclient*>(m_client_ptr);
		client_ptr->webrtc_texture(texture, fmt, width, height);
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
		if (!m_client_ptr)
		{
			return;
		}
		cclient* client_ptr = static_cast<cclient*>(m_client_ptr);
		
		client_ptr->set_mediasoup_status_callback(callback);
	}
	void cmediasoup_mgr::set_mediasoup_input_device_event_callback(mediasoup_input_device_event_cb callback)
	{

		//set_mediasoup_input_device_callback
		if (!m_client_ptr)
		{
			return;
		}
		cclient* client_ptr = static_cast<cclient*>(m_client_ptr);

		client_ptr->set_mediasoup_input_device_callback(callback);
	}
	bool cmediasoup_mgr::mediasoup_run()
	{
		if (!m_init || !m_client_ptr)
		{
			 
			WARNING_EX_LOG("mediasoup_mgr  not init !!!");
			return false;
		}
		cclient* client_ptr = static_cast<cclient*>(m_client_ptr);

		return client_ptr->webrtc_run();
		//return true;
	}

	void cmediasoup_mgr::_mediasoup_thread()
	{
		if (!m_client_ptr)
		{
			ERROR_EX_LOG("[info]mediasoupip = %s, port = %u, roomname = %s, client_name = %s, reconnectwiat = %u\n", m_mediasoup_ip.c_str(),
				m_mediasoup_port, m_room_name.c_str(), m_client_name.c_str(), m_reconnect_wait);

			return;
		}
		SYSTEM_LOG("[info]mediasoupip = %s, port = %u, roomname = %s, client_name = %s, reconnectwiat = %u\n", m_mediasoup_ip.c_str(),
			m_mediasoup_port, m_room_name.c_str(), m_client_name.c_str(), m_reconnect_wait);
		cclient* client_ptr = static_cast<cclient*>(m_client_ptr);
		client_ptr->Loop(m_mediasoup_ip, m_mediasoup_port, m_room_name, m_client_name, m_reconnect_wait);
		SYSTEM_LOG("[%s][%d] mediasoup_thread exit !!! \n", __FUNCTION__, __LINE__);
	}
}