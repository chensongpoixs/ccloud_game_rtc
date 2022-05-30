#include "desktop_capture_source.h"

#include "api/video/i420_buffer.h"
#include "api/video/video_rotation.h"
#include "rtc_base/logging.h"
#include <chrono>
#include "third_party/libyuv/include/libyuv.h"
#include "clog.h"

namespace chen {

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
void VideoCaptureSource::OnFrame(const webrtc::VideoFrame& frame) 
{
	broadcaster_.OnFrame(frame);
	return;


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
	
	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	if (out_height != frame.height() || out_width != frame.width()) {
		// Video adapter has requested a down-scale. Allocate a new buffer and
		// return scaled version.
		// For simplicity, only scale here without cropping.
		/*
		
		 GPU d%
			

		*/



		rtc::scoped_refptr<webrtc::I420Buffer> yuv_scaled_buffer =
			webrtc::I420Buffer::Create(frame.width(), frame.height());
		
		libyuv::ConvertToI420(frame.video_frame_buffer()->ToI420()->DataY(), 0, yuv_scaled_buffer->MutableDataY(),
			yuv_scaled_buffer->StrideY(), yuv_scaled_buffer->MutableDataU(),
			yuv_scaled_buffer->StrideU(), yuv_scaled_buffer->MutableDataV(),
			yuv_scaled_buffer->StrideV(), 0, 0, frame.width(), frame.height(), frame.width(),
			frame.height(), libyuv::kRotate0, libyuv::FOURCC_ARGB); // GL_BGRA，  FOURCC_BGRA 、、GL_BGR

		rtc::scoped_refptr<webrtc::I420Buffer> scaled_buffer =
			webrtc::I420Buffer::Create(out_width, out_height);
		
		scaled_buffer->ScaleFrom(*yuv_scaled_buffer->ToI420());

		rtc::scoped_refptr<webrtc::I420Buffer> argb_scaled_buffer =
			webrtc::I420Buffer::Create(out_width, out_height);

		libyuv::I420ToARGB(scaled_buffer->DataY(), scaled_buffer->StrideY(), 
			scaled_buffer->DataU(), scaled_buffer->StrideU(),
			scaled_buffer->DataV(), scaled_buffer->StrideV(),
			argb_scaled_buffer->MutableDataY(), out_width * 4, out_width, out_height
			);


		webrtc::VideoFrame::Builder new_frame_builder =
			webrtc::VideoFrame::Builder()
			.set_video_frame_buffer(argb_scaled_buffer)
			.set_timestamp_rtp(0)
			.set_timestamp_ms(rtc::TimeMillis())
			.set_rotation(webrtc::kVideoRotation_0)
			.set_timestamp_us(frame.timestamp_us())
			.set_id(frame.id());
		;
		std::chrono::steady_clock::time_point scaled_end_time = std::chrono::steady_clock::now();

		std::chrono::steady_clock::duration diff = scaled_end_time - start_time;
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		if (ms.count())
		{
			WARNING_EX_LOG("scaled ms = %u", ms.count());
		}
		broadcaster_.OnFrame(new_frame_builder.build());
		std::chrono::steady_clock::time_point broadcasster_frame = std::chrono::steady_clock::now();
		diff = broadcasster_frame - scaled_end_time;
		ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		if (ms.count() > 3)
		{
			WARNING_EX_LOG("broadcasster_frame ms = %u", ms.count());
		}
	} else {
		// No adaptations needed, just return the frame as is.
		
		broadcaster_.OnFrame(frame);
		std::chrono::steady_clock::time_point broadcasster_frame = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration diff = broadcasster_frame - start_time;
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		if (ms.count() > 3)
		{
			WARNING_EX_LOG("broadcasster_frame ms = %u", ms.count());
		}
	}
}


}