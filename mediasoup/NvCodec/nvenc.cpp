#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "nvenc.h"
#include <cstdint>
#include <string>
#include <dxgi.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include "../clog.h"
#include "../ccfg.h"

namespace chen {

struct nvenc_data
{
	ID3D11Device*        d3d11_device  = nullptr;
	ID3D11DeviceContext* d3d11_context = nullptr;
	ID3D11Texture2D*     copy_texture  = nullptr;
	IDXGIAdapter*        adapter       = nullptr;
	IDXGIFactory1*       factory       = nullptr;

	HANDLE           input_handle  = nullptr;
	ID3D11Texture2D* input_texture = nullptr;;
	IDXGIKeyedMutex* keyed_mutex   = nullptr;

	std::mutex mutex;
	uint32_t width     = 0;
	uint32_t height    = 0;
	uint32_t framerate = 0;
	uint32_t bitrate   = 0;
	uint32_t gop       = 0;
	std::string codec;
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	NvEncoderD3D11 *nvenc = nullptr;
};

static bool is_supported(void)
{
	static HMODULE hModule = NULL;
	using namespace chen;
	if (hModule == NULL) {
#if defined(_WIN64)
		hModule = LoadLibrary(TEXT("nvEncodeAPI64.dll"));
#elif defined(_WIN32)
		hModule = LoadLibrary(TEXT("nvEncodeAPI.dll"));
#endif
	}

	if (hModule == NULL) 
	{
		//printf("[nvenc] Error: NVENC library file is not found. Please ensure NV driver is installed. \n");
		ERROR_EX_LOG("[nvenc] Error: NVENC library file is not found. Please ensure NV driver is installed. ");
		return false;
	}

	typedef NVENCSTATUS(NVENCAPI *NvEncodeAPIGetMaxSupportedVersion_Type)(uint32_t*);
#if defined(_WIN32)
	NvEncodeAPIGetMaxSupportedVersion_Type NvEncodeAPIGetMaxSupportedVersion = (NvEncodeAPIGetMaxSupportedVersion_Type)GetProcAddress(hModule, "NvEncodeAPIGetMaxSupportedVersion");
#else
	NvEncodeAPIGetMaxSupportedVersion_Type NvEncodeAPIGetMaxSupportedVersion = (NvEncodeAPIGetMaxSupportedVersion_Type)dlsym(hModule, "NvEncodeAPIGetMaxSupportedVersion");
#endif

	uint32_t version = 0;
	uint32_t currentVersion = (NVENCAPI_MAJOR_VERSION << 4) | NVENCAPI_MINOR_VERSION;
	NVENC_API_CALL(NvEncodeAPIGetMaxSupportedVersion(&version));
	if (currentVersion > version) 
	{
		//printf("[nvenc] Error: Current Driver Version does not support this NvEncodeAPI version, please upgrade driver");
		ERROR_EX_LOG("[nvenc] Error: Current Driver Version does not support this NvEncodeAPI version, please upgrade driver");
		return false;
	}

	return true;
}

static void* nvenc_create()
{
	using namespace chen;
	//ERROR_EX_LOG("");
	if (!is_supported()) {
		return nullptr;
	}

	struct nvenc_data* enc = new nvenc_data;
	HRESULT hr = S_OK;

	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&enc->factory);
	if (FAILED(hr)) {
		return nullptr;
	}

	for (int gpuIndex = 0; gpuIndex <= 1; gpuIndex++) {
		hr = enc->factory->EnumAdapters(gpuIndex, &enc->adapter);
		if (FAILED(hr)) {
			goto failed;
		}
		else {
			char desc[128] = { 0 };
			DXGI_ADAPTER_DESC adapterDesc;
			enc->adapter->GetDesc(&adapterDesc);
			wcstombs(desc, adapterDesc.Description, sizeof(desc));
			if (strstr(desc, "NVIDIA") == NULL) {
				continue;
			}
		}

		hr = D3D11CreateDevice(enc->adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
							   &enc->d3d11_device, nullptr, &enc->d3d11_context);
		enc->adapter->Release();
		enc->adapter = nullptr;
		if (SUCCEEDED(hr)) {
			break;
		}
	}

	if (enc->factory) {
		enc->factory->Release();
		enc->factory = nullptr;
	}

	if (enc->d3d11_device == nullptr) {
		printf("[nvenc] Error: Failed to create d3d11 device. \n");
		ERROR_EX_LOG("[nvenc] Error: Failed to create d3d11 device.");
		goto failed;
	}

	return enc;

failed:
	if (enc->factory) {
		enc->factory->Release();
		enc->factory = nullptr;
	}

