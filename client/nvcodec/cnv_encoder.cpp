/***********************************************************************************************
created: 		2020-2-13

author:			chensong

purpose:		nvenc
************************************************************************************************/
#include "cnv_encoder.h"

#include <limits>
#include <string>

#include "third_party/openh264/src/codec/api/svc/codec_api.h"
#include "third_party/openh264/src/codec/api/svc/codec_app_def.h"
#include "third_party/openh264/src/codec/api/svc/codec_def.h"
#include "third_party/openh264/src/codec/api/svc/codec_ver.h"
#include "absl/strings/match.h"
#include "common_video/h264/h264_common.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "modules/video_coding/utility/simulcast_rate_allocator.h"
#include "modules/video_coding/utility/simulcast_utility.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"
#include "system_wrappers/include/metrics.h"
#include "third_party/libyuv/include/libyuv/convert.h"
#include "third_party/libyuv/include/libyuv/scale.h"
#include "third_party/libyuv/include/libyuv.h"
#include "ccfg.h"
#include "clog.h"
#include "cgl_egl_common.h"
#include "NvEncoderGL.h"
#include "NvEncoderCLIOptions.h"
// #include "NvCodecUtils.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include "cgl_global.h"

// namespace webrtc {

// namespace {
// 	using namespace chen;
// const bool kOpenH264EncoderDetailedLogging = false;

// // QP scaling thresholds.
// static const int kLowH264QpThreshold = 24;
// static const int kHighH264QpThreshold = 37;

// // Used by histograms. Values of entries should not be changed.
// enum NvVideoEncoderEvent
// {
// 	kH264EncoderEventInit = 0,
// 	kH264EncoderEventError = 1,
// 	kH264EncoderEventMax = 16,
// };

// int NumberOfThreads(int width, int height, int number_of_cores) 
// {
// 	// TODO(hbos): In Chromium, multiple threads do not work with sandbox on Mac,
// 	// see crbug.com/583348. Until further investigated, only use one thread.
// 	//  if (width * height >= 1920 * 1080 && number_of_cores > 8) {
// 	//    return 8;  // 8 threads for 1080p on high perf machines.
// 	//  } else if (width * height > 1280 * 960 && number_of_cores >= 6) {
// 	//    return 3;  // 3 threads for 1080p.
// 	//  } else if (width * height > 640 * 480 && number_of_cores >= 3) {
// 	//    return 2;  // 2 threads for qHD/HD.
// 	//  } else {
// 	//    return 1;  // 1 thread for VGA or less.
// 	//  }
// 	// TODO(sprang): Also check sSliceArgument.uiSliceNum om GetEncoderPrams(),
// 	//               before enabling multithreading here.
//   return 1;
// }

// VideoFrameType ConvertToVideoFrameType(EVideoFrameType type) {
// 	switch (type) {
// 	case videoFrameTypeIDR:
// 		return VideoFrameType::kVideoFrameKey;
// 	case videoFrameTypeSkip:
// 	case videoFrameTypeI:
// 	case videoFrameTypeP:
// 	case videoFrameTypeIPMixed:
// 		return VideoFrameType::kVideoFrameDelta;
// 	case videoFrameTypeInvalid:
// 		break;
// 	}
// 	RTC_NOTREACHED() << "Unexpected/invalid frame type: " << type;
// 	return VideoFrameType::kEmptyFrame;
// }

// }  // namespace

// // Helper method used by NvVideoEncoder::Encode.
// // Copies the encoded bytes from |info| to |encoded_image| and updates the
// // fragmentation information of |frag_header|. The |encoded_image->_buffer| may
// // be deleted and reallocated if a bigger buffer is required.
// //
// // After OpenH264 encoding, the encoded bytes are stored in |info| spread out
// // over a number of layers and "NAL units". Each NAL unit is a fragment starting
// // with the four-byte start code {0,0,0,1}. All of this data (including the
// // start codes) is copied to the |encoded_image->_buffer| and the |frag_header|
// // is updated to point to each fragment, with offsets and lengths set as to
// // exclude the start codes.
// struct cnv_frame_packet
// {
//     std::shared_ptr<uint8_t>   frame;
//     uint32_t                   use_size;
//     cnv_frame_packet()
//         : frame()
//         , use_size(0)
//     {
//     }
// };

// static void RtpFragmentize(EncodedImage* encoded_image,
// 	const VideoFrameBuffer& frame_buffer, cnv_frame_packet&  frame_packet
// 	/*std::vector<uint8_t>& frame_packet*/,
// 	RTPFragmentationHeader* frag_header)
// {
	
// 	size_t required_capacity = 0;
// 	encoded_image->set_size(0);

// 	required_capacity = frame_packet.use_size;
// 	//encoded_image->Allocate(required_capacity);

// 	// TODO(nisse): Use a cache or buffer pool to avoid allocation?
// 	encoded_image->Allocate(required_capacity);


// 	///////////////////////////////////////H264 NAL///////////////////////////////////////////////////////

// 	memcpy(encoded_image->data() + encoded_image->size(), &frame_packet.frame.get()[0], frame_packet.use_size);
// 	encoded_image->set_size(encoded_image->size() + frame_packet.use_size);
	
// 	//////////////////////////////////////////////////////////////////////////////////////////////////////
	
// 	std::vector<webrtc::H264::NaluIndex> nalus = webrtc::H264::FindNaluIndices(
// 		encoded_image->data(), encoded_image->size());
// 	size_t fragments_count = nalus.size();
// 	// const uint8_t start_code[4] = {0, 0, 0, 1};
// 	frag_header->VerifyAndAllocateFragmentationHeader(fragments_count);
// 	for (size_t i = 0; i < nalus.size(); i++) {
// 		frag_header->fragmentationOffset[i] = nalus[i].payload_start_offset;
// 		frag_header->fragmentationLength[i] = nalus[i].payload_size;
// 	}
// }




// static void RtpFragmentize_webrtc(EncodedImage* encoded_image,
// 	const VideoFrameBuffer& frame_buffer,
// 	SFrameBSInfo* info,
// 	RTPFragmentationHeader* frag_header) {
// 	// Calculate minimum buffer size required to hold encoded data.
// 	size_t required_capacity = 0;
// 	size_t fragments_count = 0;
// 	for (int layer = 0; layer < info->iLayerNum; ++layer) {
// 		const SLayerBSInfo& layerInfo = info->sLayerInfo[layer];
// 		for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++fragments_count) {
// 			RTC_CHECK_GE(layerInfo.pNalLengthInByte[nal], 0);
// 			// Ensure |required_capacity| will not overflow.
// 			RTC_CHECK_LE(layerInfo.pNalLengthInByte[nal],
// 				std::numeric_limits<size_t>::max() - required_capacity);
// 			required_capacity += layerInfo.pNalLengthInByte[nal];
// 		}
// 	}
// 	if (encoded_image->capacity() < required_capacity) {
// 		// Increase buffer size. Allocate enough to hold an unencoded image, this
// 		// should be more than enough to hold any encoded data of future frames of
// 		// the same size (avoiding possible future reallocation due to variations in
// 		// required size).
// 		size_t new_capacity = CalcBufferSize(VideoType::kI420, frame_buffer.width(),
// 			frame_buffer.height());
// 		if (new_capacity < required_capacity) {
// 			// Encoded data > unencoded data. Allocate required bytes.
// 			RTC_LOG(LS_WARNING)
// 				<< "Encoding produced more bytes than the original image "
// 				<< "data! Original bytes: " << new_capacity
// 				<< ", encoded bytes: " << required_capacity << ".";
// 			new_capacity = required_capacity;
// 		}
// 		encoded_image->Allocate(new_capacity);
// 	}

// 	// Iterate layers and NAL units, note each NAL unit as a fragment and copy
// 	// the data to |encoded_image->_buffer|.
// 	const uint8_t start_code[4] = { 0, 0, 0, 1 };
// 	frag_header->VerifyAndAllocateFragmentationHeader(fragments_count);
// 	size_t frag = 0;
// 	encoded_image->set_size(0);
// 	for (int layer = 0; layer < info->iLayerNum; ++layer) {
// 		const SLayerBSInfo& layerInfo = info->sLayerInfo[layer];
// 		// Iterate NAL units making up this layer, noting fragments.
// 		size_t layer_len = 0;
// 		for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++frag) {
// 			// Because the sum of all layer lengths, |required_capacity|, fits in a
// 			// |size_t|, we know that any indices in-between will not overflow.
// 			RTC_DCHECK_GE(layerInfo.pNalLengthInByte[nal], 4);
// 			RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 0], start_code[0]);
// 			RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 1], start_code[1]);
// 			RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 2], start_code[2]);
// 			RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 3], start_code[3]);
// 			frag_header->fragmentationOffset[frag] =
// 				encoded_image->size() + layer_len + sizeof(start_code);
// 			frag_header->fragmentationLength[frag] =
// 				layerInfo.pNalLengthInByte[nal] - sizeof(start_code);
// 			layer_len += layerInfo.pNalLengthInByte[nal];
// 		}
// 		// Copy the entire layer's data (including start codes).
// 		memcpy(encoded_image->data() + encoded_image->size(), layerInfo.pBsBuf,
// 			layer_len);
// 		encoded_image->set_size(encoded_image->size() + layer_len);
// 	}
// }

