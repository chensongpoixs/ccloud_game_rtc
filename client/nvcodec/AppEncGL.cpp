// /*
// * Copyright 2017-2018 NVIDIA Corporation.  All rights reserved.
// *
// * Please refer to the NVIDIA end user license agreement (EULA) associated
// * with this source code for terms and conditions that govern your use of
// * this software. Any use, reproduction, disclosure, or distribution of
// * this software and related documentation outside the terms of the EULA
// * is strictly prohibited.
// *
// */

// #include <iostream>
// #include <memory>
// #include <stdint.h>
// #include "Logger.h"
// #include "NvEncoderGL.h"
// #include "NvEncoderCLIOptions.h"
// #include "NvCodecUtils.h"
// #include <GL/glew.h>
// #include <GL/glut.h>
// #include <GL/freeglut_ext.h>


// // simplelogger::Logger *logger = simplelogger::LoggerFactory::CreateConsoleLogger();

// void ShowHelpAndExit(const char *szBadOption = NULL)
// {
//     bool bThrowError = false;
//     std::ostringstream oss;
//     if (szBadOption)
//     {
//         bThrowError = true;
//         oss << "Error parsing \"" << szBadOption << "\"" << std::endl;
//     }
//     oss << "Options:" << std::endl
//         << "-i           Input file path" << std::endl
//         << "-o           Output file path" << std::endl
//         << "-s           Input resolution in this form: WxH" << std::endl
//         << "-if          Input format: iyuv nv12" << std::endl
//         ;
//     oss << NvEncoderInitParam().GetHelpMessage(false, false, true);
//     if (bThrowError)
//     {
//         throw std::invalid_argument(oss.str());
//     }
//     else
//     {
//         std::cout << oss.str();
//         exit(0);
//     }
// }

// void ParseCommandLine(int argc, char *argv[], char *szInputFileName, int &nWidth, int &nHeight,
//     NV_ENC_BUFFER_FORMAT &eFormat, char *szOutputFileName, NvEncoderInitParam &initParam)
// {
//     std::ostringstream oss;
//     int i;
//     for (i = 1; i < argc; i++)
//     {
//         if (!_stricmp(argv[i], "-h"))
//         {
//             ShowHelpAndExit();
//         }
//         if (!_stricmp(argv[i], "-i"))
//         {
//             if (++i == argc)
//             {
//                 ShowHelpAndExit("-i");
//             }
//             sprintf(szInputFileName, "%s", argv[i]);
//             continue;
//         }
//         if (!_stricmp(argv[i], "-o"))
//         {
//             if (++i == argc)
//             {
//                 ShowHelpAndExit("-o");
//             }
//             sprintf(szOutputFileName, "%s", argv[i]);
//             continue;
//         }
//         if (!_stricmp(argv[i], "-s"))
//         {
//             if (++i == argc || 2 != sscanf(argv[i], "%dx%d", &nWidth, &nHeight))
//             {
//                 ShowHelpAndExit("-s");
//             }
//             continue;
//         }

//         std::vector<std::string> vszFileFormatName = { "iyuv", "nv12" };

//         NV_ENC_BUFFER_FORMAT aFormat[] =
//         {
//             NV_ENC_BUFFER_FORMAT_IYUV,
//             NV_ENC_BUFFER_FORMAT_NV12,
//         };

//         if (!_stricmp(argv[i], "-if"))
//         {
//             if (++i == argc)
//             {
//                 ShowHelpAndExit("-if");
//             }
//             auto it = std::find(vszFileFormatName.begin(), vszFileFormatName.end(), argv[i]);
//             if (it == vszFileFormatName.end())
//             {
//                 ShowHelpAndExit("-if");
//             }
//             eFormat = aFormat[it - vszFileFormatName.begin()];
//             continue;
//         }
//         // Regard as encoder parameter
//         if (argv[i][0] != '-')
//         {
//             ShowHelpAndExit(argv[i]);
//         }
//         oss << argv[i] << " ";
//         while (i + 1 < argc && argv[i + 1][0] != '-')
//         {
//             oss << argv[++i] << " ";
//         }
//     }
//     initParam = NvEncoderInitParam(oss.str().c_str());
// }

