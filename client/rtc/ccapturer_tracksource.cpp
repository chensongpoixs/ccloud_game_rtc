/***********************************************************************************************
					created: 		2018-10-02

					author:			chensong

					purpose:		ccapturer_tracksource
************************************************************************************************/

#include "ccapturer_tracksource.h"


namespace  chen {

//     rtc::scoped_refptr<ProxyVideoTrackSource> ProxyVideoTrackSource::Create()
//		{
//			std::unique_ptr<  VideoCaptureSource> video_source_ptr(VideoCaptureSource::Create( ));
//			if (video_source_ptr)
//			{
//
//				return new rtc::RefCountedObject<ProxyVideoTrackSource>(std::move(video_source_ptr));
//			}
//			return nullptr;
//		}
		bool ProxyVideoTrackSource::OnFrameTexture(void * texture, int32_t width, int32_t height)
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
            // TODO@chensong 20220617 --->
//			i420_buffer_->set_texture(texture);

			//::memcpy(i420_buffer_->MutableDataY(), rgba, width * height * 4);
			//libyuv::ConvertToI420(rgba, 0, i420_buffer_->MutableDataY(),
			//	i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
			//	i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
			//	i420_buffer_->StrideV(), 0, 0, width, height, width,
			//	height, libyuv::kRotate0, libyuv::FOURCC_ARGB); // GL_BGRA，  FOURCC_BGRA 、、GL_BGR


																//chen::draw_font_func(i420_buffer_->MutableDataY(), i420_buffer_->MutableDataU(), i420_buffer_->MutableDataV(), "A", g_width, g_height, 3, 1, width , height);
			NORMAL_EX_LOG("");
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
		bool ProxyVideoTrackSource::OnFrame(unsigned char * rgba, int32_t width, int32_t height)
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


		bool ProxyVideoTrackSource::OnFrame(unsigned char * y_ptr, unsigned char *uv_ptr, int32_t width, int32_t height)
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

			NORMAL_EX_LOG("");
			::memcpy(i420_buffer_->MutableDataY(), y_ptr, width * height );
			::memcpy(i420_buffer_->MutableDataU(), uv_ptr, width * height/ 2);
			NORMAL_EX_LOG("");
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
		bool ProxyVideoTrackSource::OnFrame(const webrtc::VideoFrame & frame)
		{
			video_source_ptr->VideoOnFrame(frame);
			return true;
		}


//		rtc::VideoSourceInterface<webrtc::VideoFrame>* ProxyVideoTrackSource::source() override
//        {
//			return video_source_ptr.get();
//		}
}