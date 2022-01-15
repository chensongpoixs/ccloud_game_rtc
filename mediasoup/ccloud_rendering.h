#ifndef _C_CLOUD_RENDERING_H_
#define _C_CLOUD_RENDERING_H_

#include "Broadcaster.hpp"
#include "cwebsocket_mgr.h"

namespace webrtc {

	class ccloud_rendering
	{
	public:
		ccloud_rendering()
			: m_broadcaster()
			, m_websocket_mgr()
			, m_stoped(false)
			/*, m_frls("./log", "webrtc_log", 1024, 2)*/{}
		~ccloud_rendering();

	public:
		bool init(const char * config_name);
		bool Loop();
		void Destroy();


		
	private:
		ccloud_rendering(const ccloud_rendering&);
		ccloud_rendering& operator =(const ccloud_rendering&); 
	private:
		Broadcaster		m_broadcaster;
		cwebsocket_mgr	m_websocket_mgr;
		bool			m_stoped;
		//rtc::FileRotatingLogSink m_frls;
	};

}

#endif // 