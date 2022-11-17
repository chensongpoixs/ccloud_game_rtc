/***********************************************************************************************
created: 		2020-2-13

author:			chensong

purpose:		nvenc  


速率控制

NVENC 支持多种速率控制模式，并通过结构NV_ENC_INITIALIZE_PARAMS::encodeConfig::rcParams提供对与速率控制算法相关的各种参数的控制 . 速率控制算法在 NVENC 固件中实现。
NVENC 支持以下速率控制模式：
恒定比特率 (CBR)：恒定比特率设置rateControlMode 为 NV_ENC_PARAMS_RC_CBR. 在这种模式下，只有averageBitRate需要并用作速率控制算法的目标输出比特率。客户端可以使用NV_ENC_RC_PARAMS::lowDelayKeyFrameScale来控制 I 帧与 P 帧的比率，如果 I 帧最终生成大量比特，这对于避免信道拥塞很有用。设置NV_ENC_CONFIG_H264/ NV_ENC_CONFIG_HEVC::enableFillerDataInsertion = 1以防需要严格遵守比特率。
可变比特率 (VBR)：可变比特率设置rateControlMode 为 NV_ENC_PARAMS_RC_VBR。 编码器尝试符合平均比特率averageBitRate，并在编码的任何时间不内超过maxBitRate。在这种模式下，averageBitRate必须指定。如果maxBitRate未指定，NVENC 会将其设置为内部确定的默认值。为了更好的控制，建议客户端同时指定这两个参数maxBitRate和averageBitRate。
恒定 QP：此模式设置rateControlMode为 NV_ENC_PARAMS_RC_CONSTQP. 在这种模式下，整个帧使用指定的 QP (NV_ENC_RC_PARAMS::constQP)编码。
目标质量：此模式设置rateControlMode为VBR 和所需的目标质量targetQuality. 此目标质量的范围是 0 到 51（视频编解码器 SDK 8.0 及更高版本也支持小数值）。在这种模式下，编码器通过maxBitRate中指定的比特率参数的变化来保持每帧的质量. 因此，最终的平均比特率可能会因正在编码的视频内容而有很大差异。如果maxBitRate未指定，编码器将根据需要使用尽可能多的位来实现目标质量。然而，如果maxBitRate设置后，它将形成实际比特率的上限。如果maxBitRate在此模式下设置过低，比特率可能会受到限制，导致可能无法实现所需的目标质量。

 

$(CUDA_PATH)\lib\x64\cuda.lib
$(CUDA_PATH)\lib\x64\cudadevrt.lib
$(CUDA_PATH)\lib\x64\cudart.lib
$(CUDA_PATH)\lib\x64\cudart_static.lib
$(CUDA_PATH)\lib\x64\OpenCL.lib


************************************************************************************************/
 
#ifndef _NVIDIA_ENCODER_H
#define _NVIDIA_ENCODER_H

#include "encoder_info.h"

// Video-Codec-SDK Version: 8.2
#include "NvEncoder/NvEncoderD3D11.h" 
namespace chen {
extern struct encoder_info nvenc_info;
extern uint32 g_gpu_index;;
extern DXGI_FORMAT g_dxgi_format;
}
#endif
