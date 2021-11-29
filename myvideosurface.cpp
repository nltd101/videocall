#include "myvideosurface.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <fstream>

using namespace std;
MyVideoSurface::MyVideoSurface(QObject *parent, Ui::MainWindow *ui, QCamera *camera, UdpService *service) : QAbstractVideoSurface(parent)
{
    this->service=service;
    this->ui = ui;
    this->camera = camera;

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
         cp=cp.scaled(WIDTH, HEIGHT, Qt::KeepAspectRatio);

        cloneFrame.unmap();

        cp = cp.mirrored(true, false);

        char* data= (char*)cp.bits();

        if (this->service!=NULL&&this->service->isRunning()==true)
        {
             this->service->sendFrame(data);
        }

        QImage q = QImage((uchar*)data,WIDTH,HEIGHT,QImage::Format_RGB32);
        QPixmap pm = QPixmap::fromImage(q);
        ui->label->setPixmap(pm.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        return true;
    }
    return false;
}

