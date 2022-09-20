/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef C_WEBSOCKET_SESSION_H
#define C_WEBSOCKET_SESSION_H
#include <string>
namespace chen {
	
	class cwebsocket_session
	{
	public:
		cwebsocket_session();
		virtual	~cwebsocket_session();
		bool init();
		

		void reset();
		bool handshake(const std::string & host, const std::string & path);
		void send();
	public:
		void _async_read();
		void _async_write();
	private:

		cwebsocket_session(const cwebsocket_session&);
		//cnoncopyable &operator =(cnoncopyable &&);
		cwebsocket_session& operator=(const cwebsocket_session&);
	private:

	};

}
#endif // C_WEBSOCKET_SESSION_H