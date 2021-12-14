#include "myvideosurface.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <fstream>

using namespace std;
MyVideoSurface::MyVideoSurface(QObject *parent, QCamera *camera, UdpService *service) : QAbstractVideoSurface(parent)
{

    _service = service;
    _camera = camera;
    _context = (QMainWindow *)parent;
}

MyVideoSurface::~MyVideoSurface()
{
}

QList<QVideoFrame::PixelFormat> MyVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
	if (type == QAbstractVideoBuffer::NoHandle)
	{
		return QList<QVideoFrame::PixelFormat>()
			   << QVideoFrame::Format_RGB32;
	}
	else
	{
		return QList<QVideoFrame::PixelFormat>();
	}
}
void MyVideoSurface::setOnMyFrameListener(void *context, void (*onNewFrame)(void *, QImage))
{
    _contextNewFrame = context;
    _onNewFrame = onNewFrame;
}

bool MyVideoSurface::present(const QVideoFrame &frame)
{
	if (frame.isValid())
	{

		QVideoFrame cloneFrame(frame);
		cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
		const QImage img(cloneFrame.bits(),
						 cloneFrame.width(),
						 cloneFrame.height(),
						 QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));

		QImage cp = img.copy();

		cloneFrame.unmap();
		cp = cp.mirrored(true, false);

        if (_onNewFrame)
            _onNewFrame(_contextNewFrame, cp);
		//        delete q;

		return true;
	}
	return false;
}
