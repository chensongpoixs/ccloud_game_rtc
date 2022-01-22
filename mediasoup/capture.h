#if defined(_MSC_VER)
#pragma once
#include <osg/Camera>
#include <mutex>
#include <atomic>


class DesktopCapture;

class CaptureScreen :public osg::Camera::DrawCallback
{
public:
	CaptureScreen();
	~CaptureScreen();
	void setCallback(DesktopCapture *callback);
	virtual void operator () (const osg::Camera& camera) const;
	static CaptureScreen*Get();
	void setWindow(int ix, int iy, int iw, int ih);
	osg::ref_ptr<osg::Image>image();

	void CaptureFrame();
	void set_m_data_callback(bool callback) { m_data_callback = callback; }
private:
	static CaptureScreen* _cameraDrawCallback;
	osg::ref_ptr<osg::Image>_image = nullptr;
	
	
	unsigned char *				m_rgba_ptr;
	int _ix = 0, _iy = 0, _iw = 1920, _ih = 1080;
	DesktopCapture * m_callback;
	bool						m_data_callback;
};


class lightCallBack :public osg::NodeCallback
{
public:
	lightCallBack(osg::Vec3d pos1, osg::Vec3d pos2);
	~lightCallBack();
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	osg::Vec3d pos1, pos2;
	double para = 0.0;
	bool add = true;
};
#endif