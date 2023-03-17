// /***********************************************************************************************
// created: 		2020-2-13

// author:			chensong

// purpose:		nvenc
// ************************************************************************************************/


// #include "cnvenc.h"
// #include <cstdint>
// #include <string>
// // #include ""
// namespace chen {

//     struct nvenc_data
//     {
//         std::mutex mutex;
//         uint32_t width     = 0;
//         uint32_t height    = 0;
//         uint32_t framerate = 0;
//         uint32_t bitrate   = 0;
//         uint32_t gop       =  0/*NVENC_INFINITE_GOPLENGTH*/;
//         std::string codec = "h264";
//         uint32_t format = 0;
//         NvEncoderGL *nvenc = nullptr;
//     };
//     static bool is_supported(void)
//     {

//     #if defined(_WIN32)
//     #if defined(_WIN64)
//         HMODULE hModule = LoadLibrary(TEXT("nvEncodeAPI64.dll"));
//     #else
//         HMODULE hModule = LoadLibrary(TEXT("nvEncodeAPI.dll"));
//     #endif
//     #else
//         void *hModule = dlopen("libnvidia-encode.so.1", RTLD_LAZY);
//     #endif

//         if (hModule == NULL)
//         {
//             NVENC_THROW_ERROR("NVENC library file is not found. Please ensure NV driver is installed", NV_ENC_ERR_NO_ENCODE_DEVICE);
//         }

//         m_hModule = hModule;

//         typedef NVENCSTATUS(NVENCAPI *NvEncodeAPIGetMaxSupportedVersion_Type)(uint32_t*);
//     #if defined(_WIN32)
//         NvEncodeAPIGetMaxSupportedVersion_Type NvEncodeAPIGetMaxSupportedVersion = (NvEncodeAPIGetMaxSupportedVersion_Type)GetProcAddress(hModule, "NvEncodeAPIGetMaxSupportedVersion");
//     #else
//         NvEncodeAPIGetMaxSupportedVersion_Type NvEncodeAPIGetMaxSupportedVersion = (NvEncodeAPIGetMaxSupportedVersion_Type)dlsym(hModule, "NvEncodeAPIGetMaxSupportedVersion");
//     #endif

//         uint32_t version = 0;
//         uint32_t currentVersion = (NVENCAPI_MAJOR_VERSION << 4) | NVENCAPI_MINOR_VERSION;
//         NVENC_API_CALL(NvEncodeAPIGetMaxSupportedVersion(&version));
//         if (currentVersion > version)
//         {
//             NVENC_THROW_ERROR("Current Driver Version does not support this NvEncodeAPI version, please upgrade driver", NV_ENC_ERR_INVALID_VERSION);
//         }

//     //     typedef NVENCSTATUS(NVENCAPI *NvEncodeAPICreateInstance_Type)(NV_ENCODE_API_FUNCTION_LIST*);
//     // #if defined(_WIN32)
//     //     NvEncodeAPICreateInstance_Type NvEncodeAPICreateInstance = (NvEncodeAPICreateInstance_Type)GetProcAddress(hModule, "NvEncodeAPICreateInstance");
//     // #else
//     //     NvEncodeAPICreateInstance_Type NvEncodeAPICreateInstance = (NvEncodeAPICreateInstance_Type)dlsym(hModule, "NvEncodeAPICreateInstance");
//     // #endif

//     //     if (!NvEncodeAPICreateInstance)
//     //     {
//     //         NVENC_THROW_ERROR("Cannot find NvEncodeAPICreateInstance() entry in NVENC library", NV_ENC_ERR_NO_ENCODE_DEVICE);
//     //     }

//     //     m_nvenc = { NV_ENCODE_API_FUNCTION_LIST_VER };
//     //     NVENC_API_CALL(NvEncodeAPICreateInstance(&m_nvenc));
//         return true;
//     }

//     static void* nvenc_create()
//     {

//         using namespace chen;
//         //ERROR_EX_LOG("");
//         if (!is_supported()) {
//             return nullptr;
//         }
//         struct nvenc_data* enc = new nvenc_data;
//         return  enc;
//     }

//     static void nvenc_destroy(void **nvenc_data)
//     {
//         using namespace chen;
// 	//ERROR_EX_LOG("");
//         struct nvenc_data *enc = (struct nvenc_data *)(*nvenc_data);

//         enc->mutex.lock();
    

//         enc->mutex.unlock();

//         delete *nvenc_data;
//         *nvenc_data = nullptr;
//     }



//     static bool nvenc_init(void *nvenc_data, void *encoder_config)
//     {
//         using namespace chen;
//         //ERROR_EX_LOG("");
//         if (nvenc_data == nullptr) 
//         {
//             return false;
//         }

//         struct nvenc_data *enc = (struct nvenc_data *)nvenc_data;
//         struct encoder_config* config = (struct encoder_config*)encoder_config;

//         std::lock_guard<std::mutex> locker(enc->mutex);
//         if (enc->nvenc != nullptr) 
//         {
//             return false;
//         }
//         enc->width = config->width;
//         enc->height = config->height;
//         enc->framerate = config->framerate;
//         enc->format = config->format;
//         enc->codec = config->codec;
//         enc->gop = NVENC_INFINITE_GOPLENGTH; // config->gop;
//         enc->bitrate = config->bitrate;
//          int32 nWidth = config->width;
//         int32 nHeight = config->height;
//         NV_ENC_BUFFER_FORMAT eFormat = NV_ENC_BUFFER_FORMAT_IYUV;
        
//          NORMAL_EX_LOG("");
//         init_gl();
//          NORMAL_EX_LOG("");
//          std::ostringstream oss;
//             oss << "-codec h264 -fps 25 ";
//           NvEncoderInitParam encodeCLIOptions(oss.str().c_str());
        
//         NORMAL_EX_LOG("");
//           enc->nvenc = new NvEncoderGL  (nWidth, nHeight, eFormat);
//         NORMAL_EX_LOG("");
//         NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
//         NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
//         initializeParams.encodeConfig = &encodeConfig;
//         enc->nvenc->CreateDefaultEncoderParams(&initializeParams, encodeCLIOptions.GetEncodeGUID(), encodeCLIOptions.GetPresetGUID());
//         NORMAL_EX_LOG("");
//         encodeCLIOptions.SetInitParams(&initializeParams, eFormat);
//         NORMAL_EX_LOG("");
//         enc->nvenc->CreateEncoder(&initializeParams);
//         NORMAL_EX_LOG(""); 
//         return true;
//     }

// }