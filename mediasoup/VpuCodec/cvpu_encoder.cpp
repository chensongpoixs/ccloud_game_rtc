#include "cvpu_encoder.h"
#include "modules/video_coding/codecs/h264/h264_encoder_impl.h"

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
#include "cvpu_encoder.h"
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
#include "cbm_vpu.h"
#include "../clog.h"
namespace chen {
	enum BmVideoEncoderEvent
	{
		kH264EncoderEventInit = 0,
		kH264EncoderEventError = 1,
		kH264EncoderEventMax = 16,
	};



	//std::vector<webrtc::H264::NaluIndex> webrtc_FindNaluIndices(const uint8_t* buffer,
	//	size_t buffer_size) {
	//	// This is sorta like Boyer-Moore, but with only the first optimization step:
	//	// given a 3-byte sequence we're looking at, if the 3rd byte isn't 1 or 0,
	//	// skip ahead to the next 3-byte sequence. 0s and 1s are relatively rare, so
	//	// this will skip the majority of reads/checks.
	//	std::vector<webrtc::H264::NaluIndex> sequences;
	//	if (buffer_size < 4)
	//	{
	//		return sequences;
	//	}

	//	const size_t end = buffer_size - 4;
	//	for (size_t i = 0; i < end;) 
	//	{
	//		if (buffer[i + 3] > 1) 
	//		{
	//			i += 4;
	//		}
	//		else if (buffer[i + 3] == 1 && buffer[i + 2] == 0 && buffer[i + 1] == 0 && buffer[i] == 0) {
	//			// We found a start sequence, now check if it was a 3 of 4 byte one.
	//			webrtc::H264::NaluIndex index = { i, i + 4, 0 };
	//			if (index.start_offset > 0 && buffer[index.start_offset - 1] == 0)
	//			{
	//				--index.start_offset;
	//			}

	//			// Update length of previous entry.
	//			auto it = sequences.rbegin();
	//			if (it != sequences.rend())
	//			{
	//				it->payload_size = index.start_offset - it->payload_start_offset;
	//			}

	//			sequences.push_back(index);

	//			i += 4;
	//		}
	//		else 
	//		{
	//			++i;
	//		}
	//	}

	//	// Update length of last entry, if any.
	//	auto it = sequences.rbegin();
	//	if (it != sequences.rend())
	//	{
	//		it->payload_size = buffer_size - it->payload_start_offset;
	//	}

	//	return sequences;
	//}




	//static int frame_count = 0;
	static void webrtc_RtpFragmentize(webrtc::EncodedImage* encoded_image,
		const webrtc::VideoFrameBuffer& frame_buffer,
		std::vector<uint8_t>& frame_packet,
		webrtc::RTPFragmentationHeader* frag_header)
	{
		
		//static std::string out_file_name_bm_h264 = "./bm_h264/bm_h264.h264";
		//static FILE* out_file_bm_h264_ptr = NULL; 
		//if (!out_file_bm_h264_ptr)
		//{
		//	out_file_bm_h264_ptr = ::fopen(out_file_name_bm_h264.c_str(), "wb+");
		//}
		//if (out_file_bm_h264_ptr)
		//{
		//	::fwrite(&frame_packet[0], 1, frame_packet.size(), out_file_bm_h264_ptr);
		//	::fflush(out_file_bm_h264_ptr);
		//	/*::fclose(out_file_bm_h264_ptr);
		//	out_file_bm_h264_ptr = NULL;*/
		//}
		size_t required_capacity = 0;
		encoded_image->set_size(0);

		required_capacity = frame_packet.size();
		//encoded_image->Allocate(required_capacity);

		// TODO(nisse): Use a cache or buffer pool to avoid allocation?
		encoded_image->Allocate(required_capacity);


		///////////////////////////////////////H264 NAL///////////////////////////////////////////////////////

		memcpy(encoded_image->data() + encoded_image->size(), &frame_packet[0], frame_packet.size());
		encoded_image->set_size(encoded_image->size() + frame_packet.size());

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<webrtc::H264::NaluIndex> nalus = webrtc::H264:: FindNaluIndices(
			encoded_image->data(), encoded_image->size());
		size_t fragments_count = nalus.size();
		// const uint8_t start_code[4] = {0, 0, 0, 1};
		frag_header->VerifyAndAllocateFragmentationHeader(fragments_count);
		for (size_t i = 0; i < nalus.size(); ++i)
		{
			frag_header->fragmentationOffset[i] = nalus[i].payload_start_offset;
			frag_header->fragmentationLength[i] = nalus[i].payload_size;
		}
	}