// NvEncoder::NvEncoder(const cricket::VideoCodec& codec)
//     : packetization_mode_(H264PacketizationMode::SingleNalUnit),
//       max_payload_size_(0),
//       number_of_cores_(0),
//       encoded_image_callback_(nullptr),
//       has_reported_init_(false),
//       has_reported_error_(false),
//       num_temporal_layers_(1),
//       tl0sync_limit_(0)
// 	, m_key_frame_count(0)
// {
// 	//RTC_CHECK(/*absl::EqualsIgnoreCase(codec.name, cricket::kH264CodecName)*/ codec.name == cricket::kH264CodecName);
// 	std::string packetization_mode_string;
// 	if (codec.GetParam(cricket::kH264FmtpPacketizationMode, &packetization_mode_string) 
// 		&& packetization_mode_string == "1") {
// 		packetization_mode_ = H264PacketizationMode::NonInterleaved;
// 	}

// 	encoded_images_.reserve(kMaxSimulcastStreams);
// 	nv_encoders_.reserve(kMaxSimulcastStreams);
// 	configurations_.reserve(kMaxSimulcastStreams);
// 	image_buffer_ = nullptr;
// }

// NvEncoder::~NvEncoder() 
// {
// 	Release();
// }

// int32_t NvEncoder::InitEncode(const VideoCodec* inst,
// 	int32_t number_of_cores,
// 	size_t max_payload_size)
// {
// 	ReportInit();
// 	if (!inst || inst->codecType != kVideoCodecH264) {
// 		ReportError();
// 		return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
// 	}
// 	if (inst->maxFramerate == 0) {
// 		ReportError();
// 		return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
// 	}
// 	if (inst->width < 1 || inst->height < 1) {
// 		ReportError();
// 		return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
// 	}

// 	int32_t release_ret = Release();
// 	if (release_ret != WEBRTC_VIDEO_CODEC_OK) {
// 		ReportError();
// 		return release_ret;
// 	}

// 	int number_of_streams = SimulcastUtility::NumberOfSimulcastStreams(*inst);
// 	bool doing_simulcast = (number_of_streams > 1);

// 	if (doing_simulcast && !SimulcastUtility::ValidSimulcastParameters(*inst, number_of_streams)) {
// 		return WEBRTC_VIDEO_CODEC_ERR_SIMULCAST_PARAMETERS_NOT_SUPPORTED;
// 	}

// 	assert(number_of_streams == 1);

// 	encoded_images_.resize(number_of_streams);
// 	nv_encoders_.resize(number_of_streams);
// 	configurations_.resize(number_of_streams);

// 	number_of_cores_ = number_of_cores;
// 	max_payload_size_ = max_payload_size;
// 	codec_ = *inst;

// 	// Code expects simulcastStream resolutions to be correct, make sure they are
// 	// filled even when there are no simulcast layers.
// 	if (codec_.numberOfSimulcastStreams == 0) {
// 		codec_.simulcastStream[0].width = codec_.width;
// 		codec_.simulcastStream[0].height = codec_.height;
// 	}
//     init_gl();
//     NORMAL_EX_LOG("");
//     std::ostringstream oss;
//     oss << "-codec h264 -fps 25 ";
//     NvEncoderInitParam encodeCLIOptions(oss.str().c_str());
// 	num_temporal_layers_ = codec_.H264()->numberOfTemporalLayers;

// 	for (int i = 0, idx = number_of_streams - 1; i < number_of_streams; ++i, --idx) {
		

// 		// Set internal settings from codec_settings
// 		configurations_[i].simulcast_idx = idx;
// 		configurations_[i].sending = false;
// 		configurations_[i].width = codec_.simulcastStream[idx].width;
// 		configurations_[i].height = codec_.simulcastStream[idx].height;
// 		configurations_[i].max_frame_rate = static_cast<float>(codec_.maxFramerate);
// 		configurations_[i].frame_dropping_on = codec_.H264()->frameDroppingOn;
// 		configurations_[i].key_frame_interval = codec_.H264()->keyFrameInterval;

// 		// Codec_settings uses kbits/second; encoder uses bits/second.
// 		configurations_[i].max_bps = codec_.maxBitrate * 1000;
// 		configurations_[i].target_bps = codec_.startBitrate * 1000;
 
//         // Store nvidia encoder.
// 		nv_encoders_[i] = new NvEncoderGL  (configurations_[i].width, configurations_[i].height, NV_ENC_BUFFER_FORMAT_IYUV);
//         NORMAL_EX_LOG("");
//         NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
//         NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
//         initializeParams.encodeConfig = &encodeConfig;
//         nv_encoders_[i]->CreateDefaultEncoderParams(&initializeParams, encodeCLIOptions.GetEncodeGUID(), encodeCLIOptions.GetPresetGUID());
//         NORMAL_EX_LOG("");
//         encodeCLIOptions.SetInitParams(&initializeParams, NV_ENC_BUFFER_FORMAT_IYUV);
//         NORMAL_EX_LOG("");
//         nv_encoders_[i]->CreateEncoder(&initializeParams);


// 		// encoder_config nvenc_config;
// 		// nvenc_config.codec = "h264";
// 		// // 设置像素格式
// 		// nvenc_config.format =  g_dxgi_format;  //DXGI_FORMAT_B8G8R8A8_UNORM;
// 		// NORMAL_EX_LOG("[format = %u]", g_dxgi_format);
// 		// nvenc_config.width = configurations_[i].width;
// 		// nvenc_config.height = configurations_[i].height;
// 		// nvenc_config.framerate = (uint32_t)configurations_[i].max_frame_rate;
// 		//TODO@chensong 2022-03-18 设置GOP 的大小 哈 ^_^ 不知道为什么 webrtc中设置gop key_frame_interval 字段是有效的、
// 		//根据程序
// 		using namespace chen;
// 		// nvenc_config.gop = NVENC_INFINITE_GOPLENGTH; // g_cfg.get_uint32(ECI_EncoderVideoGop);// configurations_[i].key_frame_interval;
// 		// nvenc_config.bitrate = 100000;
// 		// if (!nvenc_info.init(nv_encoders_[i], &nvenc_config)) {
// 		// 	Release();
// 		// 	ReportError();
// 		// 	return WEBRTC_VIDEO_CODEC_ERROR;
// 		// }

// 		image_buffer_.reset(new uint8_t[configurations_[i].width * configurations_[i].height * 10]);

// 		// TODO(pbos): Base init params on these values before submitting.
// 		video_format_ = EVideoFormatType::videoFormatI420;

// 		// Initialize encoded image. Default buffer size: size of unencoded data.
// 		const size_t new_capacity = CalcBufferSize(VideoType::kI420,
// 			codec_.simulcastStream[idx].width, codec_.simulcastStream[idx].height);
// 		encoded_images_[i].Allocate((new_capacity));
// 		encoded_images_[i]._completeFrame = true;
// 		encoded_images_[i]._encodedWidth = codec_.simulcastStream[idx].width;
// 		encoded_images_[i]._encodedHeight = codec_.simulcastStream[idx].height;
// 		encoded_images_[i].set_size(0);
// 	}

// 	SimulcastRateAllocator init_allocator(codec_);
// 	/*VideoBitrateAllocation allocation = init_allocator.GetAllocation(
// 		codec_.startBitrate * 1000, codec_.maxFramerate);
// 	SetRates(RateControlParameters(allocation, codec_.maxFramerate));
// 	return WEBRTC_VIDEO_CODEC_OK;*/
// 	VideoBitrateAllocation allocation = init_allocator.GetAllocation(
// 		codec_.startBitrate * 1000, codec_.maxFramerate);
// 	return SetRateAllocation(allocation, codec_.maxFramerate);
// }

// int32_t NvEncoder::Release() 
// {
// 	while (!nv_encoders_.empty()) 
// 	{
// 		NvEncoderGL* nv_encoder = nv_encoders_.back();
// 		if (nv_encoder) {
// 			//nvenc_info.destroy(&nv_encoder);
//             delete nv_encoder;
// 			nv_encoder = nullptr;
// 		}
// 		nv_encoders_.pop_back();
// 	}

// 	configurations_.clear();
// 	encoded_images_.clear();

// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// int32_t NvEncoder::RegisterEncodeCompleteCallback(EncodedImageCallback* callback) 
// {
// 	encoded_image_callback_ = callback;
// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// void NvEncoder::SetRates(const  RateControlParameters& parameters)
// {
// 	if (parameters.bitrate.get_sum_bps() == 0) {
// 		// Encoder paused, turn off all encoding.
// 		for (size_t i = 0; i < configurations_.size(); ++i)
// 			configurations_[i].SetStreamState(false);
// 		return;
// 	}

// 	// At this point, bitrate allocation should already match codec settings.
// 	if (codec_.maxBitrate > 0)
// 		RTC_DCHECK_LE(parameters.bitrate.get_sum_kbps(), codec_.maxBitrate);
// 	RTC_DCHECK_GE(parameters.bitrate.get_sum_kbps(), codec_.minBitrate);
// 	if (codec_.numberOfSimulcastStreams > 0)
// 		RTC_DCHECK_GE(parameters.bitrate.get_sum_kbps(), codec_.simulcastStream[0].minBitrate);

// 	codec_.maxFramerate = parameters.framerate_fps;

// 	size_t stream_idx = nv_encoders_.size() - 1;
// 	for (size_t i = 0; i < nv_encoders_.size(); ++i, --stream_idx) {
// 		configurations_[i].target_bps = parameters.bitrate.GetSpatialLayerSum(stream_idx);
// 		configurations_[i].max_frame_rate = static_cast<float>(parameters.framerate_fps);

// 		if (configurations_[i].target_bps) {
// 			configurations_[i].SetStreamState(true);

// 		// 	if (nv_encoders_[i]) {
// 		// 		nvenc_info.set_bitrate(nv_encoders_[i], configurations_[i].target_bps);
// 		// 		nvenc_info.set_framerate(nv_encoders_[i], (uint32_t)configurations_[i].max_frame_rate);
// 		// 	}
// 		// 	else {
// 		// 		configurations_[i].SetStreamState(false);
// 		// 	}
// 		// } 
// 	}
// }

// int32_t NvEncoder::SetRateAllocation(const VideoBitrateAllocation & bitrate, uint32_t framerate)
// {
// 	if (nv_encoders_.empty())
// 		return WEBRTC_VIDEO_CODEC_UNINITIALIZED;

	
// 	if (bitrate.get_sum_bps() == 0) {
// 		// Encoder paused, turn off all encoding.
// 		for (size_t i = 0; i < configurations_.size(); ++i)
// 			configurations_[i].SetStreamState(false);
// 		return WEBRTC_VIDEO_CODEC_OK;
// 	}

// 	// At this point, bitrate allocation should already match codec settings.
// 	if (codec_.maxBitrate > 0)
// 		RTC_DCHECK_LE(bitrate.get_sum_kbps(), codec_.maxBitrate);
// 	RTC_DCHECK_GE(bitrate.get_sum_kbps(), codec_.minBitrate);
// 	if (codec_.numberOfSimulcastStreams > 0)
// 		RTC_DCHECK_GE(bitrate.get_sum_kbps(), codec_.simulcastStream[0].minBitrate);

// 	codec_.maxFramerate = framerate;

// 	size_t stream_idx = nv_encoders_.size() - 1;
// 	for (size_t i = 0; i < nv_encoders_.size(); ++i, --stream_idx) {
// 		configurations_[i].target_bps = bitrate.GetSpatialLayerSum(stream_idx);
// 		configurations_[i].max_frame_rate = static_cast<float>(framerate);

// 		if (configurations_[i].target_bps) {
// 			configurations_[i].SetStreamState(true);

// 			// if (nv_encoders_[i]) {
// 			// 	nvenc_info.set_bitrate(nv_encoders_[i], configurations_[i].target_bps);
// 			// 	nvenc_info.set_framerate(nv_encoders_[i], (uint32_t)configurations_[i].max_frame_rate);
// 			// }
// 			// else {
// 			// 	configurations_[i].SetStreamState(false);
// 			// }
// 		}
// 	}

// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// int32_t NvEncoder::Encode(const VideoFrame& input_frame,
// 						  const std::vector<VideoFrameType>* frame_types)
// {
// 	std::chrono::steady_clock::time_point cur_time_ms;
// 	std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
// 	std::chrono::steady_clock::duration dur;
// 	std::chrono::milliseconds milliseconds;
// 	uint32_t elapse = 0;
// 	if (nv_encoders_.empty()) {
// 		ReportError();
// 		return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
// 	}
// 	if (!encoded_image_callback_) {
// 		RTC_LOG(LS_WARNING)
// 			<< "InitEncode() has been called, but a callback function "
// 			<< "has not been set with RegisterEncodeCompleteCallback()";
// 		ReportError();
// 		return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
// 	}

// 	rtc::scoped_refptr<const I420BufferInterface> frame_buffer = input_frame.video_frame_buffer()->ToI420();

// 	bool send_key_frame = false;
// 	for (size_t i = 0; i < configurations_.size(); ++i) {
// 		if (configurations_[i].key_frame_request && configurations_[i].sending) {
// 			send_key_frame = true;
// 			break;
// 		}
// 	}
// 	if (!send_key_frame && frame_types) {
// 		for (size_t i = 0; i < frame_types->size() && i < configurations_.size(); ++i) {
// 			if ((*frame_types)[i] == VideoFrameType::kVideoFrameKey && configurations_[i].sending) {
// 				send_key_frame = true;
// 				break;
// 			}
// 		}
// 	}
// 	if (send_key_frame)
// 	{
// 		m_key_frame_count = 0;
// 	}
// 	RTC_DCHECK_EQ(configurations_[0].width, frame_buffer->width());
// 	RTC_DCHECK_EQ(configurations_[0].height, frame_buffer->height());
//  rtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_  = webrtc::I420Buffer::Create(frame_buffer->width(), frame_buffer->height());
// 	// Encode image for each layer.
// 	for (size_t i = 0; i < nv_encoders_.size(); ++i) {
// 		if (!configurations_[i].sending) {
// 			continue;
// 		}

// 		if (frame_types != nullptr) {
// 			// Skip frame?
// 			if ((*frame_types)[i] == VideoFrameType::kEmptyFrame) {
// 				continue;
// 			}
// 		}

// 		if (send_key_frame) {			
// 			// if (!nv_encoders_.empty() && nv_encoders_[i]) 
// 			// {
				
// 			// 	nvenc_info.request_idr(nv_encoders_[i]);
// 			// }

// 			configurations_[i].key_frame_request = false;
// 		}

// 		// EncodeFrame output.
// 		SFrameBSInfo info;
// 		memset(&info, 0, sizeof(SFrameBSInfo));
// 		//std::vector<uint8_t> frame_packet;
// 		cnv_frame_packet  frame_packet;
//          libyuv::ConvertToI420(frame_buffer->DataY(), 0, i420_buffer_->MutableDataY(),
//         i420_buffer_->StrideY(), i420_buffer_->MutableDataU(),
//         i420_buffer_->StrideU(), i420_buffer_->MutableDataV(),
//         i420_buffer_->StrideV(), 0, 0, configurations_[0].width, configurations_[0].height, configurations_[0].width,
//         configurations_[0].height, libyuv::kRotate0, libyuv::FOURCC_ABGR);
//         // s_client.webrtc_video(data, 48,  m_win_width, m_win_height);
//         // static FILE * out_file_yuv_ptr = fopen("./capture.yuv", "wb+");
//         // NORMAL_EX_LOG("get frame OK !!!");
//         // fwrite( i420_buffer_->DataY(), m_win_width * m_win_height * 3/2, 1,  out_file_yuv_ptr);
//         // fflush(out_file_yuv_ptr);

//         //////////////////////////////////
//         const NvEncInputFrame* encoderInputFrame = nv_encoders_[i]->GetNextInputFrame();
//         NV_ENC_INPUT_RESOURCE_OPENGL_TEX *pResource = (NV_ENC_INPUT_RESOURCE_OPENGL_TEX *)encoderInputFrame->inputPtr;
        
//         glBindTexture(pResource->target, pResource->texture);
//         glTexSubImage2D(pResource->target, 0, 0, 0, nWidth, nHeight * 3/2, GL_RED, GL_UNSIGNED_BYTE, i420_buffer_->DataY());
//         glBindTexture(pResource->target, 0);
        
//         frame_packet.frame.reset(new uint8_t[frame_buffer->width()* frame_buffer->height() * 4]);
//         std::vector<std::vector<uint8_t>> vPacket;
//         nv_encoders_[i]->EncodeFrame(vPacket);
//         NORMAL_EX_LOG("vPacket.size() = %u", vPacket.size());
//             //nFrame += (int)vPacket.size();
//         size_t cur_index = 0;
//         for (std::vector<uint8_t> &packet : vPacket)
//         {
//             memcpy(frame_packet.frame.get()+cur_index, &packet[0], packet.size());
//             cur_index+= packet.size(); 
//             //fpOut.write(reinterpret_cast<char*>(packet.data()), packet.size());
//             //fpOut.out.flush();
//         }
// 		//++m_key_frame_count;
// 		// if (!EncodeFrame((int)i, input_frame, frame_packet))
// 		// {
// 		// 	ERROR_EX_LOG("encode frame failed !!!");
// 		// 	return false;
// 		// }

