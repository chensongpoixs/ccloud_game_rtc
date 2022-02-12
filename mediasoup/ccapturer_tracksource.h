#ifndef _C_CAPTURER_TRACKSOURCE_H_
#define _C_CAPTURER_TRACKSOURCE_H_

#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "desktop_capture_source.h"
#include "desktop_capture_source.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "api/video/i420_buffer.h"
#include "csend_transport.h"
#include "clog.h"
#include "sdptransform.hpp"
#include "sdp/Utils.hpp"
#include "ortc.hpp"
#include "csession_description.h"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>
#include <rtc_base/ssl_adapter.h>
#include "sdptransform.hpp"
#include "sdp/Utils.hpp"
#include "ortc.hpp"
#include "cproducer.h"
#include <memory>
#include "cclient.h"
#include "cdesktop_capture.h"
#include "cdataconsumer.h"
#include "csession_description.h"
#include "pc/video_track_source.h"

#include <thread>
#include <atomic>
#include "ccfg.h"
namespace chen {
	class CCapturerTrackSource : public webrtc::VideoTrackSource {
	public:
		static rtc::scoped_refptr<CCapturerTrackSource> Create() 
		{
			int32 fps = g_cfg.get_int32(ECI_Video_Fps);
			if (fps < 1)
			{
				fps = 30;
			}
			else if (fps > 60)
			{
				fps = 60;
			}
			std::unique_ptr<  DesktopCapture> capturer(DesktopCapture::Create(fps, 0));
			if (capturer)
			{
				capturer->StartCapture();
				return new
					rtc::RefCountedObject<CCapturerTrackSource>(std::move(capturer));
			}
			return nullptr;
		}
		void stop()
		{
			capturer_->StopCapture();
		}
	protected:
		explicit CCapturerTrackSource(
			std::unique_ptr< DesktopCapture> capturer)
			: VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

	private:
		rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
			return capturer_.get();
		}
		//std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
		std::unique_ptr< DesktopCapture> capturer_;
	};

}


#endif // !_C_CAPTURER_TRACKSOURCE_H_