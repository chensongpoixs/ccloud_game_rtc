#include "cwebsocket_session.h"


namespace chen {
	cwebsocket_session::cwebsocket_session()
	{}
	cwebsocket_session::~cwebsocket_session()
	{}
	bool cwebsocket_session::init()
	{
		return true;
	}


	void cwebsocket_session::reset()
	{}
	bool cwebsocket_session::handshake(const std::string & host, const std::string & path)
	{
		return true;
	}
	void cwebsocket_session::send()
	{}
 
	void cwebsocket_session::_async_read()
	{}
	void cwebsocket_session::_async_write()
	{}
}