// 		if (frame_packet.use_size == 0) 
// 		{
// 			return WEBRTC_VIDEO_CODEC_OK;
// 		}
// 		else {
// 			//NORMAL_EX_LOG("frame size = %u", frame_packet.use_size);
// 			if (frame_packet.frame.get()[4] == 0x67)
// 			{
// 				//NORMAL_EX_LOG(" I frame  = %u", m_key_frame_count);
// 				m_key_frame_count = 0;
// 				info.eFrameType = videoFrameTypeIDR;
// 			}
// 			else if (frame_packet.frame.get()[4] == 0x61)
// 			{
// 			//	NORMAL_EX_LOG(" P frame ");
// 				info.eFrameType = videoFrameTypeP;
// 			}
// 			else {
// 			//	NORMAL_EX_LOG(" B frame ");
// 				info.eFrameType = videoFrameTypeP;
// 			}
// 		}

// 		encoded_images_[i]._encodedWidth = configurations_[i].width;
// 		encoded_images_[i]._encodedHeight = configurations_[i].height;
// 		encoded_images_[i].SetTimestamp(input_frame.timestamp());
// 		encoded_images_[i].ntp_time_ms_ = input_frame.ntp_time_ms();
// 		encoded_images_[i].capture_time_ms_ = input_frame.render_time_ms();
// 		encoded_images_[i].rotation_ = input_frame.rotation();
// 		encoded_images_[i].SetColorSpace(input_frame.color_space());
// 		encoded_images_[i].content_type_ =
// 			(codec_.mode == VideoCodecMode::kScreensharing) ? VideoContentType::SCREENSHARE : VideoContentType::UNSPECIFIED;
// 		encoded_images_[i].timing_.flags = VideoSendTiming::kInvalid;
// 		encoded_images_[i]._frameType = ConvertToVideoFrameType(info.eFrameType);
// 		encoded_images_[i].SetSpatialIndex(configurations_[i].simulcast_idx);

// 		// Split encoded image up into fragments. This also updates
// 		// |encoded_image_|.
// 		RTPFragmentationHeader frag_header;
// 		RtpFragmentize(&encoded_images_[i], *frame_buffer, frame_packet , &frag_header);
// 		//RtpFragmentize_webrtc(&encoded_images_[i], *frame_buffer, &info, &frag_header);

// 		// Encoder can skip frames to save bandwidth in which case
// 		// |encoded_images_[i]._length| == 0.
// 		if (encoded_images_[i].size() > 0) {
// 			// Parse QP.
// 			h264_bitstream_parser_.ParseBitstream(encoded_images_[i].data(), encoded_images_[i].size());
// 			h264_bitstream_parser_.GetLastSliceQp(&encoded_images_[i].qp_);
// 			//encoded_images_[i].qp_ = 10;

// 			// Deliver encoded image.
// 			CodecSpecificInfo codec_specific;
// 			codec_specific.codecType = kVideoCodecH264;
// 			codec_specific.codecSpecific.H264.packetization_mode = packetization_mode_;
// 			codec_specific.codecSpecific.H264.temporal_idx = kNoTemporalIdx;
// 			codec_specific.codecSpecific.H264.idr_frame = info.eFrameType == videoFrameTypeIDR;
// 			codec_specific.codecSpecific.H264.base_layer_sync = true;

// 			// if (info.eFrameType == videoFrameTypeIDR &&
// 			//    encoded_images_[i]._frameType == kVideoFrameKey) {
// 			//  RTC_LOG(LS_ERROR) << "send idr frame - " << encoded_images_[i].size();
// 			//}

// 			encoded_image_callback_->OnEncodedImage(encoded_images_[i], &codec_specific, &frag_header);
// 		}
// 	}
// 	cur_time_ms = std::chrono::steady_clock::now();
// 	dur = cur_time_ms - pre_time;
// 	milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
// 	elapse = static_cast<uint32_t>(milliseconds.count());
// 	if (elapse > 3)
// 	{
// 		WARNING_EX_LOG("encoder video  frame  milliseconds = %lu", elapse);
// 	}
// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// void NvEncoder::ReportInit() 
// {
// 	if (has_reported_init_)
// 		return;
// 	RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.NvEncoder.Event",
// 							kH264EncoderEventInit, kH264EncoderEventMax);
// 	has_reported_init_ = true;
// }

// void NvEncoder::ReportError() 
// {
// 	if (has_reported_error_)
// 		return;
// 	RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.NvEncoder.Event",
// 							kH264EncoderEventError, kH264EncoderEventMax);
// 	has_reported_error_ = true;
// }

// VideoEncoder::EncoderInfo NvEncoder::GetEncoderInfo() const 
// {
// 	EncoderInfo info;
// 	info.supports_native_handle = false;
// 	info.implementation_name = "NvEncoder";
// 	info.scaling_settings = VideoEncoder::ScalingSettings(kLowH264QpThreshold, kHighH264QpThreshold);
// 	info.is_hardware_accelerated = true;
// 	info.has_internal_source = false;
// 	return info;
// }

// void NvEncoder::LayerConfig::SetStreamState(bool send_stream) 
// {
// 	if (send_stream && !sending) {
// 		// Need a key frame if we have not sent this stream before.
// 		key_frame_request = true;
// 	}
// 	sending = send_stream;
// }
// //static FILE *out_file_ptr = ::fopen("test_chensong2.rgb", "wb+");
// // bool NvEncoder::EncodeFrame(int index, const VideoFrame& input_frame, cnv_frame_packet& frame_packet
// // 							/*std::vector<uint8_t>& frame_packet*/) 
// // {
// // 	//frame_packet.clear();
// // 	//NORMAL_EX_LOG("");
// // 	if (nv_encoders_.empty() || !nv_encoders_[index])
// // 	{
// // 		ERROR_EX_LOG("nv encoder empty !!!");
// // 		return false;
// // 	}

// // 	/*
// // 	if (video_format_ == EVideoFormatType::videoFormatI420) {
// // 		if (image_buffer_ != nullptr) {
// // 			if (webrtc::ConvertFromI420(input_frame, webrtc::VideoType::kARGB, 0,
// // 										image_buffer_.get()) < 0) {
// // 				return false;
// // 			}
// // 		} 
// // 		else {
// // 			return false;
// // 		}
// // 	}
// // 	*/

// // 	int width = input_frame.width();
// // 	int height = input_frame.height();
// // 	ID3D11Texture2D* texture = nvenc_info.get_texture(nv_encoders_[index]);
// // 	ID3D11DeviceContext* context = nvenc_info.get_context(nv_encoders_[index]);
// // 	 if (texture && context  )
// // 	{
// // 		// NORMAL_EX_LOG("");
// // 		 int max_buffer_size = height * width * 4;
// // 		 //std::shared_ptr<uint8_t> out_buffer(new uint8_t[max_buffer_size]);
// // 		 frame_packet.frame.reset(new uint8_t[max_buffer_size]);
// // 		/* if (g_dxgi_format == DXGI_FORMAT_R10G10B10A2_UNORM)
// // 		 {
// // 			 NORMAL_EX_LOG("");
// // 			 D3D11_MAPPED_SUBRESOURCE dsec = { 0 };
// // 			 HRESULT hr = context->Map(texture, D3D11CalcSubresource(0, 0, 0), D3D11_MAP_WRITE, 0, &dsec);
// // 			 if (SUCCEEDED(hr))
// // 			 {

// // 				 NORMAL_EX_LOG("");
// // 				 libyuv::ARGBCopy(input_frame.video_frame_buffer()->ToI420()->DataY(), width * 4, (uint8_t*)dsec.pData, dsec.RowPitch, width, height);

// // 				 NORMAL_EX_LOG("");
// // 				 context->Unmap(texture, D3D11CalcSubresource(0, 0, 0));
// // 				 NORMAL_EX_LOG("");
// // 				 int frame_size = nvenc_info.encode_texture(nv_encoders_[index], texture, 0, frame_packet.frame.get(), max_buffer_size);
// // 				 NORMAL_EX_LOG("");
// // 				 frame_packet.use_size = frame_size;
// // 				 if (frame_packet.use_size< 1)
// // 				 {
// // 					 ERROR_EX_LOG("encoder texture  frame_size = %d !!!!", frame_size);
// // 					 return false;
// // 				 }

