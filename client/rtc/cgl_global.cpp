/***********************************************************************************************
created: 		2022-08-17

author:			chensong

purpose:		linux_app_capture
************************************************************************************************/

#include "cgl_global.h"
// #include <GL/glew.h>
// #include <GL/glut.h>
// #include <GL/freeglut_ext.h>
#include "clog.h"
#include <iostream>
#include <string.h>
#include <X11/Xlib.h>
#include <EGL/egl.h>
// #include <vertor>
// #include <string>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iterator>
#include <cstring>
#include <functional>
// #include <GL/glew.h>
// #include <GL/glut.h>
// #include <GL/freeglut_ext.h>
#include "glad/glad_egl.h"
#include "glad/glad.h"
#include "Logger.h"
#include "NvEncoderGL.h"
#include "NvEncoderCLIOptions.h"
#include "cgl_egl_common.h"
namespace chen {

    //  int window = 0;
     // egl resources
static Display *display;
static int screen;
static Window win = 0;

 static  EGLDisplay eglDisplay = EGL_NO_DISPLAY;
static EGLSurface eglSurface = EGL_NO_SURFACE;
static EGLContext eglContext = EGL_NO_CONTEXT;

// glut window handle
static int window;
    bool init_gl()
    {
//         NORMAL_EX_LOG("");
//         int argc = 1;

//         char buf[64]={"/home/rte/Work/CloudRenderServer/cloudrender"};
//         char *argv[1];
//         argv[0]=buf;
//         //char * argv[] = {"/home/rte/Work/CloudRenderServer/cloudrender"};
//         glutInit(argc, argv);
//         NORMAL_EX_LOG("");
//          glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
// NORMAL_EX_LOG("");
//         //  std::ostringstream oss;
//         //     oss << "-codec h264 -fps 25 ";
//         //   NvEncoderInitParam encodeCLIOptions(oss.str().c_str());
//          glutInitWindowSize(16, 16);
//          NORMAL_EX_LOG("");
//           window  = glutCreateWindow("AppEncGL");
//         //     if (!window)
//         //     {
//         //         std::cout << "Unable to create GLUT window." << std::endl;
//         //         return 1;
//         //     }
//         NORMAL_EX_LOG("");
//             glutHideWindow();

    int xpos = 0;
    int ypos = 0; 
    int width = 16; 
    int height = 16;
    const char *windowname = "AppEncGL";
    bool status = true;

    EGLint configAttrs[] =
    {
        EGL_RED_SIZE,        1,
        EGL_GREEN_SIZE,      1,
        EGL_BLUE_SIZE,       1,
        EGL_DEPTH_SIZE,      16,
        EGL_SAMPLE_BUFFERS,  0,
        EGL_SAMPLES,         0,
        EGL_CONFORMANT,      EGL_OPENGL_BIT,
        EGL_NONE
    };

    EGLint contextAttrs[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    EGLint windowAttrs[] = {EGL_NONE};
    EGLConfig* configList = NULL;
    EGLint configCount;

    display = XOpenDisplay(NULL);

    if (!display)
    {
        std::cout << "\nError opening X display\n";
        return false;
    }

    screen = DefaultScreen(display);

    eglDisplay = eglGetDisplay(display);

    if (eglDisplay == EGL_NO_DISPLAY)
    {
        std::cout << "\nEGL : failed to obtain display\n";
        return false;
    }

    if (!eglInitialize(eglDisplay, 0, 0))
    {
        std::cout << "\nEGL : failed to initialize\n";
        return false;
    }

    if (!eglChooseConfig(eglDisplay, configAttrs, NULL, 0, &configCount) || !configCount)
    {
        std::cout << "\nEGL : failed to return any matching configurations\n";
        return false;
    }

    configList = (EGLConfig*)malloc(configCount * sizeof(EGLConfig));

    if (!eglChooseConfig(eglDisplay, configAttrs, configList, configCount, &configCount) || !configCount)
    {
        std::cout << "\nEGL : failed to populate configuration list\n";
        status = false;
        goto end;
    }

    win = XCreateSimpleWindow(display, RootWindow(display, screen),
                              xpos, ypos, width, height, 0,
                              BlackPixel(display, screen),
                              WhitePixel(display, screen));

    eglSurface = eglCreateWindowSurface(eglDisplay, configList[0],
                                        (EGLNativeWindowType) win, windowAttrs);

    if (!eglSurface)
    {
        std::cout << "\nEGL : couldn't create window surface\n";
        status = false;
        goto end;
    }

    eglBindAPI(EGL_OPENGL_API);

    eglContext = eglCreateContext(eglDisplay, configList[0], NULL, contextAttrs);

    if (!eglContext)
    {
        std::cout << "\nEGL : couldn't create context\n";
        status = false;
        goto end;
    }

    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
    {
        std::cout << "\nEGL : couldn't make context/surface current\n";
        status = false;
        goto end;
    }

end:
    free(configList);
    return status;

        return true;
    }

    void destroy_gl()
    {
        // glutDestroyWindow(window);
    }





    bool encodec_gl(int width, int height, void * pixmap)
    {
         char   szOutFilePath[256] = "test_yuv.h264";
        int32 nWidth = width;
        int32 nHeight = height;
        NV_ENC_BUFFER_FORMAT eFormat = NV_ENC_BUFFER_FORMAT_IYUV;
        
         NORMAL_EX_LOG("");
        init_gl();
         NORMAL_EX_LOG("");
         std::ostringstream oss;
            oss << "-codec h264 -fps 25 ";
          NvEncoderInitParam encodeCLIOptions(oss.str().c_str());
        
        NORMAL_EX_LOG("");
         NvEncoderGL enc(nWidth, nHeight, eFormat);
        NORMAL_EX_LOG("");
        NV_ENC_INITIALIZE_PARAMS initializeParams = { NV_ENC_INITIALIZE_PARAMS_VER };
        NV_ENC_CONFIG encodeConfig = { NV_ENC_CONFIG_VER };
        initializeParams.encodeConfig = &encodeConfig;
        enc.CreateDefaultEncoderParams(&initializeParams, encodeCLIOptions.GetEncodeGUID(), encodeCLIOptions.GetPresetGUID());
        NORMAL_EX_LOG("");
        encodeCLIOptions.SetInitParams(&initializeParams, eFormat);
        NORMAL_EX_LOG("");
        enc.CreateEncoder(&initializeParams);
        NORMAL_EX_LOG("");
        int nFrameSize = enc.GetFrameSize();
    //     std::unique_ptr<uint8_t[]> pHostFrame(new uint8_t[nFrameSize]);
        int nFrame = 0;
        std::ofstream fpOut(szOutFilePath, std::ios::out | std::ios::binary);

         NORMAL_EX_LOG("");
        const NvEncInputFrame* encoderInputFrame = enc.GetNextInputFrame();
        NV_ENC_INPUT_RESOURCE_OPENGL_TEX *pResource = (NV_ENC_INPUT_RESOURCE_OPENGL_TEX *)encoderInputFrame->inputPtr;
        NORMAL_EX_LOG("");

        EGLDisplay egl_display = eglGetCurrentDisplay();
        if (!egl_display)
        {
            ERROR_EX_LOG("[%s][%d][egl_display]\n", __FUNCTION__, __LINE__);
            return ;
        }

        NORMAL_EX_LOG("");
        // EGLImage image = eglCreateImage(egl_display, EGL_NO_CONTEXT,
        //                 0x30B0/*EGL_NATIVE_PIXMAP_KHR*/, pixmap, EGL_TEXTURE_2D);
        // if (image == EGL_NO_IMAGE) 
        // {
        //     ERROR_EX_LOG("[%s][%d][ERROR]Cannot create EGLImage: %s",__FUNCTION__, __LINE__);
        //     // ERROR_EX_LOG("[%s][%d][ERROR]Cannot create EGLImage: %s",__FUNCTION__, __LINE__,
        //     //     gl_egl_error_to_string(eglGetError()));
        //     return ;
        // }
        NORMAL_EX_LOG("");
        glBindTexture(pResource->target, pResource->texture);
        //  gl_egl_create_texture_from_pixmap(pResource , m_win_width, m_win_height, GL_BGRA, EGL_TEXTURE_2D,  m_win_pixmap );
        NORMAL_EX_LOG("");
        gl_create_image(pixmap);
        NORMAL_EX_LOG("");
        glBindTexture(pResource->target, 0);
        NORMAL_EX_LOG("");
      gl_destroy_image();
        NORMAL_EX_LOG("");
        // struct gs_texture *texture = gl_egl_create_texture_from_eglimage(
        // 	egl_display, width, height, color_format, target, image);
    //     eglDestroyImage(egl_display, image);
        NORMAL_EX_LOG("");
        std::vector<std::vector<uint8_t>> vPacket;
        enc.EndEncode(vPacket);
        NORMAL_EX_LOG("vPacket.size() = %u", vPacket.size());
            nFrame += (int)vPacket.size();
        for (std::vector<uint8_t> &packet : vPacket)
        {
            fpOut.write(reinterpret_cast<char*>(packet.data()), packet.size());
            //fpOut.out.flush();
        }

        return true;
    }
}