/***********************************************************************************************
created: 		2020-2-13

author:			chensong

purpose:		nvenc  


���ʿ���

NVENC ֧�ֶ������ʿ���ģʽ����ͨ���ṹNV_ENC_INITIALIZE_PARAMS::encodeConfig::rcParams�ṩ�������ʿ����㷨��صĸ��ֲ����Ŀ��� . ���ʿ����㷨�� NVENC �̼���ʵ�֡�
NVENC ֧���������ʿ���ģʽ��
�㶨������ (CBR)���㶨����������rateControlMode Ϊ NV_ENC_PARAMS_RC_CBR. ������ģʽ�£�ֻ��averageBitRate��Ҫ���������ʿ����㷨��Ŀ����������ʡ��ͻ��˿���ʹ��NV_ENC_RC_PARAMS::lowDelayKeyFrameScale������ I ֡�� P ֡�ı��ʣ���� I ֡�������ɴ������أ�����ڱ����ŵ�ӵ�������á�����NV_ENC_CONFIG_H264/ NV_ENC_CONFIG_HEVC::enableFillerDataInsertion = 1�Է���Ҫ�ϸ����ر����ʡ�
�ɱ������ (VBR)���ɱ����������rateControlMode Ϊ NV_ENC_PARAMS_RC_VBR�� ���������Է���ƽ��������averageBitRate�����ڱ�����κ�ʱ�䲻�ڳ���maxBitRate��������ģʽ�£�averageBitRate����ָ�������maxBitRateδָ����NVENC �Ὣ������Ϊ�ڲ�ȷ����Ĭ��ֵ��Ϊ�˸��õĿ��ƣ�����ͻ���ͬʱָ������������maxBitRate��averageBitRate��
�㶨 QP����ģʽ����rateControlModeΪ NV_ENC_PARAMS_RC_CONSTQP. ������ģʽ�£�����֡ʹ��ָ���� QP (NV_ENC_RC_PARAMS::constQP)���롣
Ŀ����������ģʽ����rateControlModeΪVBR �������Ŀ������targetQuality. ��Ŀ�������ķ�Χ�� 0 �� 51����Ƶ������� SDK 8.0 �����߰汾Ҳ֧��С��ֵ����������ģʽ�£�������ͨ��maxBitRate��ָ���ı����ʲ����ı仯������ÿ֡������. ��ˣ����յ�ƽ�������ʿ��ܻ������ڱ������Ƶ���ݶ��кܴ���졣���maxBitRateδָ������������������Ҫʹ�þ����ܶ��λ��ʵ��Ŀ��������Ȼ�������maxBitRate���ú������γ�ʵ�ʱ����ʵ����ޡ����maxBitRate�ڴ�ģʽ�����ù��ͣ������ʿ��ܻ��ܵ����ƣ����¿����޷�ʵ�������Ŀ��������

 

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