// // 			 }
// // 		 }
// // 		 else*/ if ( 0 != g_gpu_index  )
// // 		 {
// // 			// NORMAL_EX_LOG("");
// // 			 D3D11_MAPPED_SUBRESOURCE dsec = { 0 };
// // 			 HRESULT hr = context->Map(texture, D3D11CalcSubresource(0, 0, 0), D3D11_MAP_WRITE, 0, &dsec);
// // 			 if (SUCCEEDED(hr)) {

// // 				// NORMAL_EX_LOG("");
// // 				 libyuv::ARGBCopy(input_frame.video_frame_buffer()->ToI420()->DataY(), width * 4, (uint8_t*)dsec.pData, dsec.RowPitch, width, height);

// // 				// NORMAL_EX_LOG("");
// // 				 context->Unmap(texture, D3D11CalcSubresource(0, 0, 0));
// // 				// NORMAL_EX_LOG("");
// // 				 int frame_size =   nvenc_info.encode_texture(nv_encoders_[index], texture, 0, frame_packet.frame.get(), max_buffer_size);
// // 				// NORMAL_EX_LOG("");
// // 				 frame_packet.use_size = frame_size;
// // 				 if (frame_packet.use_size < 0) 
// // 				 {
// // 					 ERROR_EX_LOG("encoder texture  frame_size = %d !!!!", frame_size);
// // 					 return false;
// // 				 }

// // 			 }
			 
// // 		 }
// // 		 else if (input_frame.video_frame_buffer()->ToI420()->get_texture())
// // 		 {

// // 			 int frame_size = nvenc_info.encode_handle((void*)nv_encoders_[index], (HANDLE)input_frame.video_frame_buffer()->ToI420()->get_texture(), 0, 0, frame_packet.frame.get(), max_buffer_size); ;
// // 			 frame_packet.use_size = frame_size;
// // 			 if (frame_packet.use_size < 1)
// // 			 {
// // 				 ERROR_EX_LOG("encoder texture  frame_size = %d !!!!", frame_size);
// // 				 return false;
// // 			 }
// // 		 }
// // 		 else
// // 		 {
// // 			 WARNING_EX_LOG("encoder texture type error !!!");
// // 		 }
// // 	}
// // 	else
// // 	{
// // 		ERROR_EX_LOG("context failed !!!");
// // 		return false;
// // 	}

// // 	return true;
// // }

// }  // namespace webrtc




#include <limits>
#include <string>

#include "third_party/openh264/src/codec/api/svc/codec_api.h"
#include "third_party/openh264/src/codec/api/svc/codec_app_def.h"
#include "third_party/openh264/src/codec/api/svc/codec_def.h"
#include "third_party/openh264/src/codec/api/svc/codec_ver.h"

#include "absl/strings/match.h"
#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "modules/video_coding/utility/simulcast_rate_allocator.h"
#include "modules/video_coding/utility/simulcast_utility.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"
#include "system_wrappers/include/metrics.h"
#include "third_party/libyuv/include/libyuv/convert.h"
#include "third_party/libyuv/include/libyuv/scale.h"

using namespace chen;
namespace webrtc {
struct cnv_frame_packet
{
    std::shared_ptr<uint8_t>   frame;
    uint32_t                   use_size;
    cnv_frame_packet()
        : frame()
        , use_size(0)
    {
    }
};
namespace {

const bool kOpenH264EncoderDetailedLogging = false;

// QP scaling thresholds.
static const int kLowH264QpThreshold = 24;
static const int kHighH264QpThreshold = 37;

// Used by histograms. Values of entries should not be changed.
enum H264EncoderImplEvent {
  kH264EncoderEventInit = 0,
  kH264EncoderEventError = 1,
  kH264EncoderEventMax = 16,
};

int NumberOfThreads(int width, int height, int number_of_cores) {
  // TODO(hbos): In Chromium, multiple threads do not work with sandbox on Mac,
  // see crbug.com/583348. Until further investigated, only use one thread.
  //  if (width * height >= 1920 * 1080 && number_of_cores > 8) {
  //    return 8;  // 8 threads for 1080p on high perf machines.
  //  } else if (width * height > 1280 * 960 && number_of_cores >= 6) {
  //    return 3;  // 3 threads for 1080p.
  //  } else if (width * height > 640 * 480 && number_of_cores >= 3) {
  //    return 2;  // 2 threads for qHD/HD.
  //  } else {
  //    return 1;  // 1 thread for VGA or less.
  //  }
  // TODO(sprang): Also check sSliceArgument.uiSliceNum om GetEncoderPrams(),
  //               before enabling multithreading here.
  return 1;
}

FrameType ConvertToVideoFrameType(EVideoFrameType type) {
  switch (type) {
    case videoFrameTypeIDR:
      return kVideoFrameKey;
    case videoFrameTypeSkip:
    case videoFrameTypeI:
    case videoFrameTypeP:
    case videoFrameTypeIPMixed:
      return kVideoFrameDelta;
    case videoFrameTypeInvalid:
      break;
  }
  RTC_NOTREACHED() << "Unexpected/invalid frame type: " << type;
  return kEmptyFrame;
}

}  // namespace

// Helper method used by H264EncoderImpl::Encode.
// Copies the encoded bytes from |info| to |encoded_image| and updates the
// fragmentation information of |frag_header|. The |encoded_image->_buffer| may
// be deleted and reallocated if a bigger buffer is required.
//
// After OpenH264 encoding, the encoded bytes are stored in |info| spread out
// over a number of layers and "NAL units". Each NAL unit is a fragment starting
// with the four-byte start code {0,0,0,1}. All of this data (including the
// start codes) is copied to the |encoded_image->_buffer| and the |frag_header|
// is updated to point to each fragment, with offsets and lengths set as to
// exclude the start codes.
static void RtpFragmentize(EncodedImage* encoded_image,
                           const VideoFrameBuffer& frame_buffer,
                           SFrameBSInfo* info,
                           RTPFragmentationHeader* frag_header) {
  // Calculate minimum buffer size required to hold encoded data.
  size_t required_capacity = 0;
  size_t fragments_count = 0;
  for (int layer = 0; layer < info->iLayerNum; ++layer) {
    const SLayerBSInfo& layerInfo = info->sLayerInfo[layer];
    for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++fragments_count) {
      RTC_CHECK_GE(layerInfo.pNalLengthInByte[nal], 0);
      // Ensure |required_capacity| will not overflow.
      RTC_CHECK_LE(layerInfo.pNalLengthInByte[nal],
                   std::numeric_limits<size_t>::max() - required_capacity);
      required_capacity += layerInfo.pNalLengthInByte[nal];
    }
  }
  if (encoded_image->capacity() < required_capacity) {
    // Increase buffer size. Allocate enough to hold an unencoded image, this
    // should be more than enough to hold any encoded data of future frames of
    // the same size (avoiding possible future reallocation due to variations in
    // required size).
    size_t new_capacity = CalcBufferSize(VideoType::kI420, frame_buffer.width(),
                                         frame_buffer.height());
    if (new_capacity < required_capacity) {
      // Encoded data > unencoded data. Allocate required bytes.
      RTC_LOG(LS_WARNING)
          << "Encoding produced more bytes than the original image "
          << "data! Original bytes: " << new_capacity
          << ", encoded bytes: " << required_capacity << ".";
      new_capacity = required_capacity;
    }
    encoded_image->Allocate(new_capacity);
  }

  // Iterate layers and NAL units, note each NAL unit as a fragment and copy
  // the data to |encoded_image->_buffer|.
  const uint8_t start_code[4] = {0, 0, 0, 1};
  frag_header->VerifyAndAllocateFragmentationHeader(fragments_count);
  size_t frag = 0;
  encoded_image->set_size(0);
  for (int layer = 0; layer < info->iLayerNum; ++layer) {
    const SLayerBSInfo& layerInfo = info->sLayerInfo[layer];
    // Iterate NAL units making up this layer, noting fragments.
    size_t layer_len = 0;
    for (int nal = 0; nal < layerInfo.iNalCount; ++nal, ++frag) {
      // Because the sum of all layer lengths, |required_capacity|, fits in a
      // |size_t|, we know that any indices in-between will not overflow.
      RTC_DCHECK_GE(layerInfo.pNalLengthInByte[nal], 4);
      RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 0], start_code[0]);
      RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 1], start_code[1]);
      RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 2], start_code[2]);
      RTC_DCHECK_EQ(layerInfo.pBsBuf[layer_len + 3], start_code[3]);
      frag_header->fragmentationOffset[frag] =
          encoded_image->size() + layer_len + sizeof(start_code);
      frag_header->fragmentationLength[frag] =
          layerInfo.pNalLengthInByte[nal] - sizeof(start_code);
      layer_len += layerInfo.pNalLengthInByte[nal];
    }
    // Copy the entire layer's data (including start codes).
    memcpy(encoded_image->data() + encoded_image->size(), layerInfo.pBsBuf,
           layer_len);
    encoded_image->set_size(encoded_image->size() + layer_len);
  }
}

