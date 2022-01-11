#include "desktop_capture.h"

#include "modules/desktop_capture/desktop_capture_options.h"
#include "rtc_base/logging.h"
#include "third_party/libyuv/include/libyuv.h"
#include "ccfg.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/ComputeBoundsVisitor>
#include <osgviewer/viewereventhandlers>
#include "cosd_util.h"


int32_t  g_width = 0;
int32_t  g_height = 0;

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
    return nullptr;
  }
  

  return dc.release();
}

bool DesktopCapture::Init(size_t target_fps, size_t capture_screen_index) 
{
	/*if (chen::g_cfg.get_int32(chen::ECI_Capture_Type))
	{
		webrtc::DesktopCaptureOptions result;
		result.set_allow_directx_capturer(true);
		dc_ = webrtc::DesktopCapturer::CreateScreenCapturer(result);
		if (!dc_)
		{
			return false;
		}
		webrtc::DesktopCapturer::SourceList sources;
		dc_->GetSourceList(&sources);
		RTC_CHECK(dc_->SelectSource(sources[capture_screen_index].id));
		window_title_ = sources[capture_screen_index].title;
	}*/
	/*webrtc::DesktopCaptureOptions result;
	result.set_allow_directx_capturer(true);*/
	// 窗口
	//dc_ = webrtc::DesktopCapturer::CreateWindowCapturer(result);
		//webrtc::DesktopCaptureOptions::CreateDefault());
	//桌面
	
	//dc_ = webrtc::DesktopCapturer::CreateScreenCapturer(result);

  //if (!dc_)
  //  return false;

  //webrtc::DesktopCapturer::SourceList sources;
  //dc_->GetSourceList(&sources);
  ////int index = 0;
  ////FILE *out_file_ptr = fopen("chensong_desktop_capture.log", "wb+");
  ////for (const webrtc::DesktopCapturer::Source & source : sources)
  ////{
	 //// fprintf(out_file_ptr, "[title = %s]\n", source.title.c_str());
	 //// fflush(out_file_ptr);
	 //// 
	 //// //if ("chensong.mp4 - PotPlayer" == source.title)
	 //// //if ("rtmp://127.0.0.1/live/test - VLC media player" == source.title)
	 //// if ("Prj_ChengDu (64-bit Development PCD3D_SM5) " == source.title)
	 //// {
		////  capture_screen_index = index;
		////  break;
	 //// }
	 //// ++index;
  ////}
  ////fprintf(out_file_ptr, "[capture_screen_index = %d]\n", capture_screen_index);
  ////fflush(out_file_ptr);
  //if (capture_screen_index > sources.size()) {
  //  RTC_LOG(LS_WARNING) << "The total sources of screen is " << sources.size()
  //                      << ", but require source of index at "
  //                      << capture_screen_index;
  //  return false;
  //}



  //RTC_CHECK(dc_->SelectSource(sources[capture_screen_index].id));
  //window_title_ = sources[capture_screen_index].title;
  fps_ = target_fps;

  RTC_LOG(LS_INFO) << "Init DesktopCapture finish window_title = " << window_title_ << " , fps = " << fps_ <<"";
  // Start new thread to capture
  return true;
}

void DesktopCapture::OnCaptureResult(
    webrtc::DesktopCapturer::Result result,
    std::unique_ptr<webrtc::DesktopFrame> frame) {
  //RTC_LOG(LS_INFO) << "new Frame";

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
    RTC_LOG(LS_INFO) << "FPS: " << cnt << ", " << frame->rect().width() << ", " << frame->rect().height();
	RTC_LOG(LS_INFO) << "[width = " << frame->size().width() << "][height = " << frame->size().height() << "]";
    cnt = 0;
    timestamp = timestamp_curr;
  }

  // Convert DesktopFrame to VideoFrame
  if (result != webrtc::DesktopCapturer::Result::SUCCESS) {
    RTC_LOG(LS_ERROR) << "Capture frame faiiled, result: " << result;
	return;
  }
  int width = frame->size().width();
  int height = frame->size().height();
  // int half_width = (width + 1) / 2;

  if (!i420_buffer_.get() ||
      i420_buffer_->width() * i420_buffer_->height() < width * height) {
    i420_buffer_ = webrtc::I420Buffer::Create(width, height);
  }

  libyuv::ConvertToI420(frame->data(), 0, i420_buffer_->MutableDataY(),
                        i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
                        i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
                        i420_buffer_->StrideV(), 0, 0, width, height, width,
                        height, libyuv::kRotate0, libyuv::FOURCC_ARGB);


  // seting 马流的信息

  {
	  webrtc::VideoFrame captureFrame =
		  webrtc::VideoFrame::Builder()
		  .set_video_frame_buffer(i420_buffer_)
		  .set_timestamp_rtp(0)
		  .set_timestamp_ms(rtc::TimeMillis())
		  .set_rotation(webrtc::kVideoRotation_0)
		  .build();
	  captureFrame.set_ntp_time_ms(0);
	  DesktopCaptureSource::OnFrame(captureFrame);
  }
 
  // rtc media info 
 /* DesktopCaptureSource::OnFrame(
      webrtc::VideoFrame(i420_buffer_, 0, 0, webrtc::kVideoRotation_0));*/
}


