#include "csession_description.h"
#include "clog.h"
namespace chen {

	/* CreateSessionDescriptionObserver */

	std::future<std::string> CreateSessionDescriptionObserver::GetFuture()
	{
	 

		return this->promise.get_future();
	}

	void  CreateSessionDescriptionObserver::Reject(const std::string& error)
	{
	 

		this->promise.set_exception(std::make_exception_ptr( (error.c_str())));
	}

	void CreateSessionDescriptionObserver::OnSuccess(
		webrtc::SessionDescriptionInterface* desc)
	{
		

		// This callback should take the ownership of |desc|.
		std::unique_ptr<webrtc::SessionDescriptionInterface> ownedDesc(desc);

		std::string sdp;

		ownedDesc->ToString(&sdp);
		this->promise.set_value(sdp);
	};

	void CreateSessionDescriptionObserver::OnFailure(webrtc::RTCError error)
	{
		

		/*WARNING_EX_LOG(
			"webtc::CreateSessionDescriptionObserver failure [%s:%s]",
			webrtc::ToString(error.type()),
			error.message());*/

		auto message = std::string(error.message());

		this->Reject(message);
	}

}