
// 2020-2-13

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
