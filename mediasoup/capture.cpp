#include "windows.h"

#include "capture.h"
#include <osg/LightSource>
#include <chrono>
#include <iostream>
#include  "desktop_capture.h"
#include "rtc_base/logging.h"


static std::mutex					g_lock;
static std::atomic<bool>			m_new_frame = false;
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
	: m_callback(nullptr)
{
	this->_image = new osg::Image;
	m_rgba_ptr = new unsigned char[sizeof(unsigned char) * (1920 * 1080 * 60)];
	
}

CaptureScreen::~CaptureScreen()
{
}
//#include <osgDB/ReadFile>
//#include <osgUtil/Optimizer>
//#include <osgDB/WriteFile>
//#include <osgGA/TrackballManipulator>
static std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
static std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
uint64_t cnt = 1;
void CaptureScreen::operator () (const osg::Camera& camera) const
{
	
	
	start_time = std::chrono::steady_clock::now();
	std::chrono::steady_clock::duration diff = start_time - end_time ;
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
	//std::cout << "operator () diff = " << ms.count() << " ms " << std::endl;
	
	{
		std::lock_guard<std::mutex> lock(g_lock);
		this->_image->readPixels(this->_ix, this->_iy, this->_iw, this->_ih, GL_BGRA, GL_UNSIGNED_BYTE);
		_image->flipVertical(); // 进行RGBA翻转一下哈
		/*char filename[128];
		sprintf(filename, "./png/ScreenShot%d.png", cnt++);
		osgDB::writeImageFile(*_image, filename);*/
		m_new_frame.store(true);
	}
	
	 end_time = std::chrono::steady_clock::now();
	  diff = end_time - start_time;		
	//start_time = end_time;
	 ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
	//std::cout << "read pixels diff = " << ms.count() << " ms " << std::endl;
	/*this->_image->data();
	this->_image->s();
	this->_image->t();*/
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



void CaptureScreen::CaptureFrame()
{

	webrtc::DesktopCapturer::Result result = webrtc::DesktopCapturer::Result::SUCCESS;
	
	if (m_callback)
	{
		if (!m_new_frame.load())
		{
			return;
		}
		std::lock_guard<std::mutex> lock(g_lock);

		std::chrono::steady_clock::time_point start_time1 = std::chrono::steady_clock::now();
		
		m_callback->OnOsgCaptureResult(result, _image->data(), _iw, _ih);
		std::chrono::steady_clock::time_point end_time1 = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration diff = start_time1 - end_time1 ;
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
		if (ms.count() > 5)
		{
			RTC_LOG(LS_INFO) << "OnOsgCaptureResult () diff = " << ms.count() << " ms ";
		}
		m_new_frame.store(false);
		
	}
}
lightCallBack::lightCallBack(osg::Vec3d vpos1, osg::Vec3d vpos2)
{
	this->pos1 = vpos1;
	this->pos2 = vpos2;
}

lightCallBack::~lightCallBack()
{
}

void lightCallBack::operator()(osg::Node * node, osg::NodeVisitor * nv)
{
	osg::LightSource*ls = dynamic_cast<osg::LightSource*>(node);
	if (ls)
	{
		if (this->para > 1.0)
		{
			this->add = false;
		}
		else if (this->para < 0.0)
		{
			this->add = true;
		}
		if (this->add)
		{
			this->para += 0.01;
		}
		else
		{
			this->para -= 0.01;
		}
		osg::Vec3d lightPos = this->pos1*this->para + this->pos2*(1 - this->para);
		ls->getLight()->setPosition(osg::Vec4d(lightPos, 1.0));
	}
	traverse(node, nv);
}


void CaptureScreen::setCallback(DesktopCapture *callback)
{
	m_callback = callback;
}