cnv_encoder::cnv_encoder(const cricket::VideoCodec& codec)
    : packetization_mode_(H264PacketizationMode::SingleNalUnit),
      max_payload_size_(0),
      number_of_cores_(0),
      encoded_image_callback_(nullptr),
      has_reported_init_(false),
      has_reported_error_(false),
      num_temporal_layers_(1),
      tl0sync_limit_(0) {
  RTC_CHECK(absl::EqualsIgnoreCase(codec.name, cricket::kH264CodecName));
  std::string packetization_mode_string;
  if (codec.GetParam(cricket::kH264FmtpPacketizationMode,
                     &packetization_mode_string) &&
      packetization_mode_string == "1") {
    packetization_mode_ = H264PacketizationMode::NonInterleaved;
  }
  downscaled_buffers_.reserve(kMaxSimulcastStreams - 1);
  encoded_images_.reserve(kMaxSimulcastStreams);
  encoders_.reserve(kMaxSimulcastStreams);
  configurations_.reserve(kMaxSimulcastStreams);
}

cnv_encoder::~cnv_encoder() {
  Release();
}

int32_t cnv_encoder::InitEncode(const VideoCodec* inst,
                                    int32_t number_of_cores,
                                    size_t max_payload_size) {
  ReportInit();
  if (!inst || inst->codecType != kVideoCodecH264) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }
  if (inst->maxFramerate == 0) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }
  if (inst->width < 1 || inst->height < 1) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;
  }

  int32_t release_ret = Release();
  if (release_ret != WEBRTC_VIDEO_CODEC_OK) {
    ReportError();
    return release_ret;
  }

  int number_of_streams = SimulcastUtility::NumberOfSimulcastStreams(*inst);
  bool doing_simulcast = (number_of_streams > 1);

  if (doing_simulcast &&
      !SimulcastUtility::ValidSimulcastParameters(*inst, number_of_streams)) {
    return WEBRTC_VIDEO_CODEC_ERR_SIMULCAST_PARAMETERS_NOT_SUPPORTED;
  }
  downscaled_buffers_.resize(number_of_streams - 1);
  encoded_images_.resize(number_of_streams);
  encoders_.resize(number_of_streams);
  pictures_.resize(number_of_streams);
  configurations_.resize(number_of_streams);

  number_of_cores_ = number_of_cores;
  max_payload_size_ = max_payload_size;
  codec_ = *inst;

  // Code expects simulcastStream resolutions to be correct, make sure they are
  // filled even when there are no simulcast layers.
  if (codec_.numberOfSimulcastStreams == 0) {
    codec_.simulcastStream[0].width = codec_.width;
    codec_.simulcastStream[0].height = codec_.height;
  }

  num_temporal_layers_ = codec_.H264()->numberOfTemporalLayers;
        init_gl();
    NORMAL_EX_LOG("");
    std::ostringstream oss;
    oss << "-codec h264 -fps 25 ";
    NvEncoderInitParam encodeCLIOptions(oss.str().c_str());
  for (int i = 0, idx = number_of_streams - 1; i < number_of_streams; ++i, --idx) 
  {
    // ISVCEncoder* openh264_encoder;
    // // Create encoder.
    // if (WelsCreateSVCEncoder(&openh264_encoder) != 0) {
    //   // Failed to create encoder.
    //   RTC_LOG(LS_ERROR) << "Failed to create OpenH264 encoder";
    //   RTC_DCHECK(!openh264_encoder);
    //   Release();
    //   ReportError();
    //   return WEBRTC_VIDEO_CODEC_ERROR;
    // }
    // RTC_DCHECK(openh264_encoder);
    // if (kOpenH264EncoderDetailedLogging) {
    //   int trace_level = WELS_LOG_DETAIL;
    //   openh264_encoder->SetOption(ENCODER_OPTION_TRACE_LEVEL, &trace_level);
    // }
    // // else WELS_LOG_DEFAULT is used by default.

    // // Store h264 encoder.
    // encoders_[i] = openh264_encoder;

    // Set internal settings from codec_settings
    configurations_[i].simulcast_idx = idx;
    configurations_[i].sending = false;
    configurations_[i].width = codec_.simulcastStream[idx].width;
    configurations_[i].height = codec_.simulcastStream[idx].height;
    configurations_[i].max_frame_rate = static_cast<float>(codec_.maxFramerate);
    configurations_[i].frame_dropping_on = codec_.H264()->frameDroppingOn;
    configurations_[i].key_frame_interval = codec_.H264()->keyFrameInterval;

    // Create downscaled image buffers.
    if (i > 0) 
    {
      downscaled_buffers_[i - 1] = I420Buffer::Create(
          configurations_[i].width, configurations_[i].height,
          configurations_[i].width, configurations_[i].width / 2,
          configurations_[i].width / 2);
    }

    // Codec_settings uses kbits/second; encoder uses bits/second.
    configurations_[i].max_bps = codec_.maxBitrate * 1000;
    configurations_[i].target_bps = codec_.startBitrate * 1000;

    encoders_[i] = new NvEncoderGL  (configurations_[i].width, configurations_[i].height, NV_ENC_BUFFER_FORMAT_IYUV);
    NORMAL_EX_LOG("");
    NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
    NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
    initializeParams.encodeConfig = &encodeConfig;
    ((NvEncoderGL*)encoders_[i])->CreateDefaultEncoderParams(&initializeParams, encodeCLIOptions.GetEncodeGUID(), encodeCLIOptions.GetPresetGUID());
    NORMAL_EX_LOG("");
    encodeCLIOptions.SetInitParams(&initializeParams, NV_ENC_BUFFER_FORMAT_IYUV);
    NORMAL_EX_LOG("");
    ((NvEncoderGL*)encoders_[i])->CreateEncoder(&initializeParams);

    // Create encoder parameters based on the layer configuration.
    // SEncParamExt encoder_params = CreateEncoderParams(i);

    // Initialize.
    // if (openh264_encoder->InitializeExt(&encoder_params) != 0) {
    //   RTC_LOG(LS_ERROR) << "Failed to initialize OpenH264 encoder";
    //   Release();
    //   ReportError();
    //   return WEBRTC_VIDEO_CODEC_ERROR;
    // }
    // // TODO(pbos): Base init params on these values before submitting.
    // int video_format = EVideoFormatType::videoFormatI420;
    // openh264_encoder->SetOption(ENCODER_OPTION_DATAFORMAT, &video_format);

    // Initialize encoded image. Default buffer size: size of unencoded data.

    const size_t new_capacity =
        CalcBufferSize(VideoType::kI420, codec_.simulcastStream[idx].width,
                       codec_.simulcastStream[idx].height);
    encoded_images_[i].Allocate(new_capacity);
    encoded_images_[i]._completeFrame = true;
    encoded_images_[i]._encodedWidth = codec_.simulcastStream[idx].width;
    encoded_images_[i]._encodedHeight = codec_.simulcastStream[idx].height;
    encoded_images_[i].set_size(0);
  }

  SimulcastRateAllocator init_allocator(codec_);
  VideoBitrateAllocation allocation = init_allocator.GetAllocation(
      codec_.startBitrate * 1000, codec_.maxFramerate);
  return SetRateAllocation(allocation, codec_.maxFramerate);
}

