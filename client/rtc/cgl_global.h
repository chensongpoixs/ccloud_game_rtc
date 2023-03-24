/***********************************************************************************************
created: 		2022-08-17

author:			chensong

purpose:		linux_app_capture
************************************************************************************************/


#ifndef C_GL_GLOBAL_H
#define C_GL_GLOBAL_H
 #include "nvEncodeAPI.h"
 #include <stdio.h>
#include <stdlib.h>
namespace chen {

    bool init_gl();

    bool encodec_gl(int width, int height, void * pixmap);

    void destroy_gl();
    // extern EGLDisplay eglDisplay ;
     void gl_egl_create_texture_from_pixmap(NV_ENC_INPUT_RESOURCE_OPENGL_TEX* pResource,   uint32_t width, uint32_t height, uint32_t color_format, uint32_t target, void * pixmap);
}

#endif // C_GL_GLOBAL_H