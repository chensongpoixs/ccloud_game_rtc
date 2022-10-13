/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
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
 
#include "cdataconsumer.h"
#include "csession_description.h"
#include "pc/video_track_source.h"
#include "third_party/libyuv/include/libyuv.h"
#include <thread>
#include <atomic>
#include "ccfg.h"
#include "NvCodec/nvenc.h"
 
namespace chen {
	 

	class ProxyVideoTrackSource : public webrtc::VideoTrackSource {
	public:
		static rtc::scoped_refptr<ProxyVideoTrackSource> Create()
		{ 
			std::unique_ptr<  VideoCaptureSource> video_source_ptr(VideoCaptureSource::Create( ));
			if (video_source_ptr)
			{
				 
				return new rtc::RefCountedObject<ProxyVideoTrackSource>(std::move(video_source_ptr));
			}
			return nullptr;
		}
		bool OnFrameTexture(void * texture, uint32 fmt, int32_t width, int32_t height)
		{
			if (!texture)
			{
				WARNING_EX_LOG("osg OnFrame texture nullptr !!!");
				return false;
			}
			if (!video_source_ptr)
			{
				WARNING_EX_LOG("capturer_ == nullptr ");
				return false;
			}
			if (width < 1 || height < 1)
			{
				WARNING_EX_LOG("osg  width = %d, height = %d ", width, height);
				return false;
			}

			if (!i420_buffer_.get() ||
				i420_buffer_->width() * i420_buffer_->height() < width * height)
			{
				i420_buffer_ = webrtc::I420Buffer::Create(width, height);
			}
			else if (i420_buffer_->width() * i420_buffer_->height() < width * height)
			{
				i420_buffer_ = webrtc::I420Buffer::Create(width, height);
			}
			i420_buffer_->set_texture(texture);
			g_dxgi_format = static_cast<DXGI_FORMAT>(fmt);
			NORMAL_EX_LOG("[fmt = %u][width = %u][height = %u]", fmt, width, height);
			//::memcpy(i420_buffer_->MutableDataY(), rgba, width * height * 4);
			//libyuv::ConvertToI420(rgba, 0, i420_buffer_->MutableDataY(),
			//	i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
			//	i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
			//	i420_buffer_->StrideV(), 0, 0, width, height, width,
			//	height, libyuv::kRotate0, libyuv::FOURCC_ARGB); // GL_BGRA，  FOURCC_BGRA 、、GL_BGR


																//chen::draw_font_func(i420_buffer_->MutableDataY(), i420_buffer_->MutableDataU(), i420_buffer_->MutableDataV(), "A", g_width, g_height, 3, 1, width , height);
			//NORMAL_EX_LOG("");
																// seting 马流的信息 
			{
				webrtc::VideoFrame captureFrame =
					webrtc::VideoFrame::Builder()
					.set_video_frame_buffer(i420_buffer_)
					.set_timestamp_rtp(0)
					.set_timestamp_ms(rtc::TimeMillis())
					.build();
				captureFrame.set_ntp_time_ms(0);

				video_source_ptr->VideoOnFrame(captureFrame);
			}

			return true;
		}
		bool OnFrame(unsigned char * rgba, uint32 fmt, int32_t width, int32_t height)
		{
			if (!rgba)
			{
				WARNING_EX_LOG("osg OnFrame rgba nullptr !!!");
				return false;
			}
			if (!video_source_ptr)
			{
				WARNING_EX_LOG("capturer_ == nullptr ");
				return false;
			}
			if (width < 1 || height < 1)
			{
				WARNING_EX_LOG("osg  width = %d, height = %d ", width, height);
				return false;
			}

			if (!i420_buffer_.get() ||
				i420_buffer_->width() * i420_buffer_->height() < width * height) 
			{
				i420_buffer_ = webrtc::I420Buffer::Create(width, height);
			}
			else if (i420_buffer_->width() * i420_buffer_->height() < width * height) 
			{
				i420_buffer_ = webrtc::I420Buffer::Create(width, height);
			}

			i420_buffer_->set_texture(NULL);
			g_dxgi_format = static_cast<DXGI_FORMAT>(fmt);
			NORMAL_EX_LOG("[fmt = %u][width = %u][height = %u]", fmt, width, height);
			::memcpy(i420_buffer_->MutableDataY(), rgba, width * height * 4);
			//libyuv::ConvertToI420(rgba, 0, i420_buffer_->MutableDataY(),
			//	i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
			//	i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
			//	i420_buffer_->StrideV(), 0, 0, width, height, width,
			//	height, libyuv::kRotate0, libyuv::FOURCC_ARGB); // GL_BGRA，  FOURCC_BGRA 、、GL_BGR


																//chen::draw_font_func(i420_buffer_->MutableDataY(), i420_buffer_->MutableDataU(), i420_buffer_->MutableDataV(), "A", g_width, g_height, 3, 1, width , height);

																// seting 马流的信息 
			{
				webrtc::VideoFrame captureFrame =
					webrtc::VideoFrame::Builder()
					.set_video_frame_buffer(i420_buffer_)
					.set_timestamp_rtp(0)
					.set_timestamp_ms(rtc::TimeMillis())
					.build();
				captureFrame.set_ntp_time_ms(0);
				
				 video_source_ptr-> VideoOnFrame(captureFrame);
			}

			return true;
		}


