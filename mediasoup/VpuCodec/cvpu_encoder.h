/***********************************************************************************************
				created: 		2022-04-01

				author:			chensong

				purpose:		vpu_encoder
************************************************************************************************/
#ifndef _C_VPU_ENCODER_H_
#define _C_VPU_ENCODER_H_
//#include "cnoncopyable.h"

#include "api/video/i420_buffer.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/utility/quality_scaler.h"

namespace chen
{

	class cvpu_encoder : public webrtc::VideoEncoder
	{
	public:
		struct LayerConfig
		{
			int simulcast_idx = 0;
			int width = -1;
			int height = -1;
			bool sending = true;
			bool key_frame_request = false;
			float max_frame_rate = 0;
			uint32_t target_bps = 0;
			uint32_t max_bps = 0;
			bool frame_dropping_on = false;
			int key_frame_interval = 0;

			void SetStreamState(bool send_stream);
		};


	public:
		explicit cvpu_encoder(const cricket::VideoCodec& codec);
		~cvpu_encoder() override; 

		int32_t InitEncode(const webrtc::VideoCodec* codec_settings,
			int32_t number_of_cores,
			size_t max_payload_size) override;
		int32_t Release() override;

		int32_t RegisterEncodeCompleteCallback(
			webrtc::EncodedImageCallback* callback) override;

		void SetRates(const RateControlParameters& parameters) override;

		int32_t SetRateAllocation(const webrtc::VideoBitrateAllocation& bitrate_allocation,
			uint32_t framerate) override;
		// The result of encoding - an EncodedImage and RTPFragmentationHeader - are
		// passed to the encode complete callback.
		int32_t Encode(const webrtc::VideoFrame& frame,
			const std::vector<webrtc::VideoFrameType>* frame_types) override;

		webrtc::VideoEncoder::EncoderInfo GetEncoderInfo() const override;

		// Exposed for testing.
		webrtc::H264PacketizationMode PacketizationModeForTesting() const 
		{
			return packetization_mode_;
		}
	private:
		webrtc::H264BitstreamParser h264_bitstream_parser_;
		// Reports statistics with histograms.
		void ReportInit();
		void ReportError();

		bool EncodeFrame(int index, const webrtc::VideoFrame& input_frame,
			std::vector<uint8_t>& frame_packet);

		std::vector<void*> vpu_encoders_;
		std::vector<LayerConfig> configurations_;
		std::vector<webrtc::EncodedImage> encoded_images_;

		webrtc::VideoCodec codec_;
		webrtc::H264PacketizationMode packetization_mode_;
		size_t max_payload_size_;
		int32_t number_of_cores_;
		webrtc::EncodedImageCallback* encoded_image_callback_;

		bool has_reported_init_;
		bool has_reported_error_;
		int video_format_;
		int num_temporal_layers_;
		uint8_t tl0sync_limit_;

		std::shared_ptr<uint8_t> image_buffer_;
	};

}

#endif // _C_VPU_ENCODER_H_