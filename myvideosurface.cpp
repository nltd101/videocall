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
         cp=cp.scaled(400, 500, Qt::KeepAspectRatio);
        cloneFrame.unmap();
//          cout<<strlen((char*)cp.bits())<<endl;
        //utils u();
        //int size;
        //u.deepCopyImageData(size,cp);

        cp = cp.mirrored(true, false);
        char* data= (char*)cp.bits();
   //     char data[30]="12345678910111213";
        if (this->service!=NULL&&this->service->isRunning()==true)
        {
             cout<<"send: "<<strlen(data)<<endl;
             this->service->send(data);
        }


        QPixmap pm = QPixmap::fromImage(cp);
        ui->label->setPixmap(pm.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        return true;
    }
    return false;
}