// void EncodeGL(char *szInFilePath, char *szOutFilePath, int nWidth, int nHeight,
//     NV_ENC_BUFFER_FORMAT eFormat, NvEncoderInitParam *encodeCLIOptions)
// {
//     std::ifstream fpIn(szInFilePath, std::ifstream::in | std::ifstream::binary);
//     if (!fpIn)
//     {
//         std::ostringstream err;
//         err << "Unable to open input file: " << szInFilePath << std::endl;
//         throw std::invalid_argument(err.str());
//     }

//     std::ofstream fpOut(szOutFilePath, std::ios::out | std::ios::binary);
//     if (!fpOut)
//     {
//         std::ostringstream err;
//         err << "Unable to open output file: " << szOutFilePath << std::endl;
//         throw std::invalid_argument(err.str());
//     }

//     NvEncoderGL enc(nWidth, nHeight, eFormat);

//     NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
//     NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
//     initializeParams.encodeConfig = &encodeConfig;
//     enc.CreateDefaultEncoderParams(&initializeParams, encodeCLIOptions->GetEncodeGUID(),
//         encodeCLIOptions->GetPresetGUID());

//     encodeCLIOptions->SetInitParams(&initializeParams, eFormat);

//     enc.CreateEncoder(&initializeParams);

//     int nFrameSize = enc.GetFrameSize();
//     std::unique_ptr<uint8_t[]> pHostFrame(new uint8_t[nFrameSize]);
//     int nFrame = 0;
//     while (true)
//     {
//         std::streamsize nRead = fpIn.read(reinterpret_cast<char*>(pHostFrame.get()), nFrameSize).gcount();

//         const NvEncInputFrame* encoderInputFrame = enc.GetNextInputFrame();
//         NV_ENC_INPUT_RESOURCE_OPENGL_TEX *pResource = (NV_ENC_INPUT_RESOURCE_OPENGL_TEX *)encoderInputFrame->inputPtr;

//        /* 
//            const EGLAttrib pixmap_attrs[] = {
// 		    EGL_IMAGE_PRESERVED_KHR,
// 		    EGL_TRUE,
// 		    EGL_NONE,
// 	    };

//        EGLImage image = eglCreateImage(egl_display, EGL_NO_CONTEXT,
//             EGL_NATIVE_PIXMAP_KHR, pixmap,
//             pixmap_attrs);*/
//         glBindTexture(pResource->target, pResource->texture);
//         glTexSubImage2D(pResource->target, 0, 0, 0, nWidth, nHeight * 3 / 2, GL_RED, GL_UNSIGNED_BYTE, pHostFrame.get());
//         glBindTexture(pResource->target, 0);
//         //eglDestroyImage(egl_display, image);
//         std::vector<std::vector<uint8_t>> vPacket;
//         if (nRead == nFrameSize)
//         {
//             enc.EncodeFrame(vPacket);
//         }
//         else
//         {
//             enc.EndEncode(vPacket);
//         }
//         nFrame += (int)vPacket.size();
//         for (std::vector<uint8_t> &packet : vPacket)
//         {
//             fpOut.write(reinterpret_cast<char*>(packet.data()), packet.size());
//         }
//         if (nRead != nFrameSize) break;
//     }

//     enc.DestroyEncoder();

//     fpOut.close();
//     fpIn.close();

//     std::cout << "Total frames encoded: " << nFrame << std::endl;
//     std::cout << "Saved in file " << szOutFilePath << std::endl;
// }