	if (enc != nullptr)  {
		delete enc;
	}
	return nullptr;
}

static void nvenc_destroy(void **nvenc_data)
{
	using namespace chen;
	//ERROR_EX_LOG("");
	struct nvenc_data *enc = (struct nvenc_data *)(*nvenc_data);

	enc->mutex.lock();

	if (enc->nvenc != nullptr) {
		enc->nvenc->DestroyEncoder();
		delete enc->nvenc;
		enc->nvenc = nullptr;
	}

	if (enc->d3d11_device) {
		enc->d3d11_device->Release();
		enc->d3d11_device = nullptr;
	}

	if (enc->d3d11_context) {
		enc->d3d11_context->Release();
		enc->d3d11_context = nullptr;
	}

	if (enc->adapter) {
		enc->adapter->Release();
		enc->adapter = nullptr;
	}

	if (enc->factory) {
		enc->factory->Release();
		enc->factory = nullptr;
	}

	if (enc->copy_texture) {
		enc->copy_texture->Release();
		enc->copy_texture = nullptr;
	}

	if (enc->input_texture) {
		enc->input_texture->Release();
		enc->input_texture = nullptr;
	}

	if (enc->keyed_mutex) {
		enc->keyed_mutex->Release();
		enc->keyed_mutex = nullptr;
	}

	enc->mutex.unlock();

	delete *nvenc_data;
	*nvenc_data = nullptr;
}

static bool nvenc_init(void *nvenc_data, void *encoder_config)
{
	using namespace chen;
	//ERROR_EX_LOG("");
	if (nvenc_data == nullptr) {
		return false;
	}

	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;
	struct encoder_config* config = (struct encoder_config*)encoder_config;

	std::lock_guard<std::mutex> locker(enc->mutex);
	if (enc->nvenc != nullptr) {
		return false;
	}

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = config->width;
	desc.Height = config->height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = config->format;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = enc->d3d11_device->CreateTexture2D(&desc, nullptr, &enc->copy_texture);
	if (FAILED(hr)) {
		printf("[nvenc] Error: Failed to create texture. \n");
		ERROR_EX_LOG("[nvenc] Error: Failed to create texture.");
		return false;
	}

	enc->width = config->width;
	enc->height = config->height;
	enc->framerate = config->framerate;
	enc->format = config->format;
	enc->codec = config->codec;
	enc->gop = config->gop;
	enc->bitrate = config->bitrate;

	NV_ENC_BUFFER_FORMAT eBufferFormat = NV_ENC_BUFFER_FORMAT_NV12;
	if (enc->format == DXGI_FORMAT_NV12) {
		eBufferFormat = NV_ENC_BUFFER_FORMAT_NV12;
	}
	else if (enc->format == DXGI_FORMAT_B8G8R8A8_UNORM) {
		eBufferFormat = NV_ENC_BUFFER_FORMAT_ARGB;
	}
	else if (enc->format == DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		eBufferFormat = NV_ENC_BUFFER_FORMAT_ARGB; // error 
	}
	else {
		printf("[nvenc] Error: Unsupported dxgi format. \n");
		ERROR_EX_LOG("[nvenc] Error: Unsupported dxgi format. ");
		return false;
	}

	GUID codecId = NV_ENC_CODEC_H264_GUID;
	if (enc->codec == "h264") {
		codecId = NV_ENC_CODEC_H264_GUID;
	}
	else if (enc->codec == "hevc") {
		codecId = NV_ENC_CODEC_HEVC_GUID;
	}
	else {
		printf("[nvenc] Error: Unsupported codec. \n");
		ERROR_EX_LOG("[nvenc] Error: Unsupported codec.");
		return false;
	}

	enc->nvenc = new NvEncoderD3D11(enc->d3d11_device, enc->width, enc->height, eBufferFormat);

	NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
	NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
	initializeParams.encodeConfig = &encodeConfig;	
	enc->nvenc->CreateDefaultEncoderParams(&initializeParams, codecId, NV_ENC_PRESET_LOW_LATENCY_DEFAULT_GUID);

	initializeParams.maxEncodeWidth = enc->width;
	initializeParams.maxEncodeHeight = enc->height;
	initializeParams.frameRateNum = 60;
	initializeParams.encodeConfig->gopLength = g_cfg.get_uint32(ECI_RtcVideoGop);
	initializeParams.encodeConfig->rcParams.averageBitRate = g_cfg.get_uint32(ECI_RtcAvgRate) * 1000 ;
	initializeParams.encodeConfig->rcParams.maxBitRate = g_cfg.get_uint32(ECI_RtcMaxRate) * 1000;
	//initializeParams.encodeConfig->rcParams.vbvBufferSize = enc->bitrate; // / (initializeParams.frameRateNum / initializeParams.frameRateDen);
	//initializeParams.encodeConfig->rcParams.vbvInitialDelay = initializeParams.encodeConfig->rcParams.vbvInitialDelay;
	initializeParams.encodeConfig->rcParams.rateControlMode = NV_ENC_PARAMS_RC_CBR_LOWDELAY_HQ;
	//initializeParams.encodeConfig->rcParams.qpMapMode = NV_ENC_QP_MAP_DELTA;
	initializeParams.encodeConfig->rcParams.disableBadapt = 1;
	initializeParams.encodeConfig->rcParams.vbvBufferSize = initializeParams.encodeConfig->rcParams.averageBitRate * 60 / 1;
	initializeParams.encodeConfig->rcParams.vbvInitialDelay = initializeParams.encodeConfig->rcParams.vbvBufferSize;
	initializeParams.encodeConfig->frameIntervalP = 1;
	initializeParams.encodeConfig->rcParams.enableAQ = 1;
	initializeParams.encodeConfig->encodeCodecConfig.h264Config.idrPeriod = NVENC_INFINITE_GOPLENGTH;
	initializeParams.encodeConfig->encodeCodecConfig.h264Config.repeatSPSPPS = 1;
	initializeParams.encodeConfig->encodeCodecConfig.h264Config.sliceMode = 0;
	initializeParams.encodeConfig->encodeCodecConfig.h264Config.sliceModeData = 0;
	enc->nvenc->CreateEncoder(&initializeParams);

	return true;
}

