#include "cmediasoup_mgr.h"
#include "cclient.h"
#include "clog.h"
namespace cmediasoup
{
	using namespace syz;
	cmediasoup_mgr::cmediasoup_mgr()
		: m_init(false)
		, m_webrtc_pause(false)
	{
	}
	cmediasoup_mgr::~cmediasoup_mgr()
	{
	}
	bool cmediasoup_mgr::init(const char * file_name)
	{
		//if (!m_stoped)
		m_init = s_client.init(file_name);
		return m_init;
	}
	void cmediasoup_mgr::startup()
	{
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
		s_client.Loop();
	}
}