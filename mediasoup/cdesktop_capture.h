/***********************************************************************************************
created: 		2022-01-20

author:			chensong

purpose:		assertion macros
************************************************************************************************/
#ifndef _C_DESKTOP_CAPTURE_H_
#define _C_DESKTOP_CAPTURE_H_
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "desktop_capture_source.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "api/video/i420_buffer.h"


#include <thread>
#include <atomic>
namespace chen {


    class DesktopCapture  :
        public webrtc::DesktopCapturer::Callback,
        public rtc::VideoSinkInterface<webrtc::VideoFrame> {
    public:
        static DesktopCapture* Create(size_t target_fps, size_t capture_screen_index);

        ~DesktopCapture() override;

        std::string GetWindowTitle() const { return window_title_; }

        void StartCapture();
        void StopCapture();

    private:
        DesktopCapture();

        void Destory();

        void OnFrame(const webrtc::VideoFrame& frame) override {}

        bool Init(size_t target_fps, size_t capture_screen_index);

        void OnCaptureResult(webrtc::DesktopCapturer::Result result,
            std::unique_ptr<webrtc::DesktopFrame> frame) override;

        std::unique_ptr<webrtc::DesktopCapturer> dc_;

        size_t fps_;
        std::string window_title_;

        std::unique_ptr<std::thread> capture_thread_;
        std::atomic_bool start_flag_;

        rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
    };

}


#endif // _C_DESKTOP_CAPTURE_H_