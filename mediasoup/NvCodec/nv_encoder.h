/***********************************************************************************************
created: 		2020-2-13

author:			chensong

purpose:		nvenc
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#ifndef MODULES_NV_VIDEO_CODING_CODECS_H264_ENCODER_IMPL_H_
#define MODULES_NV_VIDEO_CODING_CODECS_H264_ENCODER_IMPL_H_

#include <memory>
#include <vector>
#include <string.h>

#include "api/video/i420_buffer.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "modules/video_coding/utility/quality_scaler.h"
//#include "third_party/openh264/src/codec/api/svc/codec_app_def.h"

#include <dxgi.h>
#include <d3d11.h>
#include <dxgi1_2.h>

#include "nvenc.h"

namespace webrtc {

class NvEncoder : public VideoEncoder {
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
	explicit NvEncoder(const cricket::VideoCodec& codec);
	~NvEncoder() override;

	int32_t InitEncode(const VideoCodec* codec_settings,
						int32_t number_of_cores,
						size_t max_payload_size) override;
	int32_t Release() override;

	int32_t RegisterEncodeCompleteCallback(
		EncodedImageCallback* callback) override;

	void SetRates(const RateControlParameters& parameters) override;

	int32_t SetRateAllocation(const VideoBitrateAllocation& bitrate_allocation,
		uint32_t framerate) override;
	// The result of encoding - an EncodedImage and RTPFragmentationHeader - are
	// passed to the encode complete callback.
	int32_t Encode(const VideoFrame& frame,
				   const std::vector<VideoFrameType>* frame_types) override;

	EncoderInfo GetEncoderInfo() const override;

	// Exposed for testing.
	H264PacketizationMode PacketizationModeForTesting() const {
		return packetization_mode_;
	}

private:
	webrtc::H264BitstreamParser h264_bitstream_parser_;
	// Reports statistics with histograms.
	void ReportInit();
	void ReportError();

	bool EncodeFrame(int index,const VideoFrame& input_frame, cnv_frame_packet & frame_packet /*std::vector<uint8_t>& frame_packet*/);

	std::vector<void*> nv_encoders_;
	std::vector<LayerConfig> configurations_;
	std::vector<EncodedImage> encoded_images_;

	VideoCodec codec_;
	H264PacketizationMode packetization_mode_;
	size_t max_payload_size_;
	int32_t number_of_cores_;
	EncodedImageCallback* encoded_image_callback_;

	bool has_reported_init_;
	bool has_reported_error_;
	int video_format_;
	int num_temporal_layers_;
	uint8_t tl0sync_limit_;

	std::shared_ptr<uint8_t> image_buffer_;
	unsigned int m_key_frame_count;
	
};

}  // namespace webrtc

#endif  // MODULES_NV_VIDEO_CODING_CODECS_H264_ENCODER_IMPL_H_