		bool OnFrame(unsigned char * y_ptr, uint32 fmt, unsigned char *uv_ptr, int32_t width, int32_t height)
		{
			if (!y_ptr )
			{
				WARNING_EX_LOG("osg OnFrame rgba nullptr !!!");
				return false;
			}
			if (!video_source_ptr)
			{
				WARNING_EX_LOG("capturer_ == nullptr ");
				return false;
			}
			if (width < 1 || height < 1)
			{
				WARNING_EX_LOG("osg  width = %d, height = %d ", width, height);
				return false;
			}

			if (!i420_buffer_.get() ||
				i420_buffer_->width() * i420_buffer_->height() < width * height)
			{
				i420_buffer_ = webrtc::I420Buffer::Create(width, height);
			}
			else if (i420_buffer_->width() * i420_buffer_->height() < width * height)
			{
				i420_buffer_ = webrtc::I420Buffer::Create(width, height);
			}
			g_dxgi_format = static_cast<DXGI_FORMAT>(fmt);
			NORMAL_EX_LOG("[fmt = %u][width = %u][height = %u]", fmt, width, height);
			::memcpy(i420_buffer_->MutableDataY(), y_ptr, width * height );
			::memcpy(i420_buffer_->MutableDataU(), uv_ptr, width * height/ 2);
			//NORMAL_EX_LOG("");
			//libyuv::ConvertToI420(rgba, 0, i420_buffer_->MutableDataY(),
			//	i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
			//	i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
			//	i420_buffer_->StrideV(), 0, 0, width, height, width,
			//	height, libyuv::kRotate0, libyuv::FOURCC_ARGB); // GL_BGRA，  FOURCC_BGRA 、、GL_BGR


																//chen::draw_font_func(i420_buffer_->MutableDataY(), i420_buffer_->MutableDataU(), i420_buffer_->MutableDataV(), "A", g_width, g_height, 3, 1, width , height);

																// seting 马流的信息 
			{
				webrtc::VideoFrame captureFrame =
					webrtc::VideoFrame::Builder()
					.set_video_frame_buffer(i420_buffer_)
					.set_timestamp_rtp(0)
					.set_timestamp_ms(rtc::TimeMillis())
					.build();
				captureFrame.set_ntp_time_ms(0);

				video_source_ptr->VideoOnFrame(captureFrame);
			}

			return true;
		}
	
		// YUV 
		bool OnFrame(const webrtc::VideoFrame & frame)
		{
			video_source_ptr->VideoOnFrame(frame);
			return true;
		}
	protected:
		explicit ProxyVideoTrackSource(
			std::unique_ptr< VideoCaptureSource> video_source)
			: VideoTrackSource(/*remote=*/false), video_source_ptr(std::move(video_source)) {}

	private:
		rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
			return video_source_ptr.get();
		}
		std::atomic_bool start_flag_  = false;

		rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
		//std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
		std::unique_ptr< VideoCaptureSource> video_source_ptr;
	};
}


#endif // !_C_CAPTURER_TRACKSOURCE_H_