void DesktopCapture::OnOsgCaptureResult(webrtc::DesktopCapturer::Result result,  unsigned char * BGRA, int32_t width, int32_t height)
{
	if (!BGRA)
	{
		RTC_LOG(LS_ERROR) << "rgba nullptr !!!";
		return;
	}
	 
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
		RTC_LOG(LS_INFO) << "FPS: " << cnt << ", " <<  width  << ", " <<  height ;
		RTC_LOG(LS_INFO) << "[width = " <<  width << "][height = " <<  height << "]";
		cnt = 0;
		timestamp = timestamp_curr;
	}

	// Convert DesktopFrame to VideoFrame
	if (result != webrtc::DesktopCapturer::Result::SUCCESS) {
		RTC_LOG(LS_ERROR) << "Capture frame faiiled, result: " << result;
	}
	/*int width = frame->size().width();
	int height = frame->size().height();*/
	// int half_width = (width + 1) / 2;

	if (!i420_buffer_.get() ||
		i420_buffer_->width() * i420_buffer_->height() < width * height) {
		i420_buffer_ = webrtc::I420Buffer::Create(width, height);
	}
	

	
	libyuv::ConvertToI420(BGRA, 0, i420_buffer_->MutableDataY(),
		i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
		i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
		i420_buffer_->StrideV(), 0, 0, width, height, width,
		height, libyuv::kRotate0, libyuv::FOURCC_ARGB); // GL_BGRA，  FOURCC_BGRA 、、GL_BGR

	
	webrtc::draw_font_func(i420_buffer_->MutableDataY(), i420_buffer_->MutableDataU(), i420_buffer_->MutableDataV(), "A", g_width, g_height, 3, 1, width , height);
	
	// seting 马流的信息 
	{
		webrtc::VideoFrame captureFrame =
			webrtc::VideoFrame::Builder()
			.set_video_frame_buffer(i420_buffer_)
			.set_timestamp_rtp(0)
			.set_timestamp_ms(rtc::TimeMillis())
			.build();
		captureFrame.set_ntp_time_ms(0);
		DesktopCaptureSource::OnFrame(captureFrame);
	}

}
osg::ref_ptr<osgViewer::Viewer> viewer;
void DesktopCapture::StartCapture() {
  if (start_flag_) {
    RTC_LOG(WARNING) << "Capture already been running...";
    return;
  }

  start_flag_ = true;

  //if (chen::g_cfg.get_int32(chen::ECI_Capture_Type) == 0)
  {
	  if (m_work_thread.joinable())
	  {
		  m_work_thread.join();
	  }
	  CaptureScreen::Get()->setCallback(this);
	  m_work_thread = std::thread(&DesktopCapture::_work_thread, this);
  }
 
  // Start new thread to capture
 // capture_thread_.reset(new std::thread([this]() {
	//  
 // 
	//  std::this_thread::sleep_for(std::chrono::seconds(3));
	//   
 //   while (start_flag_) 
	//{
	//	 
	//		CaptureScreen::Get()->CaptureFrame();
	//		 
	//  
	////  RTC_LOG(LS_INFO) << "captureFrame fps = " << elapse;
 //     std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps_));
 //   }
	//if (viewer)
	//{
	//	viewer->stopThreading();
	//	delete viewer;
	//	viewer = nullptr;
	//}
	//
 // }));
}

void DesktopCapture::StopCapture() {
  start_flag_ = false;

  if (capture_thread_ && capture_thread_->joinable()) {
    capture_thread_->join();
  }
}


void DesktopCapture::_work_thread()
{
	osg::DisplaySettings::instance()->setNumMultiSamples(16);
	size_t size = sizeof(osgViewer::Viewer);
	void *ptr = malloc(size);
	viewer = new(ptr) osgViewer::Viewer;
	osg::ref_ptr<osg::Group> sceneRoot = new osg::Group;
	viewer->getCamera()->addPreDrawCallback(CaptureScreen::Get());
	viewer->setSceneData(osgDB::readNodeFile("D:/Work/cmedia_server/webrtc_google/lib/test/mediasoup/mediasoup/cow.ive"));
	viewer->addEventHandler(new osgViewer::WindowSizeHandler);
	viewer->run();
}
