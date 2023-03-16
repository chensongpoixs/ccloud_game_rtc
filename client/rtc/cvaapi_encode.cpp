// /***********************************************************************************************
// 				created: 		2019-03-01

// 				author:			chensong

// 				purpose:		net_types
// ************************************************************************************************/
// #include "cvaapi_encode.h"
// #if defined(__linux__)

// #include <limits>
// #include <string>

// #include "third_party/openh264/src/codec/api/svc/codec_api.h"
// #include "third_party/openh264/src/codec/api/svc/codec_app_def.h"
// #include "third_party/openh264/src/codec/api/svc/codec_def.h"
// #include "third_party/openh264/src/codec/api/svc/codec_ver.h"
// #include "absl/strings/match.h"
// #include "common_video/h264/h264_common.h"
// #include "common_video/libyuv/include/webrtc_libyuv.h"
// #include "modules/video_coding/utility/simulcast_rate_allocator.h"
// #include "modules/video_coding/utility/simulcast_utility.h"
// #include "rtc_base/checks.h"
// #include "rtc_base/logging.h"
// #include "rtc_base/time_utils.h"
// #include "system_wrappers/include/metrics.h"
// #include "third_party/libyuv/include/libyuv/convert.h"
// #include "third_party/libyuv/include/libyuv/scale.h"
// #include "third_party/libyuv/include/libyuv.h"
// #include "ccfg.h"
// #include "clog.h"
// namespace webrc {

  

// // using namespace webrtc;


// cvaapi_encode::cvaapi_encode(const cricket::VideoCodec& codec)
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

// cvaapi_encode::~cvaapi_encode() 
// {
// 	Release();
// }

// int32_t cvaapi_encode::InitEncode(const webrtc::VideoCodec* inst,
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

// 	num_temporal_layers_ = codec_.H264()->numberOfTemporalLayers;

// 	for (int i = 0, idx = number_of_streams - 1; i < number_of_streams; ++i, --idx) {
// 		// Store nvidia encoder.
// 		// nv_encoders_[i] = nvenc_info.create();

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

// 		encoder_config nvenc_config;
// 		nvenc_config.codec = "h264";
// 		// 设置像素格式
// 		nvenc_config.format =  g_dxgi_format;  //DXGI_FORMAT_B8G8R8A8_UNORM;
// 		NORMAL_EX_LOG("[format = %u]", g_dxgi_format);
// 		nvenc_config.width = configurations_[i].width;
// 		nvenc_config.height = configurations_[i].height;
// 		nvenc_config.framerate = (uint32_t)configurations_[i].max_frame_rate;
// 		//TODO@chensong 2022-03-18 设置GOP 的大小 哈 ^_^ 不知道为什么 webrtc中设置gop key_frame_interval 字段是有效的、
// 		//根据程序
// 		using namespace chen;
// 	//	nvenc_config.gop = NVENC_INFINITE_GOPLENGTH; // g_cfg.get_uint32(ECI_EncoderVideoGop);// configurations_[i].key_frame_interval;
// 		nvenc_config.bitrate = 100000;
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

// int32_t cvaapi_encode::Release() 
// {
// 	// while (!nv_encoders_.empty()) 
// 	// {
// 	// 	void* nv_encoder = nv_encoders_.back();
// 	// 	if (nv_encoder) {
// 	// 		nvenc_info.destroy(&nv_encoder);
// 	// 		nv_encoder = nullptr;
// 	// 	}
// 	// 	nv_encoders_.pop_back();
// 	// }

// 	configurations_.clear();
// 	encoded_images_.clear();

// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// int32_t cvaapi_encode::RegisterEncodeCompleteCallback(EncodedImageCallback* callback) 
// {
// 	encoded_image_callback_ = callback;
// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// void cvaapi_encode::SetRates(const RateControlParameters& parameters)
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

// 	// size_t stream_idx = nv_encoders_.size() - 1;
// 	// for (size_t i = 0; i < nv_encoders_.size(); ++i, --stream_idx) {
// 	// 	configurations_[i].target_bps = parameters.bitrate.GetSpatialLayerSum(stream_idx);
// 	// 	configurations_[i].max_frame_rate = static_cast<float>(parameters.framerate_fps);

