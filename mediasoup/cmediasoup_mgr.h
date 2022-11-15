/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_MEDIASOUP_H
#define C_MEDIASOUP_H
#include <thread>
#include <string>
#include <functional>
#include "cmediasoup_input_device_event.h"
namespace cmediasoup
{
	typedef std::function<void(uint32_t status, uint32_t error_info)>     mediasoup_status_update_cb;
	typedef std::function<void(MEvent)> mediasoup_input_device_event_cb;


	class  __declspec(dllimport) cmediasoup_mgr
	{
	public:
		cmediasoup_mgr();
		~cmediasoup_mgr();

	public:
		static bool global_init();
		static void global_destroy();

	public:
		bool init( uint32_t gpu_index);

		bool startup(const char * mediasoupIp = "127.0.0.1", uint16_t mediasoupPort = 8888, const  char * roomName = "chensong", const char* clientName = "chensong"
			, uint32_t reconnectWaittime = 5);

		void destroy();
		/// <summary>
		/// 
		/// </summary>
		/// <param name="rgba_ptr"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void   webrtc_video(unsigned char * rgba_ptr /*DXGI_FORMAT_B8G8R8A8_UNORM*/, uint32_t fmt,  int width, int height);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="y_ptr"></param>
		/// <param name="uv_ptr"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		void   webrtc_video(unsigned char * y_ptr, unsigned char * uv_ptr, uint32_t fmt, int width, int height);
		void webrtc_texture(void * texture, uint32_t fmt, int width, int height);
		void webrtc_pause();
		void webrtc_resume();
		bool webrtc_video_staus() const { return m_webrtc_pause; }
		bool  mediasoup_run();
		
		/**
		*  status ：  1 ： websocket connect 成功  
		*			  4 ： 加入房间
		*			  8 ： 视频通道 
		*    
		* 
		*  error ： 0 ： 成功    1 ： 失败
		* 
		* 
		* 
		*/
		void set_mediasoup_status_callback(mediasoup_status_update_cb callback);

		void set_mediasoup_input_device_event_callback(mediasoup_input_device_event_cb callback);
	private:
		void _mediasoup_thread();
	private:
		cmediasoup_mgr(const cmediasoup_mgr&);
		cmediasoup_mgr& operator =(const cmediasoup_mgr&);
	private:
		bool				m_init ;
		std::thread			m_thread;
		bool				m_webrtc_pause;
		std::string			m_mediasoup_ip;
		uint16_t			m_mediasoup_port;
		std::string			m_room_name;
		std::string			m_client_name;
		uint32_t			m_reconnect_wait;
		void*				m_client_ptr;
	};
}
#endif // C_MEDIASOUP_H