int nvenc_encode_texture(void *nvenc_data, ID3D11Texture2D *texture,int * ready, uint8_t* out_buf, uint32_t out_buf_size)
{
	using namespace chen;
	//ERROR_EX_LOG("");
	if (nvenc_data == nullptr) 
	{
		ERROR_EX_LOG("nvenc_data == nullptr");
		return -1;
	}

	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;	

	std::lock_guard<std::mutex> locker(enc->mutex);

	if (enc->nvenc == nullptr)
	{
		ERROR_EX_LOG("evenc ");
		return -1;
	}

	std::vector<std::vector<uint8_t>> packet;
	const NvEncInputFrame* input_frame = enc->nvenc->GetNextInputFrame();
	ID3D11Texture2D *encoder_texture = reinterpret_cast<ID3D11Texture2D*>(input_frame->inputPtr);
	//*ready = 1;
	enc->d3d11_context->CopyResource(encoder_texture, texture);
	//*ready = 0;
	try
	{
		enc->nvenc->EncodeFrame(packet);
	}
	catch (...)
	{
		ERROR_EX_LOG("nvenc encode frame failed !!! packet ");
		return -2;
	}
	

	int frame_size = 0;
	for (std::vector<uint8_t> &packet : packet) {
		if (frame_size + packet.size() < out_buf_size) {
			memcpy(out_buf + frame_size, packet.data(), packet.size());
			frame_size += (int)packet.size();
		}
		else {
			break;
		}
	}

	return frame_size;
}

