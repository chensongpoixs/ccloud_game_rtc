#ifndef _C_SESSION_DESCRIPTION_H_
#define _C_SESSION_DESCRIPTION_H_
 
#include <future>
#include <api/jsep_session_description.h>
namespace chen {


	class cSetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
	{
	public:
		cSetSessionDescriptionObserver() = default;
		~cSetSessionDescriptionObserver() override = default;

		std::future<bool> GetFuture()
		{
			return  promise.get_future();
		}
		void Reject(const std::string& error)
		{
			promise.set_exception(std::make_exception_ptr(std::runtime_error(error.c_str())));
		}

		/* Virtual methods inherited from webrtc::SetSessionDescriptionObserver. */
	public:
		void OnSuccess() override
		{
			promise.set_value(true);
		}
		void OnFailure(webrtc::RTCError error) override
		{
			auto message = std::string(error.message());

			Reject(message);
		}

	private:
		std::promise<bool> promise;
	};


	class CreateSessionDescriptionObserver : public webrtc::CreateSessionDescriptionObserver
	{
	public:
		CreateSessionDescriptionObserver() = default;
		~CreateSessionDescriptionObserver() override = default;

		std::future<std::string> GetFuture();
		void Reject(const std::string& error);

		/* Virtual methods inherited from webrtc::CreateSessionDescriptionObserver. */
	public:
		void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
		void OnFailure(webrtc::RTCError error) override;

	private:
		std::promise<std::string> promise;
	};
}

#endif // !_C_SESSION_DESCRIPTION_H_