// /**
// *  This sample application illustrates encoding of frames stored in OpenGL
// *  textures. The application reads frames from the input file and uploads them
// *  to the textures obtained from the encoder using NvEncoder::GetNextInputFrame().
// *  The encoder subsequently maps the textures for encoder using NvEncodeAPI and
// *  submits them to NVENC hardware for encoding as part of EncodeFrame() function.
// */

// int sample_application_main(int argc, char **argv)
// {
//     char szInFilePath[256] = "",
//         szOutFilePath[256] = "";
//     int nWidth = 1920, nHeight = 1080;
//     NV_ENC_BUFFER_FORMAT eFormat = NV_ENC_BUFFER_FORMAT_IYUV;
//     NvEncoderInitParam encodeCLIOptions;

//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
//     glutInitWindowSize(16, 16);
//     int window = glutCreateWindow("AppEncGL");
//     if (!window)
//     {
//         std::cout << "Unable to create GLUT window." << std::endl;
//         return 1;
//     }
//     glutHideWindow();

//     try
//     {
//         ParseCommandLine(argc, argv, szInFilePath, nWidth, nHeight, eFormat,
//             szOutFilePath, encodeCLIOptions);

//         CheckInputFile(szInFilePath);

//         if (!*szOutFilePath)
//         {
//             sprintf(szOutFilePath, encodeCLIOptions.IsCodecH264() ? "out.h264" : "out.hevc");
//         }

//         EncodeGL(szInFilePath, szOutFilePath, nWidth, nHeight, eFormat, &encodeCLIOptions);
//     }
//     catch (const std::exception &e)
//     {
//         std::cout << e.what();
//         return 1;
//     }

//     glutDestroyWindow(window);

//     return 0;
// }



// #define _GNU_SOURCE
// #include <sys/types.h>
// #include <dlfcn.h>
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/syscall.h>
// #include <sys/time.h>
// //#include <opencl-c.h>
// // #include <GL/glu.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <time.h>
// #include <fcntl.h>
// // #include <GL/glut.h>
// #include <stdio.h>
// // #include <GL/glew.h>
// #include <stdlib.h>
// // #include <SDL2/SDL.h>
// //#include <Xlib.h>
// #include <X11/Xlib.h>
// #include <EGL/egl.h>
// // #include <GL/glx.h>


// #include "clog.h"
// // #include "cgl_egl_common.h"
// #include "NvEncoderGL.h"
// #include "NvEncoderCLIOptions.h"
// #include <GL/glew.h>
// #include <GL/glut.h>
// #include <GL/freeglut_ext.h>
 

// NvEncoderGL   * nv_opengl_ptr = NULL;
// int width = 1900;
// int height = 980;

// static inline int gl_error(const char *func, const char *str)
// {
// 	GLenum error = glGetError();
// 	if (error != 0) {
//         char buffer[100];
//         (void) sprintf(buffer, "%s: %s: %lu\n", func, str, error);
//     (void) write(1, buffer, strlen(buffer));
		 
// 		return 1;
// 	}

// 	return 0;
// }
// void init_nvencode()
// {

//     nv_opengl_ptr= new NvEncoderGL  (width,  height, NV_ENC_BUFFER_FORMAT_IYUV);
     
//     NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
//     NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
//     initializeParams.encodeConfig = &encodeConfig;
//     nv_opengl_ptr->CreateDefaultEncoderParams(&initializeParams, NV_ENC_CODEC_H264_GUID, NV_ENC_PRESET_P3_GUID);
     
//     // encodeCLIOptions.SetInitParams(&initializeParams, NV_ENC_BUFFER_FORMAT_IYUV);
//     initializeParams.encodeWidth =  width;
// 	initializeParams.encodeHeight =   height;
// 	initializeParams.version = NV_ENC_INITIALIZE_PARAMS_VER;
// 	initializeParams.encodeGUID = NV_ENC_CODEC_H264_GUID;
// 	initializeParams.presetGUID = NV_ENC_PRESET_P4_GUID;
	 
