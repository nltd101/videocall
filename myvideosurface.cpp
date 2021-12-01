#include "myvideosurface.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <fstream>

using namespace std;
MyVideoSurface::MyVideoSurface(QObject *parent, QCamera *camera, UdpService *service, void (*onNewFrame)(QMainWindow *, QImage)) : QAbstractVideoSurface(parent)
{

    this->service = service;
    this->onNewFrame = onNewFrame;
    this->camera = camera;
    this->context = (QMainWindow *)parent;
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
        cp = cp.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);

        cloneFrame.unmap();

        cp = cp.mirrored(true, false);

        char *data = (char *)cp.bits();

        if (this->service != NULL && this->service->isRunning() == true)
        {
            this->service->sendFrame(data, strlen(data));
        }

        //        QImage q = cp;//QImage((uchar*)data,WIDTH,HEIGHT,QImage::Format_RGB32);

        this->onNewFrame(this->context, cp);

        return true;
    }
    return false;
}