int32_t cnv_encoder::Release() {
  while (!encoders_.empty()) {
    NvEncoderGL* openh264_encoder = encoders_.back();
    if (openh264_encoder) {
    //   RTC_CHECK_EQ(0, openh264_encoder->Uninitialize());
    //   WelsDestroySVCEncoder(openh264_encoder);
    }
    encoders_.pop_back();
  }
  downscaled_buffers_.clear();
  configurations_.clear();
  encoded_images_.clear();
  pictures_.clear();
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t cnv_encoder::RegisterEncodeCompleteCallback(
    EncodedImageCallback* callback) {
  encoded_image_callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t cnv_encoder::SetRateAllocation(
    const VideoBitrateAllocation& bitrate,
    uint32_t new_framerate) {
  if (encoders_.empty())
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;

  if (new_framerate < 1)
    return WEBRTC_VIDEO_CODEC_ERR_PARAMETER;

  if (bitrate.get_sum_bps() == 0) {
    // Encoder paused, turn off all encoding.
    for (size_t i = 0; i < configurations_.size(); ++i)
      configurations_[i].SetStreamState(false);
    return WEBRTC_VIDEO_CODEC_OK;
  }

  // At this point, bitrate allocation should already match codec settings.
  if (codec_.maxBitrate > 0)
    RTC_DCHECK_LE(bitrate.get_sum_kbps(), codec_.maxBitrate);
  RTC_DCHECK_GE(bitrate.get_sum_kbps(), codec_.minBitrate);
  if (codec_.numberOfSimulcastStreams > 0)
    RTC_DCHECK_GE(bitrate.get_sum_kbps(), codec_.simulcastStream[0].minBitrate);

  codec_.maxFramerate = new_framerate;

  size_t stream_idx = encoders_.size() - 1;
  for (size_t i = 0; i < encoders_.size(); ++i, --stream_idx) {
    // Update layer config.
    configurations_[i].target_bps = bitrate.GetSpatialLayerSum(stream_idx);
    configurations_[i].max_frame_rate = static_cast<float>(new_framerate);

    if (configurations_[i].target_bps) {
      configurations_[i].SetStreamState(true);

      // Update h264 encoder.
      SBitrateInfo target_bitrate;
      memset(&target_bitrate, 0, sizeof(SBitrateInfo));
      target_bitrate.iLayer = SPATIAL_LAYER_ALL,
      target_bitrate.iBitrate = configurations_[i].target_bps;
    //   encoders_[i]->SetOption(ENCODER_OPTION_BITRATE, &target_bitrate);
    //   encoders_[i]->SetOption(ENCODER_OPTION_FRAME_RATE,
    //                           &configurations_[i].max_frame_rate);
    } else {
      configurations_[i].SetStreamState(false);
    }
  }

  return WEBRTC_VIDEO_CODEC_OK;
}




static void RtpFragmentize(EncodedImage* encoded_image,
	const VideoFrameBuffer& frame_buffer, cnv_frame_packet&  frame_packet
	/*std::vector<uint8_t>& frame_packet*/,
	RTPFragmentationHeader* frag_header)
{
	
	size_t required_capacity = 0;
	encoded_image->set_size(0);

	required_capacity = frame_packet.use_size;
	//encoded_image->Allocate(required_capacity);

	// TODO(nisse): Use a cache or buffer pool to avoid allocation?
	encoded_image->Allocate(required_capacity);


	///////////////////////////////////////H264 NAL///////////////////////////////////////////////////////

	memcpy(encoded_image->data() + encoded_image->size(), &frame_packet.frame.get()[0], frame_packet.use_size);
	encoded_image->set_size(encoded_image->size() + frame_packet.use_size);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::vector<webrtc::H264::NaluIndex> nalus = webrtc::H264::FindNaluIndices(
		encoded_image->data(), encoded_image->size());
	size_t fragments_count = nalus.size();
	// const uint8_t start_code[4] = {0, 0, 0, 1};
	frag_header->VerifyAndAllocateFragmentationHeader(fragments_count);
	for (size_t i = 0; i < nalus.size(); i++) {
		frag_header->fragmentationOffset[i] = nalus[i].payload_start_offset;
		frag_header->fragmentationLength[i] = nalus[i].payload_size;
	}
}
int32_t cnv_encoder::Encode(const VideoFrame& input_frame,
                                const CodecSpecificInfo* codec_specific_info,
                                const std::vector<FrameType>* frame_types) {
  if (encoders_.empty()) {
    ReportError();
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }
  if (!encoded_image_callback_) {
    RTC_LOG(LS_WARNING)
        << "InitEncode() has been called, but a callback function "
        << "has not been set with RegisterEncodeCompleteCallback()";
    ReportError();
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  rtc::scoped_refptr<const I420BufferInterface> frame_buffer =
      input_frame.video_frame_buffer()->ToI420();

  bool send_key_frame = false;
  for (size_t i = 0; i < configurations_.size(); ++i) {
    if (configurations_[i].key_frame_request && configurations_[i].sending) {
      send_key_frame = true;
      break;
    }
  }
  if (!send_key_frame && frame_types) {
    for (size_t i = 0; i < frame_types->size() && i < configurations_.size();
         ++i) {
      if ((*frame_types)[i] == kVideoFrameKey && configurations_[i].sending) {
        send_key_frame = true;
        break;
      }
    }
  }

  RTC_DCHECK_EQ(configurations_[0].width, frame_buffer->width());
  RTC_DCHECK_EQ(configurations_[0].height, frame_buffer->height());

  // Encode image for each layer.
  for (size_t i = 0; i < encoders_.size(); ++i) {
    // EncodeFrame input.
    // pictures_[i] = {0};
    // pictures_[i].iPicWidth = configurations_[i].width;
    // pictures_[i].iPicHeight = configurations_[i].height;
    // pictures_[i].iColorFormat = EVideoFormatType::videoFormatI420;
    // pictures_[i].uiTimeStamp = input_frame.ntp_time_ms();
    // // Downscale images on second and ongoing layers.
    // if (i == 0) {
    //   pictures_[i].iStride[0] = frame_buffer->StrideY();
    //   pictures_[i].iStride[1] = frame_buffer->StrideU();
    //   pictures_[i].iStride[2] = frame_buffer->StrideV();
    //   pictures_[i].pData[0] = const_cast<uint8_t*>(frame_buffer->DataY());
    //   pictures_[i].pData[1] = const_cast<uint8_t*>(frame_buffer->DataU());
    //   pictures_[i].pData[2] = const_cast<uint8_t*>(frame_buffer->DataV());
    // } else {
    //   pictures_[i].iStride[0] = downscaled_buffers_[i - 1]->StrideY();
    //   pictures_[i].iStride[1] = downscaled_buffers_[i - 1]->StrideU();
    //   pictures_[i].iStride[2] = downscaled_buffers_[i - 1]->StrideV();
    //   pictures_[i].pData[0] =
    //       const_cast<uint8_t*>(downscaled_buffers_[i - 1]->DataY());
    //   pictures_[i].pData[1] =
    //       const_cast<uint8_t*>(downscaled_buffers_[i - 1]->DataU());
    //   pictures_[i].pData[2] =
    //       const_cast<uint8_t*>(downscaled_buffers_[i - 1]->DataV());
    //   // Scale the image down a number of times by downsampling factor.
    //   libyuv::I420Scale(pictures_[i - 1].pData[0], pictures_[i - 1].iStride[0],
    //                     pictures_[i - 1].pData[1], pictures_[i - 1].iStride[1],
    //                     pictures_[i - 1].pData[2], pictures_[i - 1].iStride[2],
    //                     configurations_[i - 1].width,
    //                     configurations_[i - 1].height, pictures_[i].pData[0],
    //                     pictures_[i].iStride[0], pictures_[i].pData[1],
    //                     pictures_[i].iStride[1], pictures_[i].pData[2],
    //                     pictures_[i].iStride[2], configurations_[i].width,
    //                     configurations_[i].height, libyuv::kFilterBilinear);
    // }

    // if (!configurations_[i].sending) {
    //   continue;
    // }
    // if (frame_types != nullptr) {
    //   // Skip frame?
    //   if ((*frame_types)[i] == kEmptyFrame) {
    //     continue;
    //   }
    // }
    // if (send_key_frame) {
    //   // API doc says ForceIntraFrame(false) does nothing, but calling this
    //   // function forces a key frame regardless of the |bIDR| argument's value.
    //   // (If every frame is a key frame we get lag/delays.)
    //   encoders_[i]->ForceIntraFrame(true);
    //   configurations_[i].key_frame_request = false;
    // }
    // // EncodeFrame output.
    SFrameBSInfo info;
    memset(&info, 0, sizeof(SFrameBSInfo));

    // // Encode!
    // int enc_ret = encoders_[i]->EncodeFrame(&pictures_[i], &info);
    // if (enc_ret != 0) {
    //   RTC_LOG(LS_ERROR)
    //       << "OpenH264 frame encoding failed, EncodeFrame returned " << enc_ret
    //       << ".";
    //   ReportError();
    //   return WEBRTC_VIDEO_CODEC_ERROR;
    // }





        const NvEncInputFrame* encoderInputFrame = ((NvEncoderGL*)encoders_[i])->GetNextInputFrame();
        NV_ENC_INPUT_RESOURCE_OPENGL_TEX *pResource = (NV_ENC_INPUT_RESOURCE_OPENGL_TEX *)encoderInputFrame->inputPtr;
        
        glBindTexture(pResource->target, pResource->texture);
        glTexSubImage2D(pResource->target, 0, 0, 0, frame_buffer->width(), frame_buffer->height() * 3/2, GL_RED, GL_UNSIGNED_BYTE, frame_buffer->DataY());
        glBindTexture(pResource->target, 0);
        cnv_frame_packet  frame_packet;
        frame_packet.frame.reset(new uint8_t[frame_buffer->width()* frame_buffer->height() * 4]);
        std::vector<std::vector<uint8_t>> vPacket;
        ((NvEncoderGL*)encoders_[i])->EncodeFrame(vPacket);
        NORMAL_EX_LOG("vPacket.size() = %u", vPacket.size());
            //nFrame += (int)vPacket.size();
        size_t cur_index = 0;
        for (std::vector<uint8_t> &packet : vPacket)
        {
            memcpy(frame_packet.frame.get()+cur_index, &packet[0], packet.size());
            cur_index+= packet.size(); 
            //fpOut.write(reinterpret_cast<char*>(packet.data()), packet.size());
            //fpOut.out.flush();
        }
        frame_packet.use_size = cur_index;


            if (frame_packet.frame.get()[4] == 0x67)
			{
				//NORMAL_EX_LOG(" I frame  = %u", m_key_frame_count);
				// m_key_frame_count = 0;
				info.eFrameType = videoFrameTypeIDR;
			}
			else if (frame_packet.frame.get()[4] == 0x61)
			{
			//	NORMAL_EX_LOG(" P frame ");
				info.eFrameType = videoFrameTypeP;
			}
			else {
			//	NORMAL_EX_LOG(" B frame ");
				info.eFrameType = videoFrameTypeP;
			}



    encoded_images_[i]._encodedWidth = configurations_[i].width;
    encoded_images_[i]._encodedHeight = configurations_[i].height;
    encoded_images_[i].SetTimestamp(input_frame.timestamp());
    encoded_images_[i].ntp_time_ms_ = input_frame.ntp_time_ms();
    encoded_images_[i].capture_time_ms_ = input_frame.render_time_ms();
    encoded_images_[i].rotation_ = input_frame.rotation();
    encoded_images_[i].SetColorSpace(input_frame.color_space());
    encoded_images_[i].content_type_ =
            (codec_.mode == VideoCodecMode::kScreensharing)
            ? VideoContentType::SCREENSHARE
            : VideoContentType::UNSPECIFIED;
    encoded_images_[i].timing_.flags = VideoSendTiming::kInvalid;
    encoded_images_[i]._frameType = ConvertToVideoFrameType(info.eFrameType);
    encoded_images_[i].SetSpatialIndex(configurations_[i].simulcast_idx);

RTPFragmentationHeader frag_header;
		RtpFragmentize(&encoded_images_[i], *frame_buffer, frame_packet , &frag_header);
    // Split encoded image up into fragments. This also updates
    // |encoded_image_|.
    // RTPFragmentationHeader frag_header;
    // RtpFragmentize(&encoded_images_[i], *frame_buffer, &info, &frag_header);

    // Encoder can skip frames to save bandwidth in which case
    // |encoded_images_[i]._length| == 0.
    if (encoded_images_[i].size() > 0) {
      // Parse QP.
      h264_bitstream_parser_.ParseBitstream(encoded_images_[i].data(),
                                            encoded_images_[i].size());
      h264_bitstream_parser_.GetLastSliceQp(&encoded_images_[i].qp_);

      // Deliver encoded image.
      CodecSpecificInfo codec_specific;
      codec_specific.codecType = kVideoCodecH264;
      codec_specific.codecSpecific.H264.packetization_mode =
          packetization_mode_;
      codec_specific.codecSpecific.H264.temporal_idx = kNoTemporalIdx;
      codec_specific.codecSpecific.H264.idr_frame =
          info.eFrameType == videoFrameTypeIDR;
      codec_specific.codecSpecific.H264.base_layer_sync = false;
      if (num_temporal_layers_ > 1) {
        const uint8_t tid = info.sLayerInfo[0].uiTemporalId;
        codec_specific.codecSpecific.H264.temporal_idx = tid;
        codec_specific.codecSpecific.H264.base_layer_sync =
            tid > 0 && tid < tl0sync_limit_;
        if (codec_specific.codecSpecific.H264.base_layer_sync) {
          tl0sync_limit_ = tid;
        }
        if (tid == 0) {
          tl0sync_limit_ = num_temporal_layers_;
        }
      }
      encoded_image_callback_->OnEncodedImage(encoded_images_[i],
                                              &codec_specific, &frag_header);
    }
  }
  return WEBRTC_VIDEO_CODEC_OK;
}

// Initialization parameters.
// There are two ways to initialize. There is SEncParamBase (cleared with
// memset(&p, 0, sizeof(SEncParamBase)) used in Initialize, and SEncParamExt
// which is a superset of SEncParamBase (cleared with GetDefaultParams) used
// in InitializeExt.
SEncParamExt cnv_encoder::CreateEncoderParams(size_t i) const {
  SEncParamExt encoder_params;
//   encoders_[i]->GetDefaultParams(&encoder_params);
//   if (codec_.mode == VideoCodecMode::kRealtimeVideo) {
//     encoder_params.iUsageType = CAMERA_VIDEO_REAL_TIME;
//   } else if (codec_.mode == VideoCodecMode::kScreensharing) {
//     encoder_params.iUsageType = SCREEN_CONTENT_REAL_TIME;
//   } else {
//     RTC_NOTREACHED();
//   }
//   encoder_params.iPicWidth = configurations_[i].width;
//   encoder_params.iPicHeight = configurations_[i].height;
//   encoder_params.iTargetBitrate = configurations_[i].target_bps;
//   encoder_params.iMaxBitrate = configurations_[i].max_bps;
//   // Rate Control mode
//   encoder_params.iRCMode = RC_BITRATE_MODE;
//   encoder_params.fMaxFrameRate = configurations_[i].max_frame_rate;

//   // The following parameters are extension parameters (they're in SEncParamExt,
//   // not in SEncParamBase).
//   encoder_params.bEnableFrameSkip = configurations_[i].frame_dropping_on;
//   // |uiIntraPeriod|    - multiple of GOP size
//   // |keyFrameInterval| - number of frames
//   encoder_params.uiIntraPeriod = configurations_[i].key_frame_interval;
//   encoder_params.uiMaxNalSize = 0;
//   // Threading model: use auto.
//   //  0: auto (dynamic imp. internal encoder)
//   //  1: single thread (default value)
//   // >1: number of threads
//   encoder_params.iMultipleThreadIdc = NumberOfThreads(
//       encoder_params.iPicWidth, encoder_params.iPicHeight, number_of_cores_);
//   // The base spatial layer 0 is the only one we use.
//   encoder_params.sSpatialLayers[0].iVideoWidth = encoder_params.iPicWidth;
//   encoder_params.sSpatialLayers[0].iVideoHeight = encoder_params.iPicHeight;
//   encoder_params.sSpatialLayers[0].fFrameRate = encoder_params.fMaxFrameRate;
//   encoder_params.sSpatialLayers[0].iSpatialBitrate =
//       encoder_params.iTargetBitrate;
//   encoder_params.sSpatialLayers[0].iMaxSpatialBitrate =
//       encoder_params.iMaxBitrate;
//   encoder_params.iTemporalLayerNum = num_temporal_layers_;
//   if (encoder_params.iTemporalLayerNum > 1) {
//     encoder_params.iNumRefFrame = 1;
//   }
//   RTC_LOG(INFO) << "OpenH264 version is " << OPENH264_MAJOR << "."
//                 << OPENH264_MINOR;
//   switch (packetization_mode_) {
//     case H264PacketizationMode::SingleNalUnit:
//       // Limit the size of the packets produced.
//       encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceNum = 1;
//       encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceMode =
//           SM_SIZELIMITED_SLICE;
//       encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceSizeConstraint =
//           static_cast<unsigned int>(max_payload_size_);
//       RTC_LOG(INFO) << "Encoder is configured with NALU constraint: "
//                     << max_payload_size_ << " bytes";
//       break;
//     case H264PacketizationMode::NonInterleaved:
//       // When uiSliceMode = SM_FIXEDSLCNUM_SLICE, uiSliceNum = 0 means auto
//       // design it with cpu core number.
//       // TODO(sprang): Set to 0 when we understand why the rate controller borks
//       //               when uiSliceNum > 1.
//       encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceNum = 1;
//       encoder_params.sSpatialLayers[0].sSliceArgument.uiSliceMode =
//           SM_FIXEDSLCNUM_SLICE;
//       break;
//   }
  return encoder_params;
}

void cnv_encoder::ReportInit() {
  if (has_reported_init_)
    return;
  RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.H264EncoderImpl.Event",
                            kH264EncoderEventInit, kH264EncoderEventMax);
  has_reported_init_ = true;
}

void cnv_encoder::ReportError() {
  if (has_reported_error_)
    return;
  RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.H264EncoderImpl.Event",
                            kH264EncoderEventError, kH264EncoderEventMax);
  has_reported_error_ = true;
}

VideoEncoder::EncoderInfo cnv_encoder::GetEncoderInfo() const {
  EncoderInfo info;
  info.supports_native_handle = false;
  info.implementation_name = "OpenH264";
  info.scaling_settings =
      VideoEncoder::ScalingSettings(kLowH264QpThreshold, kHighH264QpThreshold);
  info.is_hardware_accelerated = false;
  info.has_internal_source = false;
  return info;
}

void cnv_encoder::LayerConfig::SetStreamState(bool send_stream) {
  if (send_stream && !sending) {
    // Need a key frame if we have not sent this stream before.
    key_frame_request = true;
  }
  sending = send_stream;
}

}  // namespace webrtc