#include "hw_video_encoder_factory.h"

#include <absl/memory/memory.h>
#include <absl/strings/match.h>
#include <api/video_codecs/sdp_video_format.h>
#include <media/base/codec.h>
#include <media/base/media_constants.h>
#include <media/engine/simulcast_encoder_adapter.h>
//#include <modules/video_coding/codecs/av1/libaom_av1_encoder.h>
#include <modules/video_coding/codecs/h264/include/h264.h>
#include <modules/video_coding/codecs/vp8/include/vp8.h>
#include <modules/video_coding/codecs/vp9/include/vp9.h>
#include <rtc_base/logging.h>


#include "NvCodec/nvcodec_h264_encoder.h"


namespace chen {

std::vector<webrtc::SdpVideoFormat> HWVideoEncoderFactory::GetSupportedFormats()
    const {
  std::vector<webrtc::SdpVideoFormat> supported_codecs;
 /* supported_codecs.push_back(webrtc::SdpVideoFormat(cricket::kVp8CodecName));
  for (const webrtc::SdpVideoFormat& format : webrtc::SupportedVP9Codecs())
    supported_codecs.push_back(format);

  supported_codecs.push_back(webrtc::SdpVideoFormat(cricket::kAv1CodecName));

//#if defined(SORA_UNITY_SDK_WINDOWS)*/
//  if (NvCodecH264Encoder::IsSupported()) {
//    std::vector<webrtc::SdpVideoFormat> h264_codecs = {
//        CreateH264Format(webrtc::H264Profile::kProfileBaseline,
//                         webrtc::H264Level::kLevel3_1, "1"),
//        CreateH264Format(webrtc::H264Profile::kProfileBaseline,
//                         webrtc::H264Level::kLevel3_1, "0"),
//        CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline,
//                         webrtc::H264Level::kLevel3_1, "1"),
//        CreateH264Format(webrtc::H264Profile::kProfileConstrainedBaseline,
//                         webrtc::H264Level::kLevel3_1, "0")};
//
//    for (const webrtc::SdpVideoFormat& format : h264_codecs)
//      supported_codecs.push_back(format);
//  }
////#endif
  for (const webrtc::SdpVideoFormat& h264_format : webrtc::SupportedH264Codecs())
  {
	  supported_codecs.push_back(h264_format);
  }
  return supported_codecs;
}

std::unique_ptr<webrtc::VideoEncoder> HWVideoEncoderFactory::CreateVideoEncoder(
    const webrtc::SdpVideoFormat& format) {
 /* if (absl::EqualsIgnoreCase(format.name, cricket::kVp8CodecName))
    return webrtc::VP8Encoder::Create();

  if (absl::EqualsIgnoreCase(format.name, cricket::kVp9CodecName))
    return webrtc::VP9Encoder::Create(cricket::VideoCodec(format));

  if (absl::EqualsIgnoreCase(format.name, cricket::kAv1CodecName)) {
    return webrtc::CreateLibaomAv1Encoder();
  }

#if defined(SORA_UNITY_SDK_WINDOWS)
  if (absl::EqualsIgnoreCase(format.name, cricket::kH264CodecName)) {
    return std::unique_ptr<webrtc::VideoEncoder>(
        absl::make_unique<NvCodecH264Encoder>(cricket::VideoCodec(format)));
  }
#endif

  RTC_LOG(LS_ERROR) << "Trying to created encoder of unsupported format "
                    << format.name;*/

	
  if (format.name == cricket::kH264CodecName/*absl::EqualsIgnoreCase(format.name, cricket::kH264CodecName)*/) {
	  			if (webrtc::H264Encoder::IsSupported()) {
	  				if (NvCodecH264Encoder::IsSupported()) {
						NORMAL_EX_LOG("NvCodecH264Encoder");
	  					return absl::make_unique<NvCodecH264Encoder>(cricket::VideoCodec(format));
	  				}
	  				else {
						NORMAL_EX_LOG("OpenH264");
	  					return webrtc::H264Encoder::Create(cricket::VideoCodec(format));
	  				}
	  			}
				ERROR_EX_LOG("not support h264");
	  		}
  ERROR_EX_LOG("not support format  = %s" , format.name.c_str());
  return nullptr;
}

}  // namespace sora
