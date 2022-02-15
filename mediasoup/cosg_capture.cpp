#if defined(_MSC_VER)



#include "windows.h"
#include "cosg_capture.h"
#include <osg/LightSource>
#include <chrono>
#include <iostream>
#include  "osgdesktop_capture.h"
#include "rtc_base/logging.h"
#include "cinput_device.h"
#include "cinput_device.h"
#include "clog.h"
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
	, m_data_callback(true)
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

	{
		using namespace chen;
		s_input_device.set_point(_iw, _ih);
	}
	webrtc::DesktopCapturer::Result result = webrtc::DesktopCapturer::Result::SUCCESS;

	if (m_callback && m_callback)
	{
		m_callback->OnOsgCaptureResult(result, _image->data(), _iw, _ih);
	}
	static auto timestamp =
		std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch())
		.count();
	static size_t cnt = 0;

	cnt++;
	auto timestamp_curr = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch())
		.count();
	if (timestamp_curr - timestamp > 1000)
	{
		using namespace chen;
		NORMAL_LOG("FPS = %u ", cnt);
		//RTC_LOG(LS_INFO) << "FPS: " << cnt << ", " << frame->rect().width() << ", " << frame->rect().height();
		//RTC_LOG(LS_INFO) << "[width = " << frame->size().width() << "][height = " << frame->size().height() << "]";
		cnt = 0;
		timestamp = timestamp_curr;
	}
 
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

	//webrtc::DesktopCapturer::Result result = webrtc::DesktopCapturer::Result::SUCCESS;

	//if (m_callback)
	//{
	//	if (!m_new_frame.load())
	//	{
	//		return;
	//	}
	//	std::lock_guard<std::mutex> lock(g_lock);

	//	std::chrono::steady_clock::time_point start_time1 = std::chrono::steady_clock::now();

	//	m_callback->OnOsgCaptureResult(result, m_rgba_ptr, _iw, _ih);
	//	std::chrono::steady_clock::time_point end_time1 = std::chrono::steady_clock::now();
	//	std::chrono::steady_clock::duration diff = start_time1 - end_time1 ;
	//	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
	//	if (ms.count() > 5)
	//	{
	//		RTC_LOG(LS_INFO) << "OnOsgCaptureResult () diff = " << ms.count() << " ms ";
	//	}
	//	m_new_frame.store(false);

	//}
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


void CaptureScreen::setCallback(OsgDesktopCapture *callback)
{
	m_callback = callback;
}
#endif
