#include "desktop_capture_source.h"

#include "api/video/i420_buffer.h"
#include "api/video/video_rotation.h"
#include "rtc_base/logging.h"



void VideoCaptureSource::AddOrUpdateSink(
    rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
    const rtc::VideoSinkWants& wants) {
  broadcaster_.AddOrUpdateSink(sink, wants);
  UpdateVideoAdapter();
}
VideoCaptureSource* VideoCaptureSource::Create()
{
	 std::unique_ptr<VideoCaptureSource> Source_ptr(new VideoCaptureSource());
	 if (Source_ptr)
	 {
		 return Source_ptr.release();
	 }
	 return nullptr;
}
void VideoCaptureSource::RemoveSink(
    rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) {
  broadcaster_.RemoveSink(sink);
  UpdateVideoAdapter();
}

void VideoCaptureSource::UpdateVideoAdapter() {
  //video_adapter_.OnSinkWants(broadcaster_.wants());
	rtc::VideoSinkWants wants = broadcaster_.wants();
	video_adapter_.OnResolutionFramerateRequest(
		wants.target_pixel_count, wants.max_pixel_count, wants.max_framerate_fps);
}
void VideoCaptureSource::VideoOnFrame(const webrtc::VideoFrame& frame)
{
	OnFrame(frame);
}
void VideoCaptureSource::OnFrame(const webrtc::VideoFrame& frame) {
	int cropped_width = 0;
	int cropped_height = 0;
	int out_width = 0;
	int out_height = 0;

	if (!video_adapter_.AdaptFrameResolution(
		frame.width(), frame.height(), frame.timestamp_us() * 1000,
		&cropped_width, &cropped_height, &out_width, &out_height)) {
		// Drop frame in order to respect frame rate constraint.
		//RTC_LOG(LS_INFO) << "video adapter input failed !!!";
		return;
	}
	
	if (out_height != frame.height() || out_width != frame.width()) {
		// Video adapter has requested a down-scale. Allocate a new buffer and
		// return scaled version.
		// For simplicity, only scale here without cropping.
		rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
			webrtc::I420Buffer::Create(out_width, out_height);
		scaled_buffer->ScaleFrom(*frame.video_frame_buffer()->ToI420());
		webrtc::VideoFrame::Builder new_frame_builder =
			webrtc::VideoFrame::Builder()
			.set_video_frame_buffer(scaled_buffer)
			.set_timestamp_rtp(0)
			.set_timestamp_ms(rtc::TimeMillis())
			.set_rotation(webrtc::kVideoRotation_0)
			.set_timestamp_us(frame.timestamp_us())
			.set_id(frame.id());
		;
		 
		broadcaster_.OnFrame(new_frame_builder.build());
	} else {
		// No adaptations needed, just return the frame as is.

		broadcaster_.OnFrame(frame);
	}
}

