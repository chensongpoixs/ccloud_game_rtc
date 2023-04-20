/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef _C_RTC_CLIENT_H_
#define _C_RTC_CLIENT_H_
#include "ctransport.h"

#include <string>
#include "crtc_publisher.h"
#include "cwebsocket_mgr.h"
#include "csingleton.h"
namespace chen {

	enum ERtc_Type
	{
		ERtc_None = 0,
		ERtc_WebSocket_Init,

		//EMediasoup_Produce_Video_Tr
		///////////////////////////////////////////////////////////
		ERtc_WebSocket,
		ERtc_WebSocket_Close,
		ERtc_WebSocket_Destory,
		ERtc_WebSocket_Wait,
		//////////////////////////////////////////

		ERtc_Reset, //
		ERtc_Destory,
		ERtc_Wait,
		ERtc_Exit,
		//EMediasoup_
	};
	class crtc_client  : public crtc_publisher::clistener
	{
	public:
		explicit crtc_client();
	    	~crtc_client();
	public:
		bool init(uint32 gpu_index);
		void Loop(const std::string& rtc_ip, uint16_t rtc_port, const std::string& roomName, const std::string& clientName
			, uint32_t reconnect_waittime);
		void destroy();

		void stop();

		bool  rtc_texture(void * texture, uint32 fmt, int32_t width, int32_t height);
	public :
		virtual void send_create_offer_sdp(const std::string & sdp, bool create = true);
	private:
		void _presssmsg(std::list<std::string> & msgs);
	protected:
	private:
		bool													m_stoped;
		ERtc_Type												m_status;
		cwebsocket_mgr											m_websocket_mgr;
		rtc::scoped_refptr < chen::crtc_publisher>				m_rtc_publisher;
		std::string												m_room_name;
		std::string												m_user_name;
	};

#define  s_rtc_client chen::csingleton<crtc_client>::get_instance()
}

#endif //_C_RTC_CLIENT_H_