/***********************************************************************************************
				created: 		2019-03-01

				author:			chensong

				purpose:		net_types
************************************************************************************************/

#ifndef _C_GL_EGL_COMMON_H_
#define _C_GL_EGL_COMMON_H_
//#include "cnet_type.h"
#if defined(__linux__)
#include <stdio.h>
#include <stdlib.h>
#include "glad/glad_egl.h"
#include "glad/glad.h"
 #include "nvEncodeAPI.h"
 
namespace chen {

    void gl_egl_create_texture_from_pixmap(NV_ENC_INPUT_RESOURCE_OPENGL_TEX* pResource,  uint32_t width, uint32_t height, uint32_t color_format, EGLint target, void * pixmap);
// namespace chen {
//bool init_egl_image_target_texture_2d_ext(void);
}
// }
#endif // 
#endif // _C_GL_EGL_COMMON_H_