// /***********************************************************************************************
// 				created: 		2019-03-01
// 				author:			chensong
// 				purpose:		net_types
// ************************************************************************************************/

// #ifndef _C_VAAPI_ENCODE_H_
// #define _C_VAAPI_ENCODE_H_
// #include "cnet_type.h"
// #if defined(__linux__)
// #include <memory>
// #include <vector>
// #include <string.h>

// #include "api/video/i420_buffer.h"
// #include "common_video/h264/h264_bitstream_parser.h"
// #include "modules/video_coding/codecs/h264/include/h264.h"
// #include "modules/video_coding/utility/quality_scaler.h"
// namespace webrtc {
//     struct encoder_config
// {
// 	uint32_t width;
// 	uint32_t height;
// 	uint32_t framerate;
// 	uint32_t bitrate;
// 	uint32_t gop;
// 	std::string codec;  // "h264" 
// 	int32  format; // DXGI_FORMAT_NV12 DXGI_FORMAT_B8G8R8A8_UNORM
// };
//     class cvaapi_encode : public webrtc::VideoEncoder
//     {
//         public:
//             struct LayerConfig 
//             {
//             int simulcast_idx = 0;
//             int width = -1;
//             int height = -1;
//             bool sending = true;
//             bool key_frame_request = false;
//             float max_frame_rate = 0;
//             uint32_t target_bps = 0;
//             uint32_t max_bps = 0;
//             bool frame_dropping_on = false;
//             int key_frame_interval = 0;

//             void SetStreamState(bool send_stream);
//             };

//         public:
//         explicit cvaapi_encode(const cricket::VideoCodec& codec);
// 	~cvaapi_encode() override;

// 	int32_t InitEncode(const webrc::VideoCodec* codec_settings,
// 						int32_t number_of_cores,
// 						size_t max_payload_size) override;
// 	int32_t Release() override;

// 	int32_t RegisterEncodeCompleteCallback(
// 		webrtc::EncodedImageCallback* callback) override;

// 	void SetRates(const webrtc::RateControlParameters& parameters) override;

// 	int32_t SetRateAllocation(const VideoBitrateAllocation& bitrate_allocation,
// 		uint32_t framerate) override;
// 	// The result of encoding - an EncodedImage and RTPFragmentationHeader - are
// 	// passed to the encode complete callback.
// 	int32_t Encode(const VideoFrame& frame,
// 				   const std::vector<webrtc::VideoFrameType>* frame_types) override;

// 	EncoderInfo GetEncoderInfo() const override;

// 	// Exposed for testing.
// 	H264PacketizationMode PacketizationModeForTesting() const {
// 		return packetization_mode_;
// 	}

// private:
// 	webrtc::H264BitstreamParser h264_bitstream_parser_;
// 	// Reports statistics with histograms.
// 	void ReportInit();
// 	void ReportError();

// 	bool EncodeFrame(int index,const webrtc::VideoFrame& input_frame, cnv_frame_packet & frame_packet /*std::vector<uint8_t>& frame_packet*/);

// 	std::vector<void*> nv_encoders_;
// 	std::vector<LayerConfig> configurations_;
// 	std::vector<EncodedImage> encoded_images_;

// 	VideoCodec codec_;
// 	H264PacketizationMode packetization_mode_;
// 	size_t max_payload_size_;
// 	int32_t number_of_cores_;
// 	EncodedImageCallback* encoded_image_callback_;

// 	bool has_reported_init_;
// 	bool has_reported_error_;
// 	int video_format_;
// 	int num_temporal_layers_;
// 	uint8_t tl0sync_limit_;

// 	std::shared_ptr<uint8_t> image_buffer_;
// 	unsigned int m_key_frame_count;
//     }
// }
// #endif // #if defined(_MSC_VER)
// #endif // _C_VAAPI_ENCODE_H_