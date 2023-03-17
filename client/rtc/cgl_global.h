/***********************************************************************************************
created: 		2022-08-17

author:			chensong

purpose:		linux_app_capture
************************************************************************************************/


#ifndef C_GL_GLOBAL_H
#define C_GL_GLOBAL_H

namespace chen {

    bool init_gl();

    bool encodec_gl(int width, int height, void * pixmap);

    void destroy_gl();
    // extern EGLDisplay eglDisplay ;
}

#endif // C_GL_GLOBAL_H