// 	initializeParams.frameRateDen = 1;
// 	initializeParams.enablePTD = 1;
// 	initializeParams.reportSliceOffsets = 0;
// 	initializeParams.enableSubFrameWrite = 0;
	
// 	initializeParams.tuningInfo = NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY;
// 	initializeParams.frameRateNum = 60;


// 	///////////////
// 	// H.264 specific settings
// 	///
	
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.enableIntraRefresh = 1;
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.intraRefreshPeriod = 180;
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.intraRefreshCnt = 180;
// 	//initializeParams.encodeConfig->encodeCodecConfig.h264Config.idrPeriod = 
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.repeatSPSPPS = 1;
// 	/*
// 	* Slice mode - set the slice mode to "entire frame as a single slice" because WebRTC implementation doesn't work well with slicing. The default slicing mode
// 	* produces (rarely, but especially under packet loss) grey full screen or just top half of it.
// 	*/
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.sliceMode = 0;
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.sliceModeData = 0;
// 	// `outputPictureTimingSEI` is used in CBR mode to fill video frame with data to match the requested bitrate.
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.outputPictureTimingSEI = 1;
// 	initializeParams.encodeConfig->encodeCodecConfig.h264Config.enableFillerDataInsertion = 1;
// 	//initializeParams.encodeConfig->gopLength =  g_cfg.get_uint32(ECI_EncoderVideoGop);//NVENC_INFINITE_GOPLENGTH;//
// 	//initializeParams.encodeConfig->rcParams.averageBitRate = g_cfg.get_uint32(ECI_RtcAvgRate) * 1000 ;
// 	//initializeParams.encodeConfig->rcParams.maxBitRate = g_cfg.get_uint32(ECI_RtcMaxRate) * 1000;
// 	//initializeParams.encodeConfig->rcParams.rateControlMode = NV_ENC_PARAMS_RC_VBR;// NV_ENC_PARAMS_RC_VBR_HQ;// NV_ENC_PARAMS_RC_CBR_LOWDELAY_HQ;
// 	//initializeParams.encodeConfig->rcParams.qpMapMode = NV_ENC_QP_MAP_DELTA;
// 	NV_ENC_RC_PARAMS& RateControlParams = initializeParams.encodeConfig->rcParams;
// #define DEFAULT_BITRATE (1000000u)
// 	uint32_t const MinQP = static_cast<uint32_t>(1);
// 	uint32_t const MaxQP = static_cast<uint32_t>(51);
// 	RateControlParams.rateControlMode = NV_ENC_PARAMS_RC_VBR;
// 	RateControlParams.averageBitRate = 8000 * 1000;// DEFAULT_BITRATE;
// 	RateControlParams.maxBitRate = 100000 * 1000;// DEFAULT_BITRATE; // Not used for CBR
// 	RateControlParams.multiPass = NV_ENC_TWO_PASS_FULL_RESOLUTION;
// 	RateControlParams.minQP = { MinQP, MinQP, MinQP };
// 	RateControlParams.maxQP = { MaxQP, MaxQP, MaxQP };
// 	RateControlParams.enableMinQP = 1;
// 	RateControlParams.enableMaxQP = 1;
  
//     nv_opengl_ptr->CreateEncoder(&initializeParams);

// }


// static void (*hook_egXSwapBuffers)(Display *d, unsigned long e) = NULL;


// #define NV_LOG_SHOW()   



// // { struct timeval tTimeVal; \
// //     gettimeofday(&tTimeVal, NULL);  \
// //     struct tm *tTM = localtime(&tTimeVal); \
// //     char buffer[1024] = {0};  \
// //     (void)sprintf(buffer, " [%s][%d] ======================>[%04d-%02d-%02d %02d:%02d:%02d.%03ld.%03ld]\n",  __FUNCTION__, __LINE__, tTM->tm_year +1900, tTM->tm_mon +1, tTM->tm_mday, tTM->tm_hour, tTM->tm_min, tTM->tm_sec, tTimeVal.tv_usec/ 1000, tTimeVal.tv_usec %1000); \
// //     (void)write(1, buffer, strlen(buffer));} 


