/***********************************************************************************************
				created: 		2019-03-01

				author:			chensong

				purpose:		net_types
************************************************************************************************/

 
#include "cgl_egl_common.h"
#include "clog.h"
#if defined(__linux__)
#include <stdio.h>
#include <stdlib.h>
#include "glad/glad_egl.h"
#include "glad/glad.h"
#include <linux/types.h>
#include <string.h>

#include <asm/ioctl.h>


namespace chen { 
typedef unsigned int drm_handle_t;

// #define bool int
// #define  false 0
// #define  true 1


typedef void(APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(
	GLenum target, GLeglImageOES image);
static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;



static const char *gl_error_to_str(GLenum errorcode)
{
	static const struct {
		GLenum error;
		const char *str;
	} err_to_str[] = {
		{
			GL_INVALID_ENUM,
			"GL_INVALID_ENUM",
		},
		{
			GL_INVALID_VALUE,
			"GL_INVALID_VALUE",
		},
		{
			GL_INVALID_OPERATION,
			"GL_INVALID_OPERATION",
		},
		{
			GL_INVALID_FRAMEBUFFER_OPERATION,
			"GL_INVALID_FRAMEBUFFER_OPERATION",
		},
		{
			GL_OUT_OF_MEMORY,
			"GL_OUT_OF_MEMORY",
		},
		{
			GL_STACK_UNDERFLOW,
			"GL_STACK_UNDERFLOW",
		},
		{
			GL_STACK_OVERFLOW,
			"GL_STACK_OVERFLOW",
		},
	};
	for (size_t i = 0; i < sizeof(err_to_str) / sizeof(*err_to_str); i++) {
		if (err_to_str[i].error == errorcode)
			return err_to_str[i].str;
	}
	return "Unknown";
}


static const char *gl_egl_error_to_string(EGLint error_number)
{
	switch (error_number) {
	case EGL_SUCCESS:
		return "The last function succeeded without error.";
		break;
	case EGL_NOT_INITIALIZED:
		return "EGL is not initialized, or could not be initialized, for the specified EGL display connection.";
		break;
	case EGL_BAD_ACCESS:
		return "EGL cannot access a requested resource (for example a context is bound in another thread).";
		break;
	case EGL_BAD_ALLOC:
		return "EGL failed to allocate resources for the requested operation.";
		break;
	case EGL_BAD_ATTRIBUTE:
		return "An unrecognized attribute or attribute value was passed in the attribute list.";
		break;
	case EGL_BAD_CONTEXT:
		return "An EGLContext argument does not name a valid EGL rendering context.";
		break;
	case EGL_BAD_CONFIG:
		return "An EGLConfig argument does not name a valid EGL frame buffer configuration.";
		break;
	case EGL_BAD_CURRENT_SURFACE:
		return "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid.";
		break;
	case EGL_BAD_DISPLAY:
		return "An EGLDisplay argument does not name a valid EGL display connection.";
		break;
	case EGL_BAD_SURFACE:
		return "An EGLSurface argument does not name a valid surface (window, pixel buffer or pixmap) configured for GL rendering.";
		break;
	case EGL_BAD_MATCH:
		return "Arguments are inconsistent (for example, a valid context requires buffers not supplied by a valid surface).";
		break;
	case EGL_BAD_PARAMETER:
		return "One or more argument values are invalid.";
		break;
	case EGL_BAD_NATIVE_PIXMAP:
		return "A NativePixmapType argument does not refer to a valid native pixmap.";
		break;
	case EGL_BAD_NATIVE_WINDOW:
		return "A NativeWindowType argument does not refer to a valid native window.";
		break;
	case EGL_CONTEXT_LOST:
		return "A power management event has occurred. The application must destroy all contexts and reinitialise OpenGL ES state and objects to continue rendering. ";
		break;
	default:
		return "Unknown error";
		break;
	}
}


static bool find_gl_extension(const char *extension)
{
	return true;

	//.int gltIsExtSupported(const char * extension);

	// int count =    gltIsExtSupported("GL_OES_EGL_image");
	// NORMAL_EX_LOG("gltIsExtSupported === count = %u", count);
	GLint n = 0, i = 0;
	//NORMAL_EX_LOG("");
	//glGetIntegerv(GL_NUM_EXTENSIONS, &n);
	//NORMAL_EX_LOG("");
	for (i = 0; i < n; i++) 
	{
		const char *e = (char *)glGetStringi(GL_EXTENSIONS, i);
		if (extension && strcmp(e, extension) == 0)
		{
			return true;
		}
	}
	return false;
}

 static  inline bool gl_success(const char *funcname)
{
	GLenum errorcode = glGetError();
	if (errorcode != GL_NO_ERROR) {
		int attempts = 8;
		do {
			
			     printf("[%s][%d][ERROR]%s failed, glGetError returned %s(0x%X)", __FUNCTION__, __LINE__, funcname, gl_error_to_str(errorcode), errorcode);
			errorcode = glGetError();

			--attempts;
			if (attempts == 0) {
				printf("[%s][%d][ERROR]Too many GL errors, moving on", __FUNCTION__, __LINE__);
				break;
			}
		} while (errorcode != GL_NO_ERROR);
		return false;
	}

	return true;
}
 

 static bool init_egl_image_target_texture_2d_ext( )
{
	static bool initialized = false;
	NORMAL_EX_LOG("");
	if (!initialized) 
    {
		initialized = true;

		if (!find_gl_extension("GL_OES_EGL_image")) 
        {
			ERROR_EX_LOG("[%s][%d][ERROR]No GL_OES_EGL_image\n", __FUNCTION__, __LINE__);
			return false;
		}
		NORMAL_EX_LOG("");
		glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress( "glEGLImageTargetTexture2DOES");
	}
	NORMAL_EX_LOG("glEGLImageTargetTexture2DOES = %p", glEGLImageTargetTexture2DOES);
	if (!glEGLImageTargetTexture2DOES)
	{
		ERROR_EX_LOG("");
        return false;
    }
	NORMAL_EX_LOG("");
	return true;
}


  void gl_egl_create_texture_from_pixmap(  uint32_t width, uint32_t height, uint32_t color_format, EGLint target, void * pixmap)
{
	NORMAL_EX_LOG("");
    if (!init_egl_image_target_texture_2d_ext())
	{
        return ;
    }
	NORMAL_EX_LOG("");
	const EGLAttrib pixmap_attrs[] = {
		EGL_IMAGE_PRESERVED_KHR,
		EGL_TRUE,
		EGL_NONE,
	};

    EGLDisplay egl_display = eglGetCurrentDisplay();
    if (!egl_display)
    {
        printf("[%s][%d][egl_display]\n", __FUNCTION__, __LINE__);
        return ;
    }
NORMAL_EX_LOG("");
	EGLImage image = eglCreateImage(egl_display, EGL_NO_CONTEXT,
					EGL_NATIVE_PIXMAP_KHR, pixmap, pixmap_attrs);
	if (image == EGL_NO_IMAGE) 
    {
		printf("[%s][%d][ERROR]Cannot create EGLImage: %s",__FUNCTION__, __LINE__,
		     gl_egl_error_to_string(eglGetError()));
		return ;
	}
NORMAL_EX_LOG("");
    glEGLImageTargetTexture2DOES(EGL_TEXTURE_2D, image);
	if (!gl_success("glEGLImageTargetTexture2DOES")) 
    {
        printf("[%s][%d][ERROR]\n", __FUNCTION__, __LINE__);
		//gs_texture_destroy(texture);
		//texture = NULL;
	}


NORMAL_EX_LOG("");
	// struct gs_texture *texture = gl_egl_create_texture_from_eglimage(
	// 	egl_display, width, height, color_format, target, image);
	eglDestroyImage(egl_display, image);

	return ;
}
}
#endif 