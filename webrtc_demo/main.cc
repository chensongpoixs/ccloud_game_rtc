 
#include "rtc_base/logging.h"

#include <thread>
#include "system_wrappers/include/field_trial.h"
#include <api/audio_codecs/builtin_audio_decoder_factory.h>
#include <api/audio_codecs/builtin_audio_encoder_factory.h>
#include <api/create_peerconnection_factory.h>
#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>

int main(int argc, char *argv[])
{
	//HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow
//	open_windows(instance, prev_instance, cmd_line, cmd_show);
	//return 0;
	 RTC_LOG(INFO) << "desktop capture !!!";
     std::unique_ptr<rtc::Thread> net_thread = rtc::Thread::CreateWithSocketServer();
    std::unique_ptr<rtc::Thread> signel_thread =  rtc::Thread::Create();
    std::unique_ptr<rtc::Thread> work_thread =  rtc::Thread::Create();

    net_thread->Start();
    signel_thread->Start();
    work_thread->Start();

    webrtc::CreatePeerConnectionFactory(
            net_thread.get(),
            signel_thread.get(),
            work_thread.get(),
            nullptr /*default_adm*/,
            webrtc::CreateBuiltinAudioEncoderFactory(),
            webrtc::CreateBuiltinAudioDecoderFactory(),
            webrtc::CreateBuiltinVideoEncoderFactory()  ,
            webrtc::CreateBuiltinVideoDecoderFactory(),
            nullptr /*audio_mixer*/,
            nullptr /*audio_processing*/);
  printf("webrt demo run OK !!!\n");
 //  RTC_LOG(WARNING) << "Demo exit";
  return 0;
}

// #include <string>
// #include <iostream>
// int main(int argc, char *argv[])
// {
// 	std::cout << "-----" << std::endl;
// 	return 0;
// }
