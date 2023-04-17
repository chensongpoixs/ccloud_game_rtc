/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_RTC_MGR_H
#define C_RTC_MGR_H
#include <thread>
#include <string>
#include <functional>
#if defined(_MSC_VER)
#include <Windows.h>
#endif
namespace chen
{
	typedef std::function<void(uint32_t status, uint32_t error_info)>     rtc_status_update_cb;



	class   __declspec(dllimport)  crtc_media_mgr
	{
	public:
		crtc_media_mgr();
		~crtc_media_mgr();
	public:
		bool init(uint32_t gpu_index);

		void startup(const char * rtcIp = "127.0.0.1", uint16_t rtcPort = 8888, const  char * roomName = "chensong", const char* clientName = "chensong"
			, uint32_t reconnectWaittime = 5);

		void destroy();
		/// <summary>
		/// 
		/// </summary>
		/// <param name="rgba_ptr"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void    rtc_video(unsigned char * rgba_ptr /*DXGI_FORMAT_B8G8R8A8_UNORM*/, uint32_t fmt, int width, int height);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="y_ptr"></param>
		/// <param name="uv_ptr"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void    rtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32_t fmt, int width, int height);
		void  rtc_texture(void * texture, uint32_t fmt, int width, int height);
		void  rtc_pause();
		void  rtc_resume();
		//bool  rtc_video_staus() const { return m_webrtc_pause; }
		//bool  mediasoup_run();

#if defined(_MSC_VER)


		//void set_main_window(HWND win);
#endif // #if defined(_MSC_VER)
		void set_rtc_status_callback(rtc_status_update_cb callback);
	private:
		void _rtc_thread();
	private:
		crtc_media_mgr(const crtc_media_mgr&);
		crtc_media_mgr& operator =(const crtc_media_mgr&);
	private:
		bool				m_init;
		std::thread			m_thread;
		bool				m_rtc_pause;
		std::string			m_rtc_ip;
		uint16_t			m_rtc_port;
		std::string			m_room_name;
		std::string			m_client_name;
		uint32_t			m_reconnect_wait;
	};
}
#endif // C_MEDIASOUP_H