int nvenc_encode_handle(void *nvenc_data, HANDLE handle, int lock_key, int unlock_key, 
	uint8_t* out_buf, uint32_t out_buf_size)
{
	using namespace chen;
	NORMAL_EX_LOG("");
	struct video_data {
		int ready;
		void * handler;
	};
	video_data * video_data_ptr = (video_data*)handle;
	NORMAL_EX_LOG("");
	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;
	if (nvenc_data == nullptr || handle == nullptr || video_data_ptr->handler == nullptr) {
		return 0;
	}
	
	ID3D11Texture2D* input_texture = enc->input_texture;
	IDXGIKeyedMutex* keyed_mutex = enc->keyed_mutex;
	int frame_size = 0;
	NORMAL_EX_LOG("[enc->input_handle = %p][handle = %p]", enc->input_handle, video_data_ptr->handler);
	if (enc->input_handle != video_data_ptr->handler) {
		if (enc->input_texture) {
			enc->input_texture->Release();
			enc->input_texture = nullptr;
		}
		NORMAL_EX_LOG("");
		if (enc->keyed_mutex) {
			enc->keyed_mutex->Release();
			enc->keyed_mutex = nullptr;
		}
		NORMAL_EX_LOG("");
		HRESULT hr = enc->d3d11_device->OpenSharedResource((HANDLE)(uintptr_t)video_data_ptr->handler, __uuidof(ID3D11Texture2D),
			reinterpret_cast<void **>(&enc->input_texture));
		if (FAILED(hr)) {
			return -1;
		}
		NORMAL_EX_LOG("");
		input_texture = enc->input_texture;
		NORMAL_EX_LOG("");
		if (lock_key >= 0 && unlock_key >= 0)
		{
			hr = input_texture->QueryInterface( _uuidof(IDXGIKeyedMutex), reinterpret_cast<void**>(&enc->keyed_mutex));
			NORMAL_EX_LOG("hr = %u", hr);
			if (FAILED(hr))
			{
				enc->input_texture->Release();
				enc->input_texture = nullptr;
				return -1;
			}
			NORMAL_EX_LOG("");
			keyed_mutex = enc->keyed_mutex;
		}
		NORMAL_EX_LOG("");
		enc->input_handle = video_data_ptr->handler;
	}
	NORMAL_EX_LOG("");
	if (input_texture != nullptr) {
		if (lock_key >= 0 && unlock_key >= 0 && keyed_mutex) {
			HRESULT hr = keyed_mutex->AcquireSync(lock_key, 3);
			if (hr != S_OK) 
			{
				NORMAL_EX_LOG("AcquireSync time out !!!");
				return -1;
			}
		}
		NORMAL_EX_LOG("");
		//video_data_ptr->ready = 1;
		frame_size = nvenc_encode_texture(enc, input_texture, &video_data_ptr->ready, out_buf, out_buf_size);
		//video_data_ptr->ready = 0;
		NORMAL_EX_LOG("");
		if (lock_key >= 0 && unlock_key >= 0 && keyed_mutex) {
			keyed_mutex->ReleaseSync(unlock_key);
		}
	}
	NORMAL_EX_LOG("");
	return frame_size;
}

int nvenc_set_bitrate(void *nvenc_data, uint32_t bitrate_bps)
{
	if (nvenc_data == nullptr) {
		return 0;
	}
	using namespace chen;
	if ((bitrate_bps / 1000) > g_cfg.get_uint32(ECI_RtcAvgRate))
	{
		WARNING_EX_LOG("[bitrate_bps = %u ]too big [defalut bitrate = %u]", bitrate_bps/ 1000, g_cfg.get_uint32(ECI_RtcAvgRate));
		bitrate_bps = g_cfg.get_uint32(ECI_RtcAvgRate) * 1000;
	}
	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);

	if (enc->nvenc != nullptr)
	{
		NV_ENC_RECONFIGURE_PARAMS reconfigureParams;
		NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
		encodeConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CBR_LOWDELAY_HQ;
		reconfigureParams.version = NV_ENC_RECONFIGURE_PARAMS_VER;
		reconfigureParams.forceIDR = true;
		//reconfigureParams.reInitEncodeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
				//reconfigureParams.reInitEncodeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
		{
			reconfigureParams.reInitEncodeParams.version = NV_ENC_INITIALIZE_PARAMS_VER;
			reconfigureParams.reInitEncodeParams.encodeGUID = NV_ENC_CODEC_H264_GUID;
			//reconfigureParams.reInitEncodeParams.presetGUID = NV_ENC_PR;
			reconfigureParams.reInitEncodeParams.frameRateNum = 60 /*CurrentConfig.MaxFramerate*/;
			reconfigureParams.reInitEncodeParams.frameRateDen = 1;
			reconfigureParams.reInitEncodeParams.enablePTD = 1;
			reconfigureParams.reInitEncodeParams.reportSliceOffsets = 0;
			reconfigureParams.reInitEncodeParams.enableSubFrameWrite = 0;
			reconfigureParams.reInitEncodeParams.maxEncodeWidth = 4096;
			reconfigureParams.reInitEncodeParams.maxEncodeHeight = 4096;
			//reconfigureParams.reInitEncodeParams.darHeight = NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY;

		}
		reconfigureParams.reInitEncodeParams.encodeConfig = &encodeConfig;
		enc->nvenc->GetInitializeParams(&reconfigureParams.reInitEncodeParams);
		reconfigureParams.reInitEncodeParams.encodeConfig->rcParams.averageBitRate = bitrate_bps; // bitrate_bps;
		reconfigureParams.reInitEncodeParams.encodeConfig->rcParams.maxBitRate = g_cfg.get_uint32(ECI_RtcMaxRate) * 1000; // bitrate_bps;
		enc->nvenc->Reconfigure(&reconfigureParams);
	}

	return 0;
}

