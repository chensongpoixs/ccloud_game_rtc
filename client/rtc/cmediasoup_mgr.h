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

#ifdef _MSC_VER
#define DLLIMPORT __declspec(dllimport)
#elif defined(__GNUC__) ||defined(__APPLE__)
 #define DLLIMPORT
#else
// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif
namespace cmediasoup
{
	typedef std::function<void(uint32_t status, uint32_t error_info)>     mediasoup_status_update_cb;



	class DLLIMPORT  cmediasoup_mgr
	{
	public:
		cmediasoup_mgr();
		~cmediasoup_mgr();
	public:
		bool init( );

		void startup(const char * mediasoupIp = "127.0.0.1", uint16_t mediasoupPort = 8888, const  char * roomName = "chensong", const char* clientName = "chensong"
			, uint32_t reconnectWaittime = 5);

		void destroy();

		void   webrtc_video(unsigned char * rgba_ptr, int width, int height);
		void   webrtc_video(unsigned char * y_ptr, unsigned char * uv_ptr,  int width, int height);
		void webrtc_texture(void * texture, int width, int height);
		void webrtc_pause();
		void webrtc_resume();
		bool webrtc_video_staus() const { return m_webrtc_pause; }
		bool  mediasoup_run();
		

		void set_mediasoup_status_callback(mediasoup_status_update_cb callback);
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
	};
}
#endif // C_MEDIASOUP_H