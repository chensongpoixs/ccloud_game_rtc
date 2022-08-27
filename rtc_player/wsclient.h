#ifndef __WSCLIENT_HPP_
#define __WSCLIENT_HPP_

#include <string>
#include <vector>

namespace wsclient {

class WebSocketCallback {
public:
	virtual void OnMessage(const std::string& message) {}
	virtual void OnMessage(const std::vector<uint8_t>& message) {}
	virtual void OnClose() {}
};

class WebSocket {
  public:
    typedef WebSocket * pointer;
    typedef enum readyStateValues { CLOSING, CLOSED, CONNECTING, OPEN } readyStateValues;

    // Factories:
    static pointer create_dummy();
    static pointer from_url(const std::string& url, const std::string& origin = std::string());
    static pointer from_url_no_mask(const std::string& url, const std::string& origin = std::string());

    // Interfaces:
    virtual ~WebSocket() { }
    virtual void poll(int timeout = 0) = 0; // timeout in milliseconds
    virtual void send(const std::string& message) = 0;
	virtual void send(const char* message) = 0;
    virtual void sendBinary(const std::string& message) = 0;
    virtual void sendBinary(const std::vector<uint8_t>& message) = 0;
	virtual void sendBinary(const uint8_t* message, size_t len) = 0;

    virtual void sendPing() = 0;
    virtual void close() = 0;
    virtual readyStateValues getReadyState() const = 0;

    virtual void dispatch(WebSocketCallback* callable) = 0;
};

} // namespace wsclient

#endif /* __WSCLIENT_HPP_ */
