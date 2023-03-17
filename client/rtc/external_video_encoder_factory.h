#ifndef EXTERNAL_BUILTIN_VIDEO_ENCODER_FACTORY_H_
#define EXTERNAL_BUILTIN_VIDEO_ENCODER_FACTORY_H_

#include <memory>
#include "rtc_base/system/rtc_export.h"
#include "api/video_codecs/video_encoder_factory.h"
//#if defined(_MSC_VER)
namespace chen {

// Creates a new factory that can create the built-in types of video decoders.
 std::unique_ptr<webrtc::VideoEncoderFactory> CreateBuiltinExternalVideoEncoderFactory();



 

} // namespace webrtc
//#endif
#endif  // EXTERNAL_BUILTIN_VIDEO_ENCODER_FACTORY_H_