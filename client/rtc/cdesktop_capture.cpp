#include "cdesktop_capture.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "rtc_base/logging.h"
#include "third_party/libyuv/include/libyuv.h"
#include "cclient.h"

//#ifdef __unix__
//#include "cinput_device_event.h"
//#endif
namespace chen {



    DesktopCapture::DesktopCapture() : dc_(nullptr), start_flag_(false) {}

    DesktopCapture::~DesktopCapture() {
        Destory();
    }

    void DesktopCapture::Destory() {
        StopCapture();

        if (!dc_)
            return;

        dc_.reset(nullptr);
    }

    DesktopCapture* DesktopCapture::Create(size_t target_fps,
        size_t capture_screen_index) {
        std::unique_ptr<DesktopCapture> dc(new DesktopCapture());
        if (!dc->Init(target_fps, capture_screen_index)) {
            RTC_LOG(LS_WARNING) << "Failed to create DesktopCapture(fps = "
                << target_fps << ")";
            WARNING_EX_LOG("Failed to create DesktopCapture(fps = = %u)", target_fps);
            return nullptr;
        }
        return dc.release();
    }

    bool DesktopCapture::Init(size_t target_fps, size_t capture_screen_index)
    {
        // ����
        /*dc_ = webrtc::DesktopCapturer::CreateWindowCapturer(
            webrtc::DesktopCaptureOptions::CreateDefault());*/
            //����

        #ifdef _MSC_VER
        webrtc::DesktopCaptureOptions result;
        result.set_allow_directx_capturer(true);
         dc_ = webrtc::DesktopCapturer::CreateScreenCapturer(result);

#elif defined(__GNUC__) ||defined(__APPLE__)
//        dc_ = webrtc::DesktopCapturer::CreateScreenCapturer(webrtc::DesktopCaptureOptions::CreateDefault());
        dc_ = webrtc::DesktopCapturer::CreateWindowCapturer( webrtc::DesktopCaptureOptions::CreateDefault());
#else
// 其他不支持的编译器需要自己实现这个方法
#error unexpected c complier (msc/gcc), Need to implement this method for demangle
#endif


        if (!dc_)
        {
            WARNING_EX_LOG("desktop create screen capture failed !!!");
            return false;
        }

        webrtc::DesktopCapturer::SourceList sources;
        dc_->GetSourceList(&sources);
        if (capture_screen_index > sources.size())
        {
            RTC_LOG(LS_WARNING) << "The total sources of screen is " << sources.size()
                << ", but require source of index at "
                << capture_screen_index;
            return false;
        }
        for (const webrtc::DesktopCapturer::Source & source : sources)
        {
            NORMAL_EX_LOG("[sources = %u][windowid = %u][window_name = %s]", sources.size(),  source.id, source.title.c_str());
        }
        NORMAL_EX_LOG("set input device window  = %u", sources[capture_screen_index].id);
//        set_input_device_window(sources[capture_screen_index].id);
        SYSTEM_LOG(" input device load ...");
//        load_input_device(sources[capture_screen_index].id /*g_cfg.get_uint32(ECI_UnixWindowId)*/, sources[capture_screen_index].title.c_str());
        RTC_CHECK(dc_->SelectSource(sources[capture_screen_index].id));
        window_title_ = sources[capture_screen_index].title;
        fps_ = target_fps;

        RTC_LOG(LS_INFO) << "Init DesktopCapture finish window_title = " << window_title_ << " , fps = " << fps_ << "";
        // Start new thread to capture
        return true;
    }

    void DesktopCapture::OnCaptureResult(
        webrtc::DesktopCapturer::Result result,
        std::unique_ptr<webrtc::DesktopFrame> frame) {
        //RTC_LOG(LS_INFO) << "new Frame";
        NORMAL_EX_LOG("new Frame");
        static auto timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
            .count();
        static size_t cnt = 0;

        cnt++;
        auto timestamp_curr = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
        if (timestamp_curr - timestamp > 1000) {
            RTC_LOG(LS_INFO) << "FPS: " << cnt;
            cnt = 0;
            timestamp = timestamp_curr;
        }

        // Convert DesktopFrame to VideoFrame
        if (result != webrtc::DesktopCapturer::Result::SUCCESS) {
          //  RTC_LOG(LS_ERROR) << "Capture frame faiiled, result: " << result;
            WARNING_EX_LOG("Capture frame faiiled, result: = %u", result);
            return;
        }
        int width = frame->size().width() ;
        int height = frame->size().height() ;
        // int half_width = (width + 1) / 2;

        if (!i420_buffer_.get() ||
            i420_buffer_->width() * i420_buffer_->height() < width * height) {
            i420_buffer_ = webrtc::I420Buffer::Create(width, height);
        }
		//memcpy(i420_buffer_->MutableDataY(), frame->data(), width * height * 4);
       libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
            i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
            i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
            i420_buffer_->StrideV(), 0, 0, width, height, width,
            height, libyuv::kRotate0, libyuv::FOURCC_ARGB);


        // seting ��������Ϣ

        webrtc::VideoFrame captureFrame =
            webrtc::VideoFrame::Builder()
            .set_video_frame_buffer(i420_buffer_)
            .set_timestamp_rtp(0)
            .set_timestamp_ms(rtc::TimeMillis())
            .set_rotation(webrtc::kVideoRotation_0)
            .build();
       // captureFrame.set_ntp_time_ms(0);
        s_client.webrtc_video(captureFrame);
       // DesktopCaptureSource::OnFrame(captureFrame);
        // rtc media info 
       /* DesktopCaptureSource::OnFrame(
            webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0));*/
    }

    void DesktopCapture::StartCapture()
    {
        NORMAL_EX_LOG("start capture ---->");
        if (start_flag_) {
//            RTC_LOG(WARNING) << "Capture already been running...";
            WARNING_EX_LOG("Capture already been running...");
            return;
        }

        start_flag_ = true;

        // Start new thread to capture
        capture_thread_.reset(new std::thread([this]() {
            dc_->Start(this);

            while (start_flag_) {
                dc_->CaptureFrame();
                NORMAL_EX_LOG("capture loop !!!");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
            }
            }));
    }

    void DesktopCapture::StopCapture() {
        start_flag_ = false;

        if (capture_thread_ && capture_thread_->joinable()) {
            capture_thread_->join();
        }
    }
  
}