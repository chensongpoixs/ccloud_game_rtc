#include "peerConnectionUtils.hpp"
//#include "VcmCapturer.hpp"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "pc/video_track_source.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include <iostream>
#include "desktop_capture.h"
#include "PeerConnection.hpp"
#include "Handler.hpp"
#include "ccfg.h"
static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory{ nullptr };

static rtc::scoped_refptr<webrtc::AudioSourceInterface> audioSource{ nullptr };
static rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource{ nullptr };
static rtc::scoped_refptr<webrtc::VideoCaptureModule> videoCaptureModule;

//class CapturerTrackSource;
//static rtc::scoped_refptr<CapturerTrackSource> videoDevice{ nullptr };
class CapturerTrackSource;
static rtc::scoped_refptr<CapturerTrackSource> videoDevice{ nullptr };
//static rtc::Thread* signalingThread{ nullptr };
//static rtc::Thread* workerThread{ nullptr };






class CapturerTrackSource : public webrtc::VideoTrackSource
{
public:
	static rtc::scoped_refptr<CapturerTrackSource> Create()
	{
		uint32_t fps = webrtc::g_cfg.get_int32(webrtc::ECI_Video_Fps);
		if (fps <= 0)
		{
			fps = 30;
		}
		std::unique_ptr<DesktopCapture> capturer =
			absl::WrapUnique(DesktopCapture::Create(fps, 0));
		if (!capturer)
		{
			return nullptr;
		}
		capturer->StartCapture();
		capturer->AddOrUpdateSink(capturer.get(), rtc::VideoSinkWants());
		return new rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));
		/*const size_t kWidth       = 1280;
		const size_t kHeight      = 720;
		const size_t kFps         = 30;
		const size_t kDeviceIndex = 0;
		std::unique_ptr<VcmCapturer> capturer =
		  absl::WrapUnique(VcmCapturer::Create(kWidth, kHeight, kFps, kDeviceIndex));
		if (!capturer)
		{
			return nullptr;
		}
		return new rtc::RefCountedObject<CapturerTrackSource>(std::move(capturer));*/
	}
	virtual ~CapturerTrackSource()
	{
		capturer->StopCapture();
	}
	void stop()
	{
		capturer->StopCapture();
	}
	void stop_osg()
	{
		capturer->stop_osg();
	}
protected:
	explicit CapturerTrackSource(std::unique_ptr<DesktopCapture> capturer)
	  : VideoTrackSource(/*remote=*/false), capturer(std::move(capturer))
	{
	}

private:
	rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override
	{
		return capturer.get();
	}
	std::unique_ptr<DesktopCapture> capturer;
};

// PeerConnection factory creation.
static void createPeerConnectionFactory()
{
	std::cout << "[INFO] peerConnectionUtils.createPeerConnectionFactory()" << std::endl;

	webrtc::PeerConnectionInterface::RTCConfiguration config;

	/*signalingThread = new rtc::Thread();
	workerThread    = new rtc::Thread();

	signalingThread->SetName("signaling_thread", nullptr);
	workerThread->SetName("worker_thread", nullptr);

	if (!signalingThread->Start() || !workerThread->Start())
	{
		throw std::runtime_error("Thread start errored");
	}*/

	peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
		networkThread.get(),
		workerThread.get(),
		signalingThread.get(),
	  nullptr /*default_adm*/,
		 webrtc::CreateBuiltinAudioEncoderFactory() ,
	 webrtc::CreateBuiltinAudioDecoderFactory() ,
	  webrtc::CreateBuiltinVideoEncoderFactory(),
	  webrtc::CreateBuiltinVideoDecoderFactory(),
	  nullptr /*audio_mixer*/,
	  nullptr /*audio_processing*/);
}

// Audio source creation.
static void createAudioSource()
{
	std::cout << "[INFO] peerConnectionUtils.createAudiosource()" << std::endl;

	cricket::AudioOptions options;

	if (peerConnectionFactory == nullptr)
	{
		createPeerConnectionFactory();
	}

	audioSource = peerConnectionFactory->CreateAudioSource(options);
}

// Audio track creation.
rtc::scoped_refptr<webrtc::AudioTrackInterface> createAudioTrack(const std::string& label)
{
	std::cout << "[INFO] peerConnectionUtils.createAudioTrack()" << std::endl;

	if (audioSource == nullptr)
	{
		createAudioSource();
	}

	return peerConnectionFactory->CreateAudioTrack(label, audioSource);
}

// Video track creation.
rtc::scoped_refptr<webrtc::VideoTrackInterface> createVideoTrack(const std::string& label)
{
	std::cout << "[INFO] peerConnectionUtils.createVideoTrack()" << std::endl;

	if (peerConnectionFactory == nullptr)
	{
		createPeerConnectionFactory();
	}

	if (videoDevice == nullptr)
	{
		videoDevice = CapturerTrackSource::Create(); //CapturerTrackSource::Create();
	}

	return peerConnectionFactory->CreateVideoTrack(label, videoDevice);
}


void stopTrack()
{
	RTC_LOG(LS_INFO) << "stop Track ....";
	//CapturerTrackSource* capturer = videoDevice.release();
	if (peerConnectionFactory)
	{
		peerConnectionFactory->StopAecDump();
	}
	
	if (videoDevice)
	{
		videoDevice->stop();
		 
		RTC_LOG(LS_INFO) << "stop video ok !!!";
		
		//videoDevice = nullptr;
		
	}
	/*
	if (signalingThread)
	{
		signalingThread->Stop();
	}
	if (workerThread)
	{
		workerThread->Stop();
	}
	if (peerConnectionFactory)
	{
		peerConnectionFactory = nullptr;
	}*/
	RTC_LOG(LS_INFO) << "stop Track ok !!!";
}
void all_stop()
{
	if (videoDevice)
	{
		videoDevice->stop_osg();
		videoDevice = nullptr;
	}
	
	peerConnectionFactory = nullptr;


	if (signalingThread)
	{
		signalingThread->Stop();

	}
	if (workerThread)
	{
		workerThread->Stop();
	}
	if (networkThread)
	{
		networkThread->Stop();
	}

	

}