	void cvpu_encoder::LayerConfig::SetStreamState(bool send_stream)
	{
		if (send_stream && !sending) 
		{
			// Need a key frame if we have not sent this stream before.
			key_frame_request = true;
		}
		sending = send_stream;
	}
	cvpu_encoder::cvpu_encoder(const cricket::VideoCodec& codec)
		: packetization_mode_(webrtc::H264PacketizationMode::SingleNalUnit)
		, max_payload_size_(0)
		, number_of_cores_(0)
		, encoded_image_callback_(nullptr)
		, has_reported_init_(false)
		, has_reported_error_(false)
		, num_temporal_layers_(1)
		, tl0sync_limit_(0)
	{
		DEBUG_EX_LOG("bm_encoder ....");
		bmvpu_setup_logging();
		std::string packetization_mode_string;
		if (codec.GetParam(cricket::kH264FmtpPacketizationMode, &packetization_mode_string)
			&& packetization_mode_string == "1") {
			packetization_mode_ = webrtc::H264PacketizationMode::NonInterleaved;
		}

		encoded_images_.reserve(webrtc::kMaxSimulcastStreams);
		vpu_encoders_.reserve(webrtc::kMaxSimulcastStreams);
		configurations_.reserve(webrtc::kMaxSimulcastStreams);
		image_buffer_ = nullptr;
		
	}
	cvpu_encoder::~cvpu_encoder()
	{
		Release();
	}
	int32_t cvpu_encoder::InitEncode(const webrtc::VideoCodec* inst, int32_t number_of_cores, size_t max_payload_size)
	{
		DEBUG_EX_LOG("bm_encoder init  ....");
		ReportInit();
		if (!inst || inst->codecType != webrtc::kVideoCodecH264) {
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

		int number_of_streams = webrtc::SimulcastUtility::NumberOfSimulcastStreams(*inst);
		bool doing_simulcast = (number_of_streams > 1);

		if (doing_simulcast && !webrtc::SimulcastUtility::ValidSimulcastParameters(*inst, number_of_streams)) {
			return WEBRTC_VIDEO_CODEC_ERR_SIMULCAST_PARAMETERS_NOT_SUPPORTED;
		}

		assert(number_of_streams == 1);

		encoded_images_.resize(number_of_streams);
		vpu_encoders_.resize(number_of_streams);
		configurations_.resize(number_of_streams);

		number_of_cores_ = number_of_cores;
		max_payload_size_ = max_payload_size;
		codec_ = *inst;


		// Code expects simulcastStream resolutions to be correct, make sure they are
	// filled even when there are no simulcast layers.
		if (codec_.numberOfSimulcastStreams == 0)
		{
			codec_.simulcastStream[0].width = codec_.width;
			codec_.simulcastStream[0].height = codec_.height;
		}

		num_temporal_layers_ = codec_.H264()->numberOfTemporalLayers;

		/// <summary>
		/// //////////
		/// </summary>
		/// <param name="inst"></param>
		/// <param name="number_of_cores"></param>
		/// <param name="max_payload_size"></param>
		/// <returns></returns>
		/// 
		/// 
		for (int i = 0, idx = number_of_streams - 1; i < number_of_streams; ++i, --idx) 
		{
			cbm_encoder* bm_encoder_ptr = new cbm_encoder();
			// Store nvidia encoder.
			//nv_encoders_[i] = nvenc_info.create();
			vpu_encoders_[i] = bm_encoder_ptr;
			// Set internal settings from codec_settings
			configurations_[i].simulcast_idx = idx;
			configurations_[i].sending = true;
			configurations_[i].width = codec_.simulcastStream[idx].width;
			configurations_[i].height = codec_.simulcastStream[idx].height;
			configurations_[i].max_frame_rate = static_cast<float>(codec_.maxFramerate);
			configurations_[i].frame_dropping_on = codec_.H264()->frameDroppingOn;
			configurations_[i].key_frame_interval = codec_.H264()->keyFrameInterval;

			// Codec_settings uses kbits/second; encoder uses bits/second.
			configurations_[i].max_bps = codec_.maxBitrate * 1000;
			configurations_[i].target_bps = codec_.startBitrate * 1000;

			//encoder_config nvenc_config;
			//nvenc_config.codec = "h264";
			//nvenc_config.format = DXGI_FORMAT_B8G8R8A8_UNORM;
			//nvenc_config.width = configurations_[i].width;
			//nvenc_config.height = configurations_[i].height;
			//nvenc_config.framerate = (uint32_t)configurations_[i].max_frame_rate;
			////TODO@chensong 2022-03-18 设置GOP 的大小 哈 ^_^ 不知道为什么 webrtc中设置gop key_frame_interval 字段是有效的、
			////根据程序
			//nvenc_config.gop = 45;// configurations_[i].key_frame_interval;
			//nvenc_config.bitrate = configurations_[i].target_bps;



			//int soc_idx, int width, int height
			if (!bm_encoder_ptr->init(0, configurations_[i].width, configurations_[i].height)) {
				Release();
				ReportError();
				return WEBRTC_VIDEO_CODEC_ERROR;
			}

			image_buffer_.reset(new uint8_t[configurations_[i].width * configurations_[i].height * 10]);

			// TODO(pbos): Base init params on these values before submitting.
			video_format_ = EVideoFormatType::videoFormatI420;

			// Initialize encoded image. Default buffer size: size of unencoded data.
			const size_t new_capacity = webrtc::CalcBufferSize(webrtc::VideoType::kI420,
				codec_.simulcastStream[idx].width, codec_.simulcastStream[idx].height);
			encoded_images_[i].Allocate((new_capacity));
			encoded_images_[i]._completeFrame = true;
			encoded_images_[i]._encodedWidth = codec_.simulcastStream[idx].width;
			encoded_images_[i]._encodedHeight = codec_.simulcastStream[idx].height;
			encoded_images_[i].set_size(0);
		}

		webrtc::SimulcastRateAllocator init_allocator(codec_);
		/*VideoBitrateAllocation allocation = init_allocator.GetAllocation(
			codec_.startBitrate * 1000, codec_.maxFramerate);
		SetRates(RateControlParameters(allocation, codec_.maxFramerate));
		return WEBRTC_VIDEO_CODEC_OK;*/
		webrtc::VideoBitrateAllocation allocation = init_allocator.GetAllocation(
			codec_.startBitrate * 1000, codec_.maxFramerate);
		return SetRateAllocation(allocation, codec_.maxFramerate);

		return int32_t();
	}
	int32_t cvpu_encoder::Release()
	{
		while (!vpu_encoders_.empty())
		{
			cbm_encoder* nv_encoder = static_cast<cbm_encoder*> (vpu_encoders_.back());
			if (nv_encoder) {
				nv_encoder->destroy();
				delete nv_encoder;
				//nvenc_info.destroy(&nv_encoder);
				nv_encoder = nullptr;
			}
			vpu_encoders_.pop_back();
		}

		configurations_.clear();
		encoded_images_.clear();

		return WEBRTC_VIDEO_CODEC_OK;
		 
	}
	int32_t cvpu_encoder::RegisterEncodeCompleteCallback(webrtc::EncodedImageCallback* callback)
	{
		encoded_image_callback_ = callback;
		return WEBRTC_VIDEO_CODEC_OK;
	}
	void cvpu_encoder::SetRates(const RateControlParameters& parameters)
	{
		if (parameters.bitrate.get_sum_bps() == 0) 
		{
			// Encoder paused, turn off all encoding.
			for (size_t i = 0; i < configurations_.size(); ++i)
				configurations_[i].SetStreamState(false);
			return;
		}

		// At this point, bitrate allocation should already match codec settings.
		if (codec_.maxBitrate > 0)
		{
			RTC_DCHECK_LE(parameters.bitrate.get_sum_kbps(), codec_.maxBitrate);
		}
		RTC_DCHECK_GE(parameters.bitrate.get_sum_kbps(), codec_.minBitrate);
		if (codec_.numberOfSimulcastStreams > 0)
		{
			RTC_DCHECK_GE(parameters.bitrate.get_sum_kbps(), codec_.simulcastStream[0].minBitrate);

		}
		codec_.maxFramerate = parameters.framerate_fps;

		size_t stream_idx = vpu_encoders_.size() - 1;
		for (size_t i = 0; i < vpu_encoders_.size(); ++i, --stream_idx) 
		{
			configurations_[i].target_bps = parameters.bitrate.GetSpatialLayerSum(stream_idx);
			configurations_[i].max_frame_rate = static_cast<float>(parameters.framerate_fps);

			if (configurations_[i].target_bps) 
			{
				configurations_[i].SetStreamState(true);
				// TODO@chensong 2022-04-02  
				/*if (vpu_encoders_[i]) {
					nvenc_info.set_bitrate(nv_encoders_[i], configurations_[i].target_bps);
					nvenc_info.set_framerate(nv_encoders_[i], (uint32_t)configurations_[i].max_frame_rate);
				}
				else*/ 
				{
					configurations_[i].SetStreamState(false);
				}
			}
		}
	}
	int32_t cvpu_encoder::SetRateAllocation(const webrtc::VideoBitrateAllocation& bitrate, uint32_t framerate)
	{
		if (vpu_encoders_.empty())
			return WEBRTC_VIDEO_CODEC_UNINITIALIZED;


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

		codec_.maxFramerate = framerate;

		size_t stream_idx = vpu_encoders_.size() - 1;
		for (size_t i = 0; i < vpu_encoders_.size(); ++i, --stream_idx) {
			configurations_[i].target_bps = bitrate.GetSpatialLayerSum(stream_idx);
			configurations_[i].max_frame_rate = static_cast<float>(framerate);

			if (configurations_[i].target_bps) {
				configurations_[i].SetStreamState(true);
				// TODO@chensong 2022-04-02
				/*if (vpu_encoders_[i]) {
					nvenc_info.set_bitrate(nv_encoders_[i], configurations_[i].target_bps);
					nvenc_info.set_framerate(nv_encoders_[i], (uint32_t)configurations_[i].max_frame_rate);
				}
				else */ {
					//configurations_[i].SetStreamState(false);
				}
			}
		}
		DEBUG_EX_LOG("bm_encoder init ok ....");
		return WEBRTC_VIDEO_CODEC_OK;
		 
	}


	//FILE* out_file_yuv_ptr = ::fopen("bm_yuv.yuv", "wb+");


	int32_t cvpu_encoder::Encode(const webrtc::VideoFrame& input_frame, const std::vector<webrtc::VideoFrameType>* frame_types)
	{
		//DEBUG_EX_LOG("bm_encoder  encode fame ....");
		if (vpu_encoders_.empty()) {
			ReportError();
			return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
		}
		if (!encoded_image_callback_) {
			RTC_LOG(LS_WARNING)
				<< "InitEncode() has been called, but a callback function "
				<< "has not been set with RegisterEncodeCompleteCallback()";
			ERROR_EX_LOG(" InitEncode() has been called, but a callback function ");
			ReportError();
			return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
		}

		rtc::scoped_refptr<const webrtc::I420BufferInterface> frame_buffer = input_frame.video_frame_buffer()->ToI420();


		//if (out_file_yuv_ptr)
		//{
		//	::fwrite(frame_buffer->DataY(), 1, frame_buffer->width() * frame_buffer->height(),  out_file_yuv_ptr);
		//	::fflush(out_file_yuv_ptr);
		//	::fwrite(frame_buffer->DataU(), 1, ((frame_buffer->width() + 1)/2 ) * frame_buffer->height() /2, out_file_yuv_ptr);
		//	::fflush(out_file_yuv_ptr);
		//	::fwrite(frame_buffer->DataV(), 1, ((frame_buffer->width() + 1) / 2) * frame_buffer->height() / 2, out_file_yuv_ptr);
		//	::fflush(out_file_yuv_ptr);
		//	//::fclose(out_file_yuv_ptr);
		//	DEBUG_EX_LOG("width = %u, height = %u", frame_buffer->width(), frame_buffer->height());
		//}



		bool send_key_frame = false;
		/*for (size_t i = 0; i < configurations_.size(); ++i) {
			if (configurations_[i].key_frame_request && configurations_[i].sending) {
				send_key_frame = true;
				break;
			}
		}
		if (!send_key_frame && frame_types) {
			for (size_t i = 0; i < frame_types->size() && i < configurations_.size(); ++i) {
				if ((*frame_types)[i] == webrtc::VideoFrameType::kVideoFrameKey && configurations_[i].sending) {
					send_key_frame = true;
					break;
				}
			}
		}*/

		RTC_DCHECK_EQ(configurations_[0].width, frame_buffer->width());
		RTC_DCHECK_EQ(configurations_[0].height, frame_buffer->height());

		// Encode image for each layer.
		for (size_t i = 0; i < vpu_encoders_.size(); ++i) 
		{
			//if (!configurations_[i].sending) 
			//{
			//	continue;
			//}

			//if (frame_types != nullptr) 
			//{
			//	// Skip frame?
			//	if ((*frame_types)[i] == webrtc::VideoFrameType::kEmptyFrame) 
			//	{
			//		continue;
			//	}
			//}

			if (send_key_frame) 
			{
				if (!vpu_encoders_.empty() && vpu_encoders_[i]) 
				{
					// TODO@chensong 2022-04-02 
					//nvenc_info.request_idr(nv_encoders_[i]);
				}

				configurations_[i].key_frame_request = false;
			}

			// EncodeFrame output.
			SFrameBSInfo info;
			memset(&info, 0, sizeof(SFrameBSInfo));
			std::vector<uint8_t> frame_packet;

			//EncodeFrame((int)i, input_frame, frame_packet);

			cbm_encoder* bm_encoder_ptr = static_cast<cbm_encoder*> (vpu_encoders_[i]);

			if (!bm_encoder_ptr)
			{
				ERROR_EX_LOG("vpu_encoder not find (i = %u) !!!", i);
				return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
			}
			BmVpuFrameType encoder_frame_type;
			
			std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
			std::chrono::steady_clock::duration dur;
			std::chrono::milliseconds ms;
			int ret = bm_encoder_ptr->encoder_image( (uint8_t*)frame_buffer->DataY(), (uint8_t*)frame_buffer->DataU(), (uint8_t*)frame_buffer->DataV(), frame_buffer->width(), frame_buffer->height(), frame_packet, encoder_frame_type);
			std::chrono::steady_clock::time_point cur_time_ms = std::chrono::steady_clock::now();
			
			dur = cur_time_ms - pre_time;
			ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
			uint32_t elapse = static_cast<uint32_t>(ms.count());
			DEBUG_EX_LOG("encoder frame ms = %u", elapse);
			if (ret != 0 || frame_packet.size() == 0) 
			{
				ERROR_EX_LOG("encoder frame failed !!!");
				return WEBRTC_VIDEO_CODEC_OK;
			}
			else 
			{ 
				switch (encoder_frame_type)
				{
				case BM_VPU_FRAME_TYPE_IDR:
				case BM_VPU_FRAME_TYPE_I:
				{
					encoded_images_[i]._frameType = webrtc::VideoFrameType::kVideoFrameKey;
					break;
				}
				
				case BM_VPU_FRAME_TYPE_P:
				{
					// return WEBRTC_VIDEO_CODEC_OK;
					//encoded_images_[i]._frameType = webrtc::VideoFrameType::kVideoFrameKey;
					encoded_images_[i]._frameType = webrtc::VideoFrameType::kVideoFrameDelta;
					break;
				}
				case videoFrameTypeInvalid:
				default:
					ERROR_EX_LOG("encoder frame  [type =  %d] error failed !!!", encoder_frame_type);
					return WEBRTC_VIDEO_CODEC_OK;
					break;
				}
				/*if (frame_packet[4] == 0x67) {
					info.eFrameType = videoFrameTypeIDR;
				}
				else if (frame_packet[4] == 0x61) {
					info.eFrameType = videoFrameTypeP;
				}
				else {
					return WEBRTC_VIDEO_CODEC_OK;
				}*/
			}

			encoded_images_[i]._encodedWidth = configurations_[i].width;
			encoded_images_[i]._encodedHeight = configurations_[i].height;
			encoded_images_[i].SetTimestamp(input_frame.timestamp());
			encoded_images_[i].ntp_time_ms_ = input_frame.ntp_time_ms();
			encoded_images_[i].capture_time_ms_ = input_frame.render_time_ms();
			encoded_images_[i].rotation_ = input_frame.rotation();
			encoded_images_[i].SetColorSpace(input_frame.color_space());
			encoded_images_[i].content_type_ =
				(codec_.mode == webrtc::VideoCodecMode::kScreensharing)
				? webrtc::VideoContentType::SCREENSHARE
				: webrtc::VideoContentType::UNSPECIFIED;
			encoded_images_[i].timing_.flags = webrtc::VideoSendTiming::kInvalid;
			//encoded_images_[i]._frameType = webrtc::ConvertToVideoFrameType(info.eFrameType);
			encoded_images_[i].SetSpatialIndex(configurations_[i].simulcast_idx);

			// Split encoded image up into fragments. This also updates
			// |encoded_image_|.
			webrtc::RTPFragmentationHeader frag_header;
			webrtc_RtpFragmentize(&encoded_images_[i], *frame_buffer, frame_packet, &frag_header);
			//RtpFragmentize_webrtc(&encoded_images_[i], *frame_buffer, &info, &frag_header);

			// Encoder can skip frames to save bandwidth in which case
			// |encoded_images_[i]._length| == 0.
			if (encoded_images_[i].size() > 0)
			{
				// Parse QP.
				/*h264_bitstream_parser_.ParseBitstream(encoded_images_[i].data(),
				                                      encoded_images_[i].size());*/
				h264_bitstream_parser_.GetLastSliceQp(&encoded_images_[i].qp_);
				//encoded_images_[i].qp_ = 10;

				// Deliver encoded image.
				webrtc::CodecSpecificInfo codec_specific;
				codec_specific.codecType = webrtc::kVideoCodecH264;
				codec_specific.codecSpecific.H264.packetization_mode = packetization_mode_;
				codec_specific.codecSpecific.H264.temporal_idx = webrtc::kNoTemporalIdx;
				codec_specific.codecSpecific.H264.idr_frame = encoder_frame_type == BM_VPU_FRAME_TYPE_IDR || BM_VPU_FRAME_TYPE_I == encoder_frame_type;
				codec_specific.codecSpecific.H264.base_layer_sync = false;

				// if (info.eFrameType == videoFrameTypeIDR &&
				//    encoded_images_[i]._frameType == kVideoFrameKey) {
				//  RTC_LOG(LS_ERROR) << "send idr frame - " << encoded_images_[i].size();
				//}
				//DEBUG_EX_LOG("bm_encoder callback image ok ....");
				encoded_image_callback_->OnEncodedImage(encoded_images_[i], &codec_specific, &frag_header);
			}
			else
			{
				ERROR_EX_LOG("--- encoder_frame_type = %u", encoder_frame_type);
			}
		}

		return WEBRTC_VIDEO_CODEC_OK;
		
	}
	webrtc::VideoEncoder::EncoderInfo cvpu_encoder::GetEncoderInfo() const
	{
		EncoderInfo info;
		info.supports_native_handle = false;
		info.implementation_name = "bmEncoder";
		info.scaling_settings = VideoEncoder::ScalingSettings(24 /*webrtc::kLowH264QpThreshold*/, 37 /*kHighH264QpThreshold*/);
		info.is_hardware_accelerated = true;
		info.has_internal_source = false;
		return info;
	}
	void cvpu_encoder::ReportInit()
	{
		if (has_reported_init_)
		{
			return;
		}
		using namespace webrtc;
		using namespace rtc;
		 RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.bmEncoder.Event",
			kH264EncoderEventInit, kH264EncoderEventMax);
		has_reported_init_ = true;
	}
	void cvpu_encoder::ReportError()
	{
		if (has_reported_error_)
		{
			return;
		}
		RTC_HISTOGRAM_ENUMERATION("WebRTC.Video.bmEncoder.Event",
			kH264EncoderEventError, kH264EncoderEventMax);
		has_reported_error_ = true;
	}
	bool cvpu_encoder::EncodeFrame(int index, const webrtc::VideoFrame& input_frame, std::vector<uint8_t>& frame_packet)
	{
		return false;
	}
}