#if defined(_MSC_VER)



#include "windows.h"
#include "cosg_capture.h"
#include <osg/LightSource>
#include <chrono>
#include <iostream>
 
#include "rtc_base/logging.h"
#include "cinput_device.h"
#include "cinput_device.h"
#include "clog.h"
#include "cclient.h"
CaptureScreen*CaptureScreen::_cameraDrawCallback = nullptr;
 
CaptureScreen*CaptureScreen::Get()
{
	if (!_cameraDrawCallback)
	{
		_cameraDrawCallback = new CaptureScreen();
	}
	return _cameraDrawCallback;
}

CaptureScreen::CaptureScreen()
	 
{
	this->_image = new osg::Image;
	 

}

CaptureScreen::~CaptureScreen()
{
}
 
void CaptureScreen::operator () (const osg::Camera& camera) const
{
 
	this->_image->readPixels(this->_ix, this->_iy, this->_iw, this->_ih, GL_BGRA, GL_UNSIGNED_BYTE);
	//_image->flipVertical(); // 进行RGBA翻转一下哈

	s_client.webrtc_video(_image->data(), _iw, _ih);

}

void CaptureScreen::setWindow(int ix, int iy, int iw, int ih)
{
	this->_ix = ix;
	this->_iy = iy;
	this->_iw = iw;
	this->_ih = ih;
}

osg::ref_ptr<osg::Image> CaptureScreen::image()
{
	//this->_image->data();
	return this->_image;
}



#endif