// void glXSwapBuffers(Display *d, unsigned long e)
// {
//     if (!hook_egXSwapBuffers)
//    {
//        hook_egXSwapBuffers = dlsym(RTLD_NEXT, "glXSwapBuffers");
//    }
//    NV_LOG_SHOW();
 
//     if (!nv_opengl_ptr)
//     {
//         init_nvencode();
//     }
//         static std::ofstream fpOut("test_h264.h264", std::ios::out | std::ios::binary);
//         if (!fpOut)
//         {
//             std::ostringstream err;
//             err << "Unable to open output file: test_h264.h264" << std::endl;
//             throw std::invalid_argument(err.str());
//         }
//         NV_LOG_SHOW();
//         ////////////////////////////////////////////////////////////
//          glReadBuffer(GL_BACK);
//             gl_error("glReadBuffer", "glReadBuffer failed !!!");
//             // glGenTextures(1, &_textureInfo.texture);
//             // glGenTextures(1, & texture);
//             //  glBindTexture(GL_TEXTURE_2D, texture);
//             NV_LOG_SHOW();
//              const NvEncInputFrame* encoderInputFrame = nv_opengl_ptr->GetNextInputFrame();
//         NV_ENC_INPUT_RESOURCE_OPENGL_TEX *pResource = (NV_ENC_INPUT_RESOURCE_OPENGL_TEX *)encoderInputFrame->inputPtr;
//  NV_LOG_SHOW();
//             // glBindFramebuffer(GL_FRAMEBUFFER, pResource->framebuffer);
//             // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, new_texture, 0);

//             // 渲染到帧缓冲
//             // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//             //             gl_error("glBindFramebuffer", "glBindFramebuffer failed !!!");
//             //             // 渲染代码省略

//             // // 将帧缓冲的数据拷贝到纹理
//             // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//             glBindFramebuffer(GL_FRAMEBUFFER, pResource->framebuffer);
//             gl_error("glBindFramebuffer", "glBindFramebuffer GL_TEXTURE_2D failed !!!");
//              NV_LOG_SHOW();
//           //  glBindTexture(GL_TEXTURE_RECTANGLE, new_texture);
//           glBindTexture(pResource->target, pResource->texture);
//             gl_error("glBindTexture", "glBindTexture GL_TEXTURE_2D failed !!!");
//              NV_LOG_SHOW();
//             glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, 0, 0, width, height);
//               gl_error("glBindTexture", "glBindTexture GL_TEXTURE_2D failed !!!");
//                NV_LOG_SHOW();
//             glBindTexture(pResource->target, 0); 
//               gl_error("glBindTexture", "glBindTexture GL_TEXTURE_2D failed !!!");
//                NV_LOG_SHOW();
//         ////////////////////////////////////////////////////////////
       
//         // glBindTexture(pResource->target, pResource->texture);
//         // glTexSubImage2D(pResource->target, 0, 0, 0, nWidth, nHeight * 3 / 2, GL_RED, GL_UNSIGNED_BYTE, pHostFrame.get());
//         // glBindTexture(pResource->target, 0); 
//         std::vector<std::vector<uint8_t>> vPacket;
//         // if (nRead == nFrameSize)
//         // {
//             nv_opengl_ptr->EncodeFrame(vPacket);
//         // }
//         // else
//         // {
//         //     enc.EndEncode(vPacket);
//         // }
//         // nFrame += (int)vPacket.size();
//         for (std::vector<uint8_t> &packet : vPacket)
//         {
//             fpOut.write(reinterpret_cast<char*>(packet.data()), packet.size());
//         }
//          NV_LOG_SHOW();


//          hook_egXSwapBuffers(d, e);

// }