int nvenc_set_framerate(void *nvenc_data, uint32_t framerate) 
{
	if (nvenc_data == nullptr) {
		return 0;
	}
	using namespace chen;
	if (framerate < g_cfg.get_uint32(ECI_RtcFrames))
	{
		WARNING_EX_LOG("framerate = %u", framerate);
	}
	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);

	if (enc->nvenc != nullptr) {
		NV_ENC_RECONFIGURE_PARAMS reconfigureParams;
		NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
		encodeConfig.gopLength = g_cfg.get_uint32(ECI_RtcVideoGop);
		reconfigureParams.version = NV_ENC_RECONFIGURE_PARAMS_VER;
		reconfigureParams.forceIDR = true;
		
		//reconfigureParams.reInitEncodeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
		{
			reconfigureParams.reInitEncodeParams.version = NV_ENC_INITIALIZE_PARAMS_VER;
			reconfigureParams.reInitEncodeParams.encodeGUID = NV_ENC_CODEC_H264_GUID;
			//reconfigureParams.reInitEncodeParams.presetGUID = NV_ENC_PR;
			reconfigureParams.reInitEncodeParams.frameRateNum = 60 /*CurrentConfig.MaxFramerate*/;
			reconfigureParams.reInitEncodeParams.frameRateDen = 1;
			reconfigureParams.reInitEncodeParams.enablePTD = 1;
			reconfigureParams.reInitEncodeParams.reportSliceOffsets = 0;
			reconfigureParams.reInitEncodeParams.enableSubFrameWrite = 0;
			reconfigureParams.reInitEncodeParams.maxEncodeWidth = 4096;
			reconfigureParams.reInitEncodeParams.maxEncodeHeight = 4096;
			//reconfigureParams.reInitEncodeParams.darHeight = NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY;
		
		}

		reconfigureParams.reInitEncodeParams.encodeConfig = &encodeConfig;
		enc->nvenc->GetInitializeParams(&reconfigureParams.reInitEncodeParams);
		reconfigureParams.reInitEncodeParams.frameRateNum = framerate;
		enc->nvenc->Reconfigure(&reconfigureParams);
	}

	return 0;
}

int nvenc_request_idr(void *nvenc_data)
{
	using namespace chen;
	//NORMAL_EX_LOG("");
	if (nvenc_data == nullptr)
	{
		return 0;
	}

	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);

	if (enc->nvenc != nullptr) 
	{
		NORMAL_EX_LOG("request Keyframe");
		enc->nvenc->ForceIDR();
	}
	else
	{
		WARNING_EX_LOG("not request Keyframe");
	}

	return 0;
}

int nvenc_get_sequence_params(void *nvenc_data, uint8_t* outBuf, uint32_t max_buf_size)
{
	using namespace chen;
	//ERROR_EX_LOG("");
	if (nvenc_data == nullptr) {
		return 0;
	}

	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);

	if (enc->nvenc != nullptr) {
		std::vector<uint8_t> seq_params;
		enc->nvenc->GetSequenceParams(seq_params);
		if (seq_params.size() > 0 && seq_params.size() < max_buf_size) {
			memcpy(outBuf, seq_params.data(), seq_params.size());
			return (int)seq_params.size();
		}		
	}

	return 0;
}

int nvenc_set_region_of_interest(void* nvenc_data, int x, int y, int width, int height, int delta_qp)
{
	if (nvenc_data == nullptr) {
		return 0;
	}
	using namespace chen;
	//ERROR_EX_LOG("");
	struct nvenc_data* enc = (struct nvenc_data*)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);

	if (enc->nvenc != nullptr){ 
		enc->nvenc->SetROI(x, y, width, height, delta_qp);
	}

	return 0;
}

static ID3D11Device* get_device(void *nvenc_data)
{	
	using namespace chen;
	//ERROR_EX_LOG("");
	if (nvenc_data == nullptr) {
		return nullptr;
	}

	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);
	return enc->d3d11_device;
}

static ID3D11Texture2D* get_texture(void *nvenc_data)
{
	using namespace chen;
	//ERROR_EX_LOG("");
	if (nvenc_data == nullptr) {
		return nullptr;
	}

	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);
	return enc->copy_texture;
}


static ID3D11DeviceContext* get_context(void *nvenc_data)
{
	using namespace chen;
	//ERROR_EX_LOG("");
	if (nvenc_data == nullptr) {
		return nullptr;
	}

	struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;

	std::lock_guard<std::mutex> locker(enc->mutex);
	return enc->d3d11_context;
}

struct encoder_info nvenc_info = {
	is_supported,
	nvenc_create,
	nvenc_destroy,
	nvenc_init,
	nvenc_encode_texture,
	nvenc_encode_handle,
	nvenc_set_bitrate,
	nvenc_set_framerate,
	nvenc_request_idr,
	nvenc_get_sequence_params,
	nvenc_set_region_of_interest,
	get_device,
	get_texture,
	get_context
};

}