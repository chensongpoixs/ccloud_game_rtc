#ifndef EXAMPLES_OSGDESKTOP_CAPTURE_DESKTOP_CAPTURER_TEST_H_
#define EXAMPLES_OSGDESKTOP_CAPTURE_DESKTOP_CAPTURER_TEST_H_

#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "desktop_capture_source.h"
#include "desktop_capture_source.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "api/video/i420_buffer.h"
#include "cosg_capture.h"

#include <thread>
#include <atomic>

extern int32_t  g_width  ;
extern int32_t  g_height  ;
class OsgDesktopCapture : public DesktopCaptureSource,
                       public webrtc::DesktopCapturer::Callback,
                       public rtc::VideoSinkInterface<webrtc::VideoFrame> {
 public:
  static OsgDesktopCapture* Create(size_t target_fps, size_t capture_screen_index);

  ~OsgDesktopCapture() override;

  std::string GetWindowTitle() const { return window_title_; }

  void StartCapture();
  void StopCapture();
  void Pause() { m_push_pause = false; }
  void Resume() { m_push_pause = true; }
  void stop_osg();
  void OnOsgCaptureResult(webrtc::DesktopCapturer::Result result, unsigned char * rgba, int32_t width, int32_t height);

 private:
	 OsgDesktopCapture();
  void _work_thread();

  void Destory();

  void OnFrame(const webrtc::VideoFrame& frame) override {}

  bool Init(size_t target_fps, size_t capture_screen_index);

  void OnCaptureResult(webrtc::DesktopCapturer::Result result,
                       std::unique_ptr<webrtc::DesktopFrame> frame) override;
  std::unique_ptr<webrtc::DesktopCapturer> dc_;
  //std::unique_ptr<CaptureScreen>
  size_t fps_;
  std::string window_title_;

  std::unique_ptr<std::thread> capture_thread_;
  std::thread					m_work_thread;
  std::atomic_bool start_flag_;

  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  bool						m_push_pause;
  
};


#endif  // EXAMPLES_DESKTOP_CAPTURE_DESKTOP_CAPTURER_TEST_H_