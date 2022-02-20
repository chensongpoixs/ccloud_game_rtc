#if defined(_MSC_VER)
#pragma once
#include <osg/Camera>
#include <mutex>
#include <atomic>

 
class CaptureScreen :public osg::Camera::DrawCallback
{
public:
	CaptureScreen();
	~CaptureScreen();
	 
	virtual void operator () (const osg::Camera& camera) const;
	static CaptureScreen*Get();
	void setWindow(int ix, int iy, int iw, int ih);
	osg::ref_ptr<osg::Image>image();

	 
private:
	static CaptureScreen* _cameraDrawCallback;
	osg::ref_ptr<osg::Image>_image = nullptr;
	 
	int _ix = 0, _iy = 0, _iw = 1920, _ih = 1080;
	
};



#endif