// 	// 	if (configurations_[i].target_bps) {
// 	// 		configurations_[i].SetStreamState(true);

// 	// 		if (nv_encoders_[i]) {
// 	// 			nvenc_info.set_bitrate(nv_encoders_[i], configurations_[i].target_bps);
// 	// 			nvenc_info.set_framerate(nv_encoders_[i], (uint32_t)configurations_[i].max_frame_rate);
// 	// 		}
// 	// 		else {
// 	// 			configurations_[i].SetStreamState(false);
// 	// 		}
// 	// 	} 
// 	// }
// }

// int32_t cvaapi_encode::SetRateAllocation(const VideoBitrateAllocation & bitrate, uint32_t framerate)
// {
// 	// if (nv_encoders_.empty())
// 	// 	return WEBRTC_VIDEO_CODEC_UNINITIALIZED;

	
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

// 	// size_t stream_idx = nv_encoders_.size() - 1;
// 	// for (size_t i = 0; i < nv_encoders_.size(); ++i, --stream_idx) {
// 	// 	configurations_[i].target_bps = bitrate.GetSpatialLayerSum(stream_idx);
// 	// 	configurations_[i].max_frame_rate = static_cast<float>(framerate);

// 	// 	if (configurations_[i].target_bps) {
// 	// 		configurations_[i].SetStreamState(true);

// 	// 		// if (nv_encoders_[i]) {
// 	// 		// 	nvenc_info.set_bitrate(nv_encoders_[i], configurations_[i].target_bps);
// 	// 		// 	nvenc_info.set_framerate(nv_encoders_[i], (uint32_t)configurations_[i].max_frame_rate);
// 	// 		// }
// 	// 		// else {
// 	// 		// 	configurations_[i].SetStreamState(false);
// 	// 		// }
// 	// 	}
// 	// }

// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// int32_t cvaapi_encode::Encode(const VideoFrame& input_frame,
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

 
	 
// 	return WEBRTC_VIDEO_CODEC_OK;
// }

// void cvaapi_encode::ReportInit() 
// {
// 	if (has_reported_init_)
// 		return;
// 	RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.NvEncoder.Event",
// 							kH264EncoderEventInit, kH264EncoderEventMax);
// 	has_reported_init_ = true;
// }

// void cvaapi_encode::ReportError() 
// {
// 	if (has_reported_error_)
// 		return;
// 	RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.NvEncoder.Event",
// 							kH264EncoderEventError, kH264EncoderEventMax);
// 	has_reported_error_ = true;
// }

// cvaapi_encode::EncoderInfo cvaapi_encode::GetEncoderInfo() const 
// {
// 	EncoderInfo info;
// 	info.supports_native_handle = false;
// 	info.implementation_name = "NvEncoder";
// 	info.scaling_settings = VideoEncoder::ScalingSettings(kLowH264QpThreshold, kHighH264QpThreshold);
// 	info.is_hardware_accelerated = true;
// 	info.has_internal_source = false;
// 	return info;
// }

// void cvaapi_encode::LayerConfig::SetStreamState(bool send_stream) 
// {
// 	if (send_stream && !sending) {
// 		// Need a key frame if we have not sent this stream before.
// 		key_frame_request = true;
// 	}
// 	sending = send_stream;
// }
// //static FILE *out_file_ptr = ::fopen("test_chensong2.rgb", "wb+");
// bool cvaapi_encode::EncodeFrame(int index, const VideoFrame& input_frame, cnv_frame_packet& frame_packet
// 							/*std::vector<uint8_t>& frame_packet*/) 
// {
// 	//frame_packet.clear();
// 	//NORMAL_EX_LOG("");
// 	if (nv_encoders_.empty() || !nv_encoders_[index])
// 	{
// 		ERROR_EX_LOG("nv encoder empty !!!");
// 		return false;
// 	}

 

// 	int width = input_frame.width();
// 	int height = input_frame.height();
	

// 	return true;
// }